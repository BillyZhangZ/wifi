/**
 * @file
 */
/******************************************************************************
 * Copyright 2013-2014, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/

#define AJ_MODULE NET

#include <assert.h>
#include <qca_includes.h>
#include <stdlib.h>
#include <atheros_stack_offload.h>
#include <custom_stack_offload.h>
#include <common_stack_offload.h>

#include "aj_target.h"
#include "aj_bufio.h"
#include "aj_net.h"
#include "aj_util.h"
#include "aj_crypto.h"
#include "aj_debug.h"

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
uint8_t dbgNET = 0;
#endif

#ifdef htonl
#undef htonl
#define htonl A_CPU2BE32
#endif

#ifndef INADDR_ANY
#define INADDR_ANY (0)
#endif

typedef struct {
    int l_onoff;
    int l_linger;
} AJ_Linger;

static char* AddrStr(uint32_t addr)
{
    static char txt[16];
    return inet_ntoa(htonl(addr), txt);
}

extern int Inet6Pton(char* src, void* dst);

/*
 * IANA assigned IPv4 multicast group for AllJoyn.
 */
static const char AJ_IPV4_MULTICAST_GROUP[] = "224.0.0.113";

/*
 * IANA assigned IPv6 multicast group for AllJoyn.
 */
static const char AJ_IPV6_MULTICAST_GROUP[] = "ff02::13a";

/*
 * IANA assigned UDP multicast port for AllJoyn
 */
#define AJ_UDP_PORT 9956

/*
 * Using a static receive buffer for now
 */
#define STATIC_RX_BUF

#define YIELD_CPU() AJ_Sleep(5)

#ifdef STATIC_RX_BUF
static uint8_t RxBuffer[1454];
#endif

/**
 * Target-specific context for network I/O
 */
typedef struct {
    int tcpSock;
    int udpSock;
    int udp6Sock;
} NetContext;

static NetContext netContext;

uint16_t AJ_EphemeralPort(void)
{
    uint8_t bytes[2];
    AJ_RandBytes(bytes, 2);
    /*
     * Return a random port number in the IANA-suggested range
     */
    return 49152 + *((uint16_t*)bytes) % (65535 - 49152);
}

static AJ_Status ConfigNetSock(AJ_NetSocket* netSock, AJ_RxFunc rxFunc, uint32_t rxLen, AJ_TxFunc txFunc, uint32_t txLen)
{
    uint8_t* txData = NULL;
    uint8_t* rxData = NULL;

    memset(netSock, 0, sizeof(AJ_NetSocket));
    memset(&netContext, 0, sizeof(NetContext));

    if (rxLen) {
#ifndef STATIC_RX_BUF
        while (!rxData) {
            YIELD_CPU();
            rxData = (uint8_t*)A_MALLOC(rxLen, MALLOC_ID_CONTEXT);
        }
#else
        if (rxLen > sizeof(RxBuffer)) {
            AJ_ErrPrintf(("ConfigNetSock(): Asking for %u bytes but buffer is only %u", rxLen, sizeof(RxBuffer)));
            return AJ_ERR_RESOURCES;
        }
        rxData = RxBuffer;
#endif
    }
    AJ_IOBufInit(&netSock->rx, rxData, rxLen, AJ_IO_BUF_RX, (void*)&netContext);
    netSock->rx.recv = rxFunc;
    while (!txData) {
        YIELD_CPU();
        txData = (uint8_t*)CUSTOM_ALLOC(txLen);
    }
    AJ_IOBufInit(&netSock->tx, txData, txLen, AJ_IO_BUF_TX, (void*)&netContext);
    netSock->tx.send = txFunc;
    return AJ_OK;
}

