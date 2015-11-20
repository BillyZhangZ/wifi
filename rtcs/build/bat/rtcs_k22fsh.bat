@echo off

rem convert path to backslash format
set ROOTDIR=%1
set ROOTDIR=%ROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3



rem copy common files
IF NOT EXIST "%OUTPUTDIR%\" mkdir "%OUTPUTDIR%\"
copy "%ROOTDIR%\rtcs\source\include\telnet.h" "%OUTPUTDIR%\telnet.h" /Y
copy "%ROOTDIR%\rtcs\source\include\fcs16.h" "%OUTPUTDIR%\fcs16.h" /Y
copy "%ROOTDIR%\rtcs\source\include\dhcpsrv.h" "%OUTPUTDIR%\dhcpsrv.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_base64.h" "%OUTPUTDIR%\rtcs_base64.h" /Y
copy "%ROOTDIR%\rtcs\source\include\igmp.h" "%OUTPUTDIR%\igmp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\httpsrv.h" "%OUTPUTDIR%\httpsrv.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ppphdlc.h" "%OUTPUTDIR%\ppphdlc.h" /Y
copy "%ROOTDIR%\rtcs\source\include\iwcfg.h" "%OUTPUTDIR%\iwcfg.h" /Y
copy "%ROOTDIR%\rtcs\source\include\dhcp.h" "%OUTPUTDIR%\dhcp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ppp.h" "%OUTPUTDIR%\ppp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\mld.h" "%OUTPUTDIR%\mld.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcscfg.h" "%OUTPUTDIR%\rtcscfg.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rip.h" "%OUTPUTDIR%\rip.h" /Y
copy "%ROOTDIR%\rtcs\source\include\addrinfo.h" "%OUTPUTDIR%\addrinfo.h" /Y
copy "%ROOTDIR%\rtcs\source\include\nat.h" "%OUTPUTDIR%\nat.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ip6.h" "%OUTPUTDIR%\ip6.h" /Y
copy "%ROOTDIR%\rtcs\source\include\asn1.h" "%OUTPUTDIR%\asn1.h" /Y
copy "%ROOTDIR%\rtcs\source\include\dns.h" "%OUTPUTDIR%\dns.h" /Y
copy "%ROOTDIR%\rtcs\source\include\chap.h" "%OUTPUTDIR%\chap.h" /Y
copy "%ROOTDIR%\rtcs\source\include\nat_session.h" "%OUTPUTDIR%\nat_session.h" /Y
copy "%ROOTDIR%\rtcs\source\include\arp.h" "%OUTPUTDIR%\arp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_sock.h" "%OUTPUTDIR%\rtcs_sock.h" /Y
copy "%ROOTDIR%\rtcs\source\include\bootp.h" "%OUTPUTDIR%\bootp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\icmp.h" "%OUTPUTDIR%\icmp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\snmp.h" "%OUTPUTDIR%\snmp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\arpif.h" "%OUTPUTDIR%\arpif.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ip-e.h" "%OUTPUTDIR%\ip-e.h" /Y
copy "%ROOTDIR%\rtcs\source\include\httpsrv_supp.h" "%OUTPUTDIR%\httpsrv_supp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\pap.h" "%OUTPUTDIR%\pap.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ftpsrv.h" "%OUTPUTDIR%\ftpsrv.h" /Y
copy "%ROOTDIR%\rtcs\source\include\dnscln.h" "%OUTPUTDIR%\dnscln.h" /Y
copy "%ROOTDIR%\rtcs\source\include\snmpcfg.h" "%OUTPUTDIR%\snmpcfg.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ip6_trace.h" "%OUTPUTDIR%\ip6_trace.h" /Y
copy "%ROOTDIR%\rtcs\source\include\pppfsm.h" "%OUTPUTDIR%\pppfsm.h" /Y
copy "%ROOTDIR%\rtcs\source\include\icmp6.h" "%OUTPUTDIR%\icmp6.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcstime.h" "%OUTPUTDIR%\rtcstime.h" /Y
copy "%ROOTDIR%\rtcs\source\include\tftp.h" "%OUTPUTDIR%\tftp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ftpsrv_msg.h" "%OUTPUTDIR%\ftpsrv_msg.h" /Y
copy "%ROOTDIR%\rtcs\source\include\route.h" "%OUTPUTDIR%\route.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ip6_if.h" "%OUTPUTDIR%\ip6_if.h" /Y
copy "%ROOTDIR%\rtcs\source\include\md5.h" "%OUTPUTDIR%\md5.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ipcp.h" "%OUTPUTDIR%\ipcp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\checksum.h" "%OUTPUTDIR%\checksum.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcspcb.h" "%OUTPUTDIR%\rtcspcb.h" /Y
copy "%ROOTDIR%\rtcs\source\include\nd6.h" "%OUTPUTDIR%\nd6.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ip6_prv.h" "%OUTPUTDIR%\ip6_prv.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ip.h" "%OUTPUTDIR%\ip.h" /Y
copy "%ROOTDIR%\rtcs\source\include\socket.h" "%OUTPUTDIR%\socket.h" /Y
copy "%ROOTDIR%\rtcs\source\include\icmp6_prv.h" "%OUTPUTDIR%\icmp6_prv.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ccp.h" "%OUTPUTDIR%\ccp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs.h" "%OUTPUTDIR%\rtcs.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ipc_udp.h" "%OUTPUTDIR%\ipc_udp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\natvers.h" "%OUTPUTDIR%\natvers.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_smtp.h" "%OUTPUTDIR%\rtcs_smtp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_if.h" "%OUTPUTDIR%\rtcs_if.h" /Y
copy "%ROOTDIR%\rtcs\source\include\tcp.h" "%OUTPUTDIR%\tcp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_stats.h" "%OUTPUTDIR%\rtcs_stats.h" /Y
copy "%ROOTDIR%\rtcs\source\include\udp.h" "%OUTPUTDIR%\udp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\tcpip.h" "%OUTPUTDIR%\tcpip.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ipcfg.h" "%OUTPUTDIR%\ipcfg.h" /Y
copy "%ROOTDIR%\rtcs\source\include\iptunnel.h" "%OUTPUTDIR%\iptunnel.h" /Y
copy "%ROOTDIR%\rtcs\source\include\lcp.h" "%OUTPUTDIR%\lcp.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcsrtos.h" "%OUTPUTDIR%\rtcsrtos.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_err.h" "%OUTPUTDIR%\rtcs_err.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ftpc.h" "%OUTPUTDIR%\ftpc.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ipradix.h" "%OUTPUTDIR%\ipradix.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcslog.h" "%OUTPUTDIR%\rtcslog.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcsvers.h" "%OUTPUTDIR%\rtcsvers.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs25x.h" "%OUTPUTDIR%\rtcs25x.h" /Y
copy "%ROOTDIR%\rtcs\source\include\ticker.h" "%OUTPUTDIR%\ticker.h" /Y
copy "%ROOTDIR%\rtcs\source\include\rtcs_in.h" "%OUTPUTDIR%\rtcs_in.h" /Y


goto tool_%TOOL%

rem iar files
:tool_iar
goto copy_end

rem cw10gcc files
:tool_cw10gcc
goto copy_end

rem uv4 files
:tool_uv4
goto copy_end

rem gcc_arm files
:tool_gcc_arm
goto copy_end

:copy_end

