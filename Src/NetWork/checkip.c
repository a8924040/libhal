/******************************************************************************

              Copyright (C), 2007-2008, hangzhou, zju, 306Room.

 ******************************************************************************
  File Name     : checkIP.c
  Version       : Initial Draft
  Author        : qixiangbing
  Created       : 2008/7/2
  Last Modified :
  Description   : check the ip whether conflict
  Function List :
              arpping
              check_ip
              main
              read_interface
  History       :
  1.Date        : 2008/7/2
    Author      : qixiangbing
    Modification: Created file

******************************************************************************/
#include "CommonDef.h"
#include "CommonInclude.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include "checkip.h"

/* miscellaneous defines */
#define MAC_BCAST_ADDR      (unsigned char *) "\xff\xff\xff\xff\xff\xff"
#define ETH_INTERFACE       "eth0"
/* globals */
struct server_config_t server_config;

int read_interface(char *interface, int *ifindex, u_int32_t *addr, unsigned char *arp)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *our_ip;
    memset(&ifr, 0, sizeof(struct ifreq));
    if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0)
    {
        ifr.ifr_addr.sa_family = AF_INET;
        strcpy(ifr.ifr_name, interface);
        /*this is not execute*/
        if(addr)
        {
            if(ioctl(fd, SIOCGIFADDR, &ifr) == 0)
            {
                our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
                *addr = our_ip->sin_addr.s_addr;
                LIBHAL_DEBUG("%s (our ip) = %s", ifr.ifr_name, inet_ntoa(our_ip->sin_addr));
            }
            else
            {
                LIBHAL_ERROR("SIOCGIFADDR failed, is the interface up and"
                             "configured?: %s", strerror(errno));
                return -1;
            }
        }
        if(ioctl(fd, SIOCGIFINDEX, &ifr) == 0)
        {
            LIBHAL_DEBUG("adapter index %d", ifr.ifr_ifindex);
            *ifindex = ifr.ifr_ifindex;
        }
        else
        {
            LIBHAL_ERROR("SIOCGIFINDEX failed!: %s", strerror(errno));
            return -1;
        }
        if(ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
        {
            memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);
            LIBHAL_DEBUG("adapter hardware address %02x:%02x:%02x:%02x:%02x:%02x",
                         arp[0], arp[1], arp[2], arp[3], arp[4], arp[5]);
        }
        else
        {
            LIBHAL_ERROR("SIOCGIFHWADDR failed!: %s", strerror(errno));
            return -1;
        }
    }
    else
    {
        LIBHAL_ERROR("socket failed!: %s", strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}

/* args:    yiaddr - what IP to ping
 *      ip - our ip
 *      mac - our arp address
 *      interface - interface to use
 * retn:    1 addr free
 *      0 addr used
 *      -1 error
 */

/* FIXME: match response against chaddr */
int arpping(u_int32_t yiaddr, u_int32_t ip, unsigned char *mac, char *interface)
{
    int timeout = 2;
    int     optval = 1;
    int s;          /* socket */
    int rv = 1;         /* return value */
    struct sockaddr addr;       /* for interface name */
    struct arpMsg   arp;
    fd_set      fdset;
    struct timeval  tm;
    time_t      prevTime;
    if((s = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1)
    {
        LIBHAL_ERROR("Could not open raw socket");
        return -1;
    }
    if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1)
    {
        LIBHAL_ERROR("Could not setsocketopt on raw socket");
        close(s);
        return -1;
    }
    /* send arp request */
    memset(&arp, 0, sizeof(arp));
    memcpy(arp.ethhdr.h_dest, MAC_BCAST_ADDR, 6);    /* MAC DA */
    memcpy(arp.ethhdr.h_source, mac, 6);        /* MAC SA */
    arp.ethhdr.h_proto = htons(ETH_P_ARP);      /* protocol type (Ethernet) */
    arp.htype = htons(ARPHRD_ETHER);        /* hardware type */
    arp.ptype = htons(ETH_P_IP);            /* protocol type (ARP message) */
    arp.hlen = 6;                   /* hardware address length */
    arp.plen = 4;                   /* protocol address length */
    arp.operation = htons(ARPOP_REQUEST);       /* ARP op code */
    memcpy(arp.tInaddr, &yiaddr, sizeof(yiaddr));       /* target IP address */
    memcpy(arp.sInaddr, &ip, sizeof(ip));       /* source IP address */
    * ((u_int *) arp.sInaddr) = ip;         /* source IP address */
    memcpy(arp.sHaddr, mac, 6);         /* source hardware address */
    strcpy(addr.sa_data, interface);
    if(sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
    { rv = 0; }
    /* wait arp reply, and check it */
    tm.tv_usec = 0;
    time(&prevTime);
    while(timeout > 0)
    {
        FD_ZERO(&fdset);
        FD_SET(s, &fdset);
        tm.tv_sec = timeout;
        if(select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0)
        {
            LIBHAL_DEBUG("Error on ARPING request: %s", strerror(errno));
            if(errno != EINTR) { rv = 0; }
        }
        else if(FD_ISSET(s, &fdset))
        {
            if(recv(s, &arp, sizeof(arp), 0) < 0) { rv = 0; }
            if(arp.operation == htons(ARPOP_REPLY) &&
                    bcmp(arp.tHaddr, mac, 6) == 0 &&
                    * ((u_int *) arp.sInaddr) == yiaddr)
            {
                LIBHAL_DEBUG("Valid arp reply receved for this address");
                rv = 0;
                break;
            }
        }
        timeout -= time(NULL) - prevTime;
        time(&prevTime);
    }
    close(s);
    LIBHAL_DEBUG("%salid arp replies for this address", rv ? "No v" : "V");
    return rv;
}


/* check is an IP is taken, if it is, add it to the lease table */
int check_ip(u_int32_t addr)
{
    struct in_addr temp;
    if(arpping(addr, server_config.server, server_config.arp, ETH_INTERFACE) == 0)
    {
        temp.s_addr = addr;
        LIBHAL_DEBUG("%s belongs to someone, reserving it for %ld seconds",
                     inet_ntoa(temp), server_config.conflict_time);
        return 1;
    }
    else { return 0; }
}

/*****************************************************************************
 函 数 名  : CheckIpConflic
 功能描述  : 检测ip是否可用，是否冲突
 输入参数  : char *interface
             char *ipaddr
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int CheckIpConflic(char *interface,  char *ipaddr)
{
    if(read_interface(interface, &server_config.ifindex,
                      &server_config.server, server_config.arp) < 0)
    {
        exit(0);
    }
    if(!check_ip(inet_addr(ipaddr)))
    {
        LIBHAL_DEBUG("IP:%s can use", ipaddr);
        return 0;
    }
    else
    {
        LIBHAL_DEBUG("IP:%s conflict", ipaddr);
        return -1;
    }
    return 0;
}