static AJ_Status CloseNetSock(AJ_NetSocket* netSock)
{
    NetContext* context = (NetContext*)netSock->rx.context;
    if (context) {
        /*
         * Free the TX and RX buffers using appropriate free calls
         */
        if (netSock->tx.bufStart) {
            CUSTOM_FREE(netSock->tx.bufStart);
        }
#ifndef STATIC_RX_BUF
        if (netSock->rx.bufStart) {
            A_FREE(netSock->rx.bufStart, MALLOC_ID_CONTEXT);
        }
#endif
        if (context->tcpSock) {
            t_shutdown((void*)handle, context->tcpSock);
        }
        if (context->udpSock) {
            t_shutdown((void*)handle, context->udpSock);
        }
        if (context->udp6Sock) {
            t_shutdown((void*)handle, context->udp6Sock);
        }
        memset(context, 0, sizeof(NetContext));
        memset(netSock, 0, sizeof(AJ_NetSocket));
    }
    return AJ_OK;
}

/*
 * Send always writes the entire I/O buffer
 */
AJ_Status AJ_Net_Send(AJ_IOBuffer* txBuf)
{
    NetContext* context = (NetContext*)txBuf->context;
    uint32_t tx = AJ_IO_BUF_AVAIL(txBuf);
    AJ_InfoPrintf(("AJ_Net_Send(buf=0x%p)\n", txBuf));

    assert(txBuf->readPtr == txBuf->bufStart);

    while (tx) {
        int32_t ret = t_send((void*)handle, context->tcpSock, txBuf->readPtr, tx, 0);
        if (ret < 0) {
            AJ_ErrPrintf(("AJ_Net_Send(): send() failed, status=AJ_ERR_WRITE\n"));
            return AJ_ERR_WRITE;
        }
        txBuf->readPtr += ret;
        tx -= ret;
    }
    AJ_IO_BUF_RESET(txBuf);
    AJ_InfoPrintf(("AJ_Net_Send(): status=AJ_OK\n"));
    return AJ_OK;
}

/*
 * The socket that is blocked in select
 */
static uint32_t selectSock;
static uint8_t interrupted;

/*
 * This function can be called from an interrupt context to cancel a pending select.  It uses
 * knowledge of the internals of the QCA stack offload code to get the appropriate MQX event pointer
 * and unblock the select.
 */
void AJ_Net_Interrupt()
{
    if (selectSock) {
        size_t index = find_socket_context(selectSock, TRUE);
        interrupted = TRUE;
        unblock(ath_sock_context[index], 0 /* RX_DIRECTION */);
    }
}

AJ_Status AJ_Net_Recv(AJ_IOBuffer* rxBuf, uint32_t len, uint32_t timeout)
{
    NetContext* context = (NetContext*)rxBuf->context;
    AJ_Status status = AJ_OK;
    uint32_t rx = AJ_IO_BUF_SPACE(rxBuf);
    int ret;
    uint8_t* zeroCopy;

    AJ_InfoPrintf(("AJ_Net_Recv(buf=0x%p, len=%d., timeout=%d.)\n", rxBuf, len, timeout));
    rx = min(rx, len);

    selectSock = context->tcpSock;
    interrupted = FALSE;
    ret = t_select((void*)handle, context->tcpSock, timeout);
    selectSock = 0;
    if (ret == A_ERROR) {
        if (interrupted) {
            return AJ_ERR_INTERRUPTED;
        } else {
            return AJ_ERR_TIMEOUT;
        }
    } else if (ret == A_SOCK_INVALID) {
        AJ_ErrPrintf(("AJ_Net_Recv(): t_select returns %d\n", ret));
        return AJ_ERR_READ;
    }

    ret = t_recv((void*)handle, context->tcpSock, &zeroCopy, rx, 0);
    if (ret < 0) {
        AJ_ErrPrintf(("AJ_Net_Recv(): recv() failed, status=AJ_ERR_READ\n"));
        status = AJ_ERR_READ;
    } else {
        AJ_ASSERT(ret <= rx);
        AJ_InfoPrintf(("AJ_Net_Recv(): Read %u bytes; asked for %u\n", ret, rx));
        memcpy(rxBuf->writePtr, zeroCopy, ret);
        zero_copy_free(zeroCopy);
        rxBuf->writePtr += ret;
    }

    AJ_InfoPrintf(("AJ_Net_Recv(): status=%s\n", AJ_StatusText(status)));
    return status;
}

/*
 * port is in CPU order, address is in network order
 */
