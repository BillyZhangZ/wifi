#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include <errno.h>  
#include <arpa/inet.h>

#define PORT 2390

struct mag3110_data{
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int msd;
    unsigned int parkingflag;
};

int main(int argc,char **argv)
{
    int ser_sockfd, cli_sockfd;
    int err,n;
    int addlen;
    struct sockaddr_in ser_addr;
    struct sockaddr_in cli_addr;
    struct mag3110_data mag3110data;
    char   buff[1024];

    ser_sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(ser_sockfd==-1) {
        printf("socket error:%s\n",strerror(errno));
        return -1;
    }

    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ser_addr.sin_port = htons(PORT);
    err = bind(ser_sockfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
    if(err==-1) {
        printf("bind error:%s\n",strerror(errno));
        return -1;
    }

    err = listen(ser_sockfd, 5);
    if(err==-1) {
        printf("listen error\n");
        return -1;
    }

    printf("listen the port:\n");

    while(1)
    {   
        addlen = sizeof(struct sockaddr);
        cli_sockfd = accept(ser_sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&addlen);
        if(cli_sockfd == -1) {
            printf("accept error\n");
        }
        memset(buff, 0, sizeof buff);
        printf("Connection from %s, port %d\n", inet_ntop(AF_INET, &cli_addr.sin_addr, buff, sizeof(buff)), ntohs(cli_addr.sin_port));
        while(1) {
            //printf("waiting for client...\n");
            n = recv(cli_sockfd, &mag3110data, sizeof(mag3110data), 0);
            if(n == -1) {
                printf("recv error\n");
                break;
            }else{
                printf("%d,%d,%d :%d   %d \n", mag3110data.x, mag3110data.y, mag3110data.z, mag3110data.msd, mag3110data.parkingflag);
            }
        }
        close(cli_sockfd);
    }

    close(ser_sockfd);

    return 0;
}