AJ_Status AJ_Net_Connect(AJ_NetSocket* netSock, uint16_t port, uint8_t addrType, const uint32_t* address)
{
    AJ_Status status;
    int ret;
    int addrLen;
    SOCKADDR_T sin;
    SOCKADDR_6_T sin6;
    int sock;
    void* addr;
    AJ_Linger ling;

    AJ_InfoPrintf(("AJ_Net_Connect(netSock=0x%p, port=%d., addrType=%d., addr=0x%p)\n", netSock, port, addrType, address));

    if (addrType == AJ_ADDR_IPV4) {
        memset(&sin, 0, sizeof(sin));
        sock = t_socket((void*)handle, ATH_AF_INET, SOCK_STREAM_TYPE, 0);
        sin.sin_family = ATH_AF_INET;
        sin.sin_port = port;
        sin.sin_addr = htonl(*address);
        addrLen = sizeof(sin);
        addr = &sin;
    } else {
        memset(&sin6, 0, sizeof(sin6));
        sock = t_socket((void*)handle, ATH_AF_INET6, SOCK_STREAM_TYPE, 0);
        sin6.sin6_family = ATH_AF_INET6;
        sin6.sin6_port = port;
        memcpy(&sin6.sin6_addr, address, 16);
        addrLen = sizeof(sin6);
        addr = &sin6;
    }
    if (sock == A_SOCK_INVALID) {
        AJ_ErrPrintf(("t_socket: failed to create socket\n"));
        return AJ_ERR_CONNECT;
    }
    ret = t_connect((void*)handle, sock, addr, addrLen);
    if (ret < 0) {
        AJ_ErrPrintf(("AJ_Net_Connect() to %s/%d failed: %d\n", AddrStr(sin.sin_addr), port, ret));
        t_shutdown((void*)handle, sock);
        status = AJ_ERR_CONNECT;
    } else {
        AJ_InfoPrintf(("AJ_Net_Connect() connected to %s/%d\n", AddrStr(sin.sin_addr), port));
        status = ConfigNetSock(netSock, AJ_Net_Recv, 1024, AJ_Net_Send, 1500);
        if (status == AJ_OK) {
            ((NetContext*)netSock->rx.context)->tcpSock = sock;
        }
    }
    ling.l_onoff = 1;
    ling.l_linger = 0;
    ret = t_setsockopt((void*)handle, sock, ATH_IPPROTO_IP, SO_LINGER, (void*)&ling, sizeof(ling));
    if (ret < 0) {
        AJ_ErrPrintf(("CloseNetSock(): t_setsockopt(SO_LINGER) failed with error %d\n", ret));
    }
    return status;
}

void AJ_Net_Disconnect(AJ_NetSocket* netSock)
{
    AJ_InfoPrintf(("AJ_Net_Disconnect(nexSock=0x%p)\n", netSock));
    CloseNetSock(netSock);
}

static AJ_Status SendTo(AJ_IOBuffer* txBuf)
{
    NetContext* context = (NetContext*)txBuf->context;
    int tx = AJ_IO_BUF_AVAIL(txBuf);
    assert(txBuf->readPtr == txBuf->bufStart);
    AJ_InfoPrintf(("AJ_Net_SendTo(buf=0x%p)\n", txBuf));
    if (tx) {
        int ret;
        /*
         * Multicast send over IPv4
         */
        if (context->udpSock != A_SOCK_INVALID)
        {
            SOCKADDR_T sin;
            memset(&sin, 0, sizeof(sin));
            sin.sin_family = ATH_AF_INET;
            sin.sin_port = AJ_UDP_PORT;
            ath_inet_aton(AJ_IPV4_MULTICAST_GROUP, &sin.sin_addr);
            ret = t_sendto((void*)handle, context->udpSock, txBuf->bufStart, tx, 0, &sin, sizeof(sin));
        }

        if (context->udpSock != A_SOCK_INVALID)
        { // Testing
            A_NETBUF* a_netbuf_ptr;
            a_netbuf_ptr = ((TX_PACKET_PTR)(txBuf->bufStart - TX_PKT_OVERHEAD))->a_netbuf_ptr;
            a_netbuf_ptr->head = a_netbuf_ptr->native.bufFragment[0].payload;
            a_netbuf_ptr->end = (pointer)((A_UINT32)a_netbuf_ptr->native.bufFragment[0].payload + a_netbuf_ptr->native.bufFragment[0].length);
            // reserve head room
            a_netbuf_ptr->tail = a_netbuf_ptr->data = (pointer)((A_UINT32)a_netbuf_ptr->head + AR6000_DATA_OFFSET);
        }

        /*
         * Broadcast send over IPv4
         */
        if (context->udpSock != A_SOCK_INVALID)
        {
            SOCKADDR_T sin;
            memset(&sin, 0, sizeof(sin));
            sin.sin_family = ATH_AF_INET;
            sin.sin_port = AJ_UDP_PORT;
            // the driver does not support subnet bcast so send to 255.255.255.255
            sin.sin_addr = 0xFFFFFFFF;
            if (sin.sin_addr != 0) {
                ret = t_sendto((void*)handle, context->udpSock, txBuf->bufStart, tx, 0, &sin, sizeof(sin));
            }
        }

        if (context->udpSock != A_SOCK_INVALID)
        { // Testing
            A_NETBUF* a_netbuf_ptr;
            a_netbuf_ptr = ((TX_PACKET_PTR)(txBuf->bufStart - TX_PKT_OVERHEAD))->a_netbuf_ptr;
            a_netbuf_ptr->head = a_netbuf_ptr->native.bufFragment[0].payload;
            a_netbuf_ptr->end = (pointer)((A_UINT32)a_netbuf_ptr->native.bufFragment[0].payload + a_netbuf_ptr->native.bufFragment[0].length);
            // reserve head room
            a_netbuf_ptr->tail = a_netbuf_ptr->data = (pointer)((A_UINT32)a_netbuf_ptr->head + AR6000_DATA_OFFSET);
        }


        /*
         * Multicast send over IPv6
         */
        if (context->udp6Sock != A_SOCK_INVALID)
        {
            SOCKADDR_6_T sin6;
            memset(&sin6, 0, sizeof(sin6));
            sin6.sin6_family = ATH_AF_INET6;
            sin6.sin6_port = AJ_UDP_PORT;
            Inet6Pton((char *)AJ_IPV6_MULTICAST_GROUP, &sin6.sin6_addr);
            ret = t_sendto((void*)handle, context->udp6Sock, txBuf->bufStart, tx, 0, &sin6, sizeof(sin6));
        }

        if (context->udp6Sock != A_SOCK_INVALID)
        { // Testing
            A_NETBUF* a_netbuf_ptr;
            a_netbuf_ptr = ((TX_PACKET_PTR)(txBuf->bufStart - TX_PKT_OVERHEAD))->a_netbuf_ptr;
            a_netbuf_ptr->head = a_netbuf_ptr->native.bufFragment[0].payload;
            a_netbuf_ptr->end = (pointer)((A_UINT32)a_netbuf_ptr->native.bufFragment[0].payload + a_netbuf_ptr->native.bufFragment[0].length);
            // reserve head room
            a_netbuf_ptr->tail = a_netbuf_ptr->data = (pointer)((A_UINT32)a_netbuf_ptr->head + AR6000_DATA_OFFSET);
        }

        if (ret < 0) {
            AJ_ErrPrintf(("AJ_Net_SendTo(): sendto() failed, status=AJ_ERR_WRITE\n"));
            return AJ_ERR_WRITE;
        }
    }
    AJ_IO_BUF_RESET(txBuf);
    return AJ_OK;
}


#define POLL_TIMEOUT  100

/*
 * We ignore the len parameter and read as much as we can
 */
static AJ_Status RecvFrom(AJ_IOBuffer* rxBuf, uint32_t len, uint32_t timeout)
{
    AJ_Status status = AJ_OK;
    NetContext* context = (NetContext*)rxBuf->context;
    int ret;
    int sock = context->udpSock != A_SOCK_INVALID ? context->udpSock : context->udp6Sock;
    uint32_t poll = min(POLL_TIMEOUT, timeout / 2); /* make sure we poll both sockets */

    AJ_InfoPrintf(("AJ_Net_RecvFrom(buf=0x%p, len=%d., timeout=%d.)\n", rxBuf, len, timeout));

    if (rxBuf->bufStart) {
        zero_copy_free(rxBuf->bufStart);
        rxBuf->bufStart = NULL;
    }
    while (1) {
        ret = t_select((void*)handle, sock, poll);
        if (ret != A_ERROR) {
            break;
        }
        /*
         * Toggle sockets
         */
        sock = (sock == context->udpSock) ? 
               (context->udp6Sock != A_SOCK_INVALID ? context->udp6Sock : context->udpSock) :
               (context->udpSock != A_SOCK_INVALID ? context->udpSock : context->udp6Sock);
        if (timeout < POLL_TIMEOUT) {
            AJ_ErrPrintf(("AJ_Net_RecvFrom(): select() timed out. status=AJ_ERR_TIMEOUT\n"));
            return AJ_ERR_TIMEOUT;
        }
        timeout -= POLL_TIMEOUT;
    }
    if (ret != A_OK) {
        return AJ_ERR_READ;
    }
    if (sock == context->udpSock) {
        /*
         * Receive from IPv4 UDP socket
         */
        SOCKADDR_T sin;
        uint32_t sinLen = sizeof(sin);
        ret = t_recvfrom((void*)handle, context->udpSock, &rxBuf->bufStart, 1500, 0, &sin, &sinLen);
        if (ret < 0) {
            AJ_ErrPrintf(("AJ_Net_RecvFrom(): t_recvfrom(%d) failed on IPV4. status=AJ_ERR_READ\n", context->udpSock));
            return AJ_ERR_READ;
        }
        AJ_InfoPrintf(("recvfrom IPv4 %d bytes\n", ret));
    } else {
        /*
         * Receive from IPv6 UDP socket
         */
        SOCKADDR_6_T sin6;
        uint32_t sin6Len = sizeof(sin6);
        ret = t_recvfrom((void*)handle, context->udp6Sock, &rxBuf->bufStart, 1500, 0, &sin6, &sin6Len);
        if (ret < 0) {
            AJ_ErrPrintf(("AJ_Net_RecvFrom(): t_recvfrom(%d) failed on IPV6. status=AJ_ERR_READ\n", context->udp6Sock));
            return AJ_ERR_READ;
        }
        AJ_InfoPrintf(("recvfrom IPv6 %d bytes\n", ret));
    }
    rxBuf->bufSize = ret;
    rxBuf->writePtr = rxBuf->bufStart + ret;
    rxBuf->readPtr = rxBuf->bufStart;
    return status;
}

static int MCastUp4()
{
    int ret;
    uint32_t groupAddr;
    IP_MREQ_T mreq;
    SOCKADDR_T sin;
    int sock;

    sock = t_socket((void*)handle, ATH_AF_INET, SOCK_DGRAM_TYPE, 0);
    if (sock == A_SOCK_INVALID) {
        AJ_ErrPrintf(("MCastUp4(): t_socket() failed\n"));
        return A_SOCK_INVALID;
    }
    /*
     * Bind our multicast port and local address
     */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = ATH_AF_INET;
    /*
     * bind the socket to the address with ephemeral port
     */
    sin.sin_port = AJ_EphemeralPort();
    sin.sin_addr = INADDR_ANY;
    ret = t_bind((void*)handle, sock, &sin, sizeof(sin));
    if (ret != A_OK) {
        AJ_ErrPrintf(("MCastUp4(): t_bind(%d) failed\n", sock));
        t_shutdown((void*)handle, sock);
        return A_SOCK_INVALID;
    }

    /*
     * Join the AllJoyn IPv4 multicast group
     */
    ath_inet_aton(AJ_IPV4_MULTICAST_GROUP, &groupAddr);
    mreq.imr_multiaddr = htonl(groupAddr);
    mreq.imr_interface = INADDR_ANY;
    ret = t_setsockopt((void*)handle, sock, ATH_IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
    if (ret != A_OK) {
        AJ_ErrPrintf(("MCastUp4(): t_setsockopt(%d) failed\n", sock));
        t_shutdown((void*)handle, sock);
        return A_SOCK_INVALID;
    }
    return sock;
}

static int MCastUp6()
{
    int ret;
    IPV6_MREQ_T group6;
    SOCKADDR_6_T sin6;
    int sock;
    uint8_t gblAddr[16];
    uint8_t locAddr[16];
    uint8_t gwAddr[16];
    uint8_t gblExtAddr[16];
    uint32_t linkPrefix;
    uint32_t glbPrefix;
    uint32_t gwPrefix;
    uint32_t glbExtPrefix;

    /*
     * We pass the current global IPv6 address into the sockopt for joining the multicast group.
     */
    //ret = t_ip6config(handle, IPCFG_QUERY, gblAddr, locAddr, gwAddr, gblExtAddr, &linkPrefix, &glbPrefix, &gwPrefix, &glbExtPrefix);
    ret = t_ip6config(handle, IPCFG_QUERY, (IP6_ADDR_T *)gblAddr, (IP6_ADDR_T *)locAddr, (IP6_ADDR_T *)gwAddr, (IP6_ADDR_T *)gblExtAddr, &linkPrefix, &glbPrefix, &gwPrefix, &glbExtPrefix);
    if (ret != A_OK) {
        AJ_ErrPrintf(("MCastUp6(): t_ip6config() failed\n"));
        return A_SOCK_INVALID;
    }
    /*
     * Create the IPv6 socket
     */
    sock = t_socket((void*)handle, ATH_AF_INET6, SOCK_DGRAM_TYPE, 0);
    if (sock == A_SOCK_INVALID) {
        AJ_ErrPrintf(("MCastUp6(): t_socket() failed\n"));
        return A_SOCK_INVALID;
    }
    /*
     * Bind to an emphemeral port
     */
    memset(&sin6, 0, sizeof(sin6));
    sin6.sin6_family = ATH_AF_INET6;
    sin6.sin6_port = AJ_EphemeralPort();
    ret = t_bind((void*)handle, sock, &sin6, sizeof(sin6));
    if (ret != A_OK) {
        AJ_ErrPrintf(("MCastUp6(): t_bind(%d) failed\n", sock));
        t_shutdown((void*)handle, sock);
        return A_SOCK_INVALID;
    }
    /*
     * Join the AllJoyn IPv6 multicast group
     */
    memset(&group6, 0, sizeof(group6));

    Inet6Pton((char *)AJ_IPV6_MULTICAST_GROUP, group6.ipv6mr_multiaddr.addr);
    memcpy(group6.ipv6mr_interface.addr, locAddr, sizeof(locAddr));

    ret = t_setsockopt((void*)handle, sock, ATH_IPPROTO_IP, IPV6_JOIN_GROUP, (char*)&group6, sizeof(group6));
    if (ret != A_OK) {
        AJ_ErrPrintf(("MCastUp6(): t_setsockopt(%d) failed\n", sock));
        t_shutdown((void*)handle, sock);
        return A_SOCK_INVALID;
    }
    return sock;
}

AJ_Status AJ_Net_MCastUp(AJ_NetSocket* netSock)
{
    AJ_Status status = AJ_ERR_READ;
    int udpSock = MCastUp4();
    int udp6Sock = MCastUp6();

    if ((udpSock != A_SOCK_INVALID) || (udp6Sock != A_SOCK_INVALID)) {
        status = ConfigNetSock(netSock, RecvFrom, 1454, SendTo, 471);
        if (status == AJ_OK) {
            ((NetContext*)netSock->rx.context)->udpSock = udpSock;
            ((NetContext*)netSock->rx.context)->udp6Sock = udp6Sock;
        }
    }
    if (status != AJ_OK) {
        CloseNetSock(netSock);
    }
    return status;
}

void AJ_Net_MCastDown(AJ_NetSocket* netSock)
{
    if (netSock->rx.bufStart) {
        zero_copy_free(netSock->rx.bufStart);
        netSock->rx.bufStart = NULL;
    }
    CloseNetSock(netSock);
}
