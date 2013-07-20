/*
**  ************************************************************************
**
**  File    Name  : NetWork.c
**  Description   : 网络相关文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              头文件
**  ************************************************************************
*/
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>


#include "CommonInclude.h"
#include "Common.h"
#include "NetWork.h"
#include "Log.h"
#include "CommonDef.h"
#include "IniFile.h"

#define DEFAULT_SECTION               "DEFAULT"
#define DEFAULT_KEY                   "default"
#define NETWORK_BUF_SIZE              32
#define NETWORK_CONFIG_PATH           "/mnt/mtd/Config/network.conf"
#define IPADDR_KEY                    "ipaddr"
#define MACADDR_KEY                   "mac"
#define GATEWAY_KEY                   "gateway"
#define NETMASK_KEY                   "netmask"
#define DEFAULT_GATEWAY               "0.0.0.0"

#define NETDEV_PATH                      "/sys/class/net/"
#define PROC_NETDEV_PATH                "/proc/net/dev"


/* $Function        :   内部函数check_net_dev( char *p_dev)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名
==  Return          :   1： 存在  0： 不存在
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int check_net_dev(char *p_dev)
{
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(NETDEV_PATH);
    while((ptr = readdir(dir)) != NULL)
    {
        if((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)
                || (strcmp(ptr->d_name, "lo") == 0))       //排除lo网卡
        {
            continue;
        }
        if(strcmp(ptr->d_name, p_dev) == 0)
        {
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);
    return 0;
}

/* $Function        :   内部函数get_net_dev(char *p_dev, int *pDevSum)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev :dev buffer pDevSum 网卡数目
==  Return          :   1： 存在  0： 不存在
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int get_net_dev(NETWORK_INTERFACE_STRU *pstNetIf)
{
    DIR *dir;
    int dev_sum = 0;
    struct dirent *ptr;
    dir = opendir(NETDEV_PATH);
    if(NULL == dir)
    {
        LIBHAL_ERROR("Invalid parameter");
    }
    while((ptr = readdir(dir)) != NULL)
    {
        if((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)
                || (strcmp(ptr->d_name, "lo") == 0))       //排除lo网卡
        {
            continue;
        }
        sprintf(pstNetIf->NetWorkInterface[dev_sum], "%s", ptr->d_name);
        dev_sum ++;
        pstNetIf->NetWorkInterfaceSum = dev_sum;
    }
    closedir(dir);
    return 0;
}


/* $Function        :   内部函数static int get_local_ip( char *p_dev, char *p_ip)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:ip
==  Return          :   -2：参数无效，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int get_dev_ip(char *p_dev, char *p_ip)
{
    char dev_temp[128] = {0};
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!read_profile_string(dev_temp, IPADDR_KEY, p_ip, NETWORK_BUF_SIZE, "", NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("read ipaddr from config file failed");
        return -2;
    }
    return 0;
}
/*****************************************************************************
 函 数 名  : get_dev_mac_socket
 功能描述  : 通过socket 方式获取网卡mac地址
 输入参数  : char *p_dev
             char *p_mac
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int get_dev_mac_socket(char *p_dev, char *p_mac)
{
    int sock_mac;
    struct ifreq ifr_mac;
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_mac == -1)
    {
        LIBHAL_ERROR("create socket mac fail");
        return -1;
    }
    memset(&ifr_mac, 0, sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, p_dev, sizeof(ifr_mac.ifr_name) - 1);
    if((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        LIBHAL_ERROR("mac ioctl error");
        close(sock_mac);
        return -1;
    }
    sprintf(p_mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[5]);
    close(sock_mac);
    LIBHAL_DEBUG("get mac success local mac:%s begin save mac to config file", p_mac);
    char dev_temp[128] = {0};
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!write_profile_string(dev_temp, MACADDR_KEY, p_mac, NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("write mac to config file failed");
        return -2;
    }
    return 0;
}


/* $Function        :   内部函数static int get_local_gateway( char *p_dev, char *gateway)
==   ===============================================================
==  Description     :   设置网卡gateway
==  Argument        :   p_dev ：网卡名，p_gateway:gateway
==  Return          :   -2：参数无效，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int get_dev_gateway(char *p_dev, char *p_gateway)
{
    char dev_temp[128] = {0};
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!read_profile_string(dev_temp, GATEWAY_KEY, p_gateway, NETWORK_BUF_SIZE, "", NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("read gateway from config file failed");
        return -2;
    }
    return 0;
}


/* $Function        :   内部函数static int get_local_gateway( char *p_dev, char *gateway)
==   ===============================================================
==  Description     :   设置网卡gateway
==  Argument        :   p_dev ：网卡名，p_gateway:gateway
==  Return          :   -2：参数无效，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int get_dev_mac(char *p_dev, char *p_mac)
{
    char dev_temp[128] = {0};
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!read_profile_string(dev_temp, MACADDR_KEY, p_mac, NETWORK_BUF_SIZE, "", NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("read mac from config file failed");
        return -2;
    }
    return 0;
}


/* $Function        :   内部函数static int get_local_netmask char *p_dev, char *netmask)
==   ===============================================================
==  Description     :   设置网卡netmask
==  Argument        :   p_dev ：网卡名，p_netmask:netmask
==  Return          :   -2：参数无效，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int get_dev_netmask(char *p_dev, char *p_netmask)
{
    char dev_temp[128] = {0};
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!read_profile_string(dev_temp, NETMASK_KEY, p_netmask, NETWORK_BUF_SIZE, "", NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("read mac from config file failed");
        return -2;
    }
    return 0;
}

/* $Function        :   内部函数get_dev_flow( p_dev, p_network_flow)
==   ===============================================================
==  Description     :   设置网卡netmask
==  Argument        :   p_dev ：网卡名，p_netmask:netmask
==  Return          :   -2：参数无效，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int get_dev_flow(char *p_dev, NetWorkFlow_T *p_network_flow)
{
    FILE *fp;
    char buf[512];
    char *s;
    unsigned long long flow_tmp;
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    memset(stat, 0, sizeof(NetWorkFlow_T));
    fp = fopen(PROC_NETDEV_PATH, "r");
    if(!fp)
    {
        perror("fopen PROC_NETDEV_PATH");
        LIBHAL_ERROR("cannot open %s\n", PROC_NETDEV_PATH);
        return -1;
    }
    fgets(buf, sizeof(buf), fp);       /* eat line */
    fgets(buf, sizeof(buf), fp);
    while(fgets(buf, sizeof(buf), fp))
    {
        if(strstr(buf, p_dev))
        {
            s = strchr(buf, ':');
            s += 1;
            sscanf(s, "%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu",
                   &p_network_flow->Rx_bytes,
                   &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp,
                   &p_network_flow->Tx_bytes,
                   &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp, &flow_tmp
                  );
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    LIBHAL_ERROR("cannot find device %s\n", p_dev);
    return -1;
}

/* $Function        :   内部函数static int set_local_ip( char *p_dev, char *p_ip)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:ip
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int set_dev_ip(char *p_dev, char *p_ip)
{
    int sock_set_ip;
    struct sockaddr_in sin_set_ip;
    struct ifreq ifr_set_ip;
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    if((sock_set_ip = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        LIBHAL_ERROR("socket create fail");
        return -1;
    }
    bzero(&ifr_set_ip, sizeof(ifr_set_ip));
    memset(&sin_set_ip, 0, sizeof(sin_set_ip));
    strncpy(ifr_set_ip.ifr_name, p_dev, sizeof(ifr_set_ip.ifr_name) - 1);
    sin_set_ip.sin_family = AF_INET;
    sin_set_ip.sin_addr.s_addr = inet_addr(p_ip);
    memcpy(&ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));
    if(ioctl(sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0)
    {
        LIBHAL_ERROR("Not setup interface");
        return -1;
    }
    //设置激活标志
    ifr_set_ip.ifr_flags |= IFF_UP | IFF_RUNNING;
    //get the status of the device
    if(ioctl(sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip) < 0)
    {
        LIBHAL_ERROR("SIOCSIFFLAGS");
        close(sock_set_ip);
        return -1;
    }
    close(sock_set_ip);
    LIBHAL_DEBUG("set ip success,begin save ip to config file");
    char dev_temp[128] = {0};
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!write_profile_string(dev_temp, IPADDR_KEY, p_ip, NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("write ipaddr to config file failed");
        return -2;
    }
    return 0;
}

/* $Function        :   内部函数static int set_local_netmask( char *p_dev, char *p_mac)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:mac
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int set_dev_netmask(char *p_dev, char *p_netmask)
{
    int sock_netmask;
    struct ifreq ifr_mask;
    struct sockaddr_in *sin_net_mask;
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    sock_netmask = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_netmask == -1)
    {
        LIBHAL_ERROR("Not create network socket connect/n");
        return -1;
    }
    memset(&ifr_mask, 0, sizeof(ifr_mask));
    strncpy(ifr_mask.ifr_name, p_dev, sizeof(ifr_mask.ifr_name) - 1);
    sin_net_mask = (struct sockaddr_in *) &ifr_mask.ifr_addr;
    sin_net_mask -> sin_family = AF_INET;
    inet_pton(AF_INET, p_netmask, &sin_net_mask ->sin_addr);
    if(ioctl(sock_netmask, SIOCSIFNETMASK, &ifr_mask) < 0)
    {
        LIBHAL_ERROR("sock_netmask ioctl error/n");
        close(sock_netmask);
        return -1;
    }
    close(sock_netmask);
    LIBHAL_DEBUG("set netmask success,begin save netmask to config file");
    char dev_temp[32] = {0};
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!write_profile_string(dev_temp, NETMASK_KEY, p_netmask, NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("write netmask to config file failed\n");
        return -2;
    }
    return 0;
}


/* $Function        :   内部函数static int set_local_gateway( char *p_dev, char *p_gateway)
==   ===============================================================
==  Description     :   设置网卡gateway
==  Argument        :   p_dev ：网卡名，p_gateway:gateway
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int set_dev_gateway(char *p_dev, char *p_gateway)
{
    struct rtentry rt;
    struct sockaddr_in *s_in;
    int socket_gateway;
    char pDevNetMask[32] = {0};
    char pDevIP[32] = {0};
    NETWORK_INTERFACE_STRU NetIfDev;
    int i;
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    if(get_dev_ip(p_dev, pDevIP) < 0)
    {
        LIBHAL_ERROR("Get_dev_mac failed \n");
        return -1;
    }
    LIBHAL_DEBUG("pDevIP = %s\n", pDevIP);
    if(get_dev_netmask(p_dev, pDevNetMask) < 0)
    {
        LIBHAL_ERROR("Get_dev_mac failed \n");
        return -1;
    }
    LIBHAL_DEBUG("pDevIP = %s\n", pDevNetMask);
    memset(&NetIfDev, 0, sizeof(NETWORK_INTERFACE_STRU));
    // Create a socket to the INET kernel.
    if((socket_gateway = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        LIBHAL_ERROR("socket_gateway failed");
        return -1;
    }
    memset(&rt, 0, sizeof(struct rtentry));
    /* Default is special, meaning 0.0.0.0. */
    rt.rt_flags = RTF_UP | RTF_MODIFIED;
    rt.rt_dev = (char *) p_dev;
    rt.rt_metric = 0;
    get_net_dev(&NetIfDev);
    s_in = (struct sockaddr_in *) & (rt.rt_dst);
    s_in->sin_family = AF_INET;
    s_in->sin_port = 0;
    s_in->sin_addr.s_addr = INADDR_ANY;
    rt.rt_flags = RTF_UP | RTF_MODIFIED;
    rt.rt_flags |= RTF_GATEWAY;
    /* Delete the old default gateway and startup the new one.  */
    for(i = 0; i < NetIfDev.NetWorkInterfaceSum; i++)
    {
        rt.rt_dev = NetIfDev.NetWorkInterface[i];
        ioctl(socket_gateway, SIOCDELRT, &rt);
    }
    if(NULL != strstr(p_gateway, DEFAULT_GATEWAY))
    {
        memset(&rt, 0, sizeof(struct rtentry));
        /* Default is special, meaning 0.0.0.0. */
        rt.rt_flags = RTF_UP | RTF_MODIFIED;
        rt.rt_dev = p_dev;
        s_in = (struct sockaddr_in *) & (rt.rt_genmask);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = inet_addr(pDevNetMask);
        s_in = (struct sockaddr_in *) & (rt.rt_dst);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = inet_addr(pDevIP)&inet_addr(pDevNetMask);
        ioctl(socket_gateway, SIOCDELRT, &rt);
    }
    if(strcmp(p_gateway, DEFAULT_GATEWAY))
    {
        memset(&rt, 0, sizeof(struct rtentry));
        rt.rt_flags = RTF_UP | RTF_MODIFIED;
        rt.rt_dev = p_dev;
        rt.rt_metric = 0;
        /* gateway */
        s_in = (struct sockaddr_in *) & (rt.rt_gateway);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = inet_addr(p_gateway);
        /*net mask*/
        s_in = (struct sockaddr_in *) & (rt.rt_genmask);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = INADDR_ANY;
        /* dst */
        s_in = (struct sockaddr_in *) & (rt.rt_dst);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = INADDR_ANY;
        rt.rt_flags |= RTF_GATEWAY;
        /* Tell the kernel to accept this route. */
        if(ioctl(socket_gateway, SIOCADDRT, &rt) < 0)
        {
            LIBHAL_ERROR("NetWorkSetGateway SIOCADDRT failed\n"
                         "%s", strerror(errno));
            close(socket_gateway);
            return -1;
        }
    }
    else     /* 处理传下来为0.0.0.0的路由 */
    {
        memset(&rt, 0, sizeof(struct rtentry));
        /* Default is special, meaning 0.0.0.0. */
        rt.rt_flags = RTF_UP | RTF_MODIFIED;
        rt.rt_dev = p_dev;
        s_in = (struct sockaddr_in *) & (rt.rt_genmask);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = inet_addr(DEFAULT_GATEWAY);
        s_in = (struct sockaddr_in *) & (rt.rt_dst);
        s_in->sin_family = AF_INET;
        s_in->sin_port = 0;
        s_in->sin_addr.s_addr = INADDR_ANY;
        rt.rt_metric = 0;
        /* Tell the kernel to accept this route. */
        if(ioctl(socket_gateway, SIOCADDRT, &rt) < 0)
        {
            LIBHAL_ERROR("NetWorkSetGateway SIOCADDRT failed\n"
                         "%s", strerror(errno));
            close(socket_gateway);
            return -1;
        }
    }
    close(socket_gateway);
    LIBHAL_DEBUG("set gateway success,begin save gateway to config file");
    char dev_temp[128] = {0};
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!write_profile_string(DEFAULT_SECTION, DEFAULT_KEY, p_dev, NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("write default  Netdev to config file failed");
        return -2;
    }
    if(!write_profile_string(dev_temp, GATEWAY_KEY, p_gateway, NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("write ipaddr to config file failed");
        return -2;
    }
    return 0;
}

/* $Function        :   内部函数static int set_local_netmac( char *p_dev, char *p_mac)
==   ===============================================================
==  Description     :   设置网卡mac
==  Argument        :   p_dev ：网卡名，p_mac:mac
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
static int set_dev_mac(char *p_dev, char *p_mac)
{
    char command[32] = { 0  };
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    sprintf(command, "ifconfig %s down", p_dev);
    if(RunSystemCommand(command) < 0)
    {
        LIBHAL_ERROR("%s faield\n", command);
        return -1;
    }
    else
    {
        LIBHAL_DEBUG("%s Successful", command);
    }
    sprintf(command, "ifconfig %s hw ether %s", p_dev, p_mac);
    if(RunSystemCommand(command) < 0)
    {
        sprintf(command, "ifconfig %s up", p_dev);
        RunSystemCommand(command);
        LIBHAL_ERROR("%s faield", command);
        return -1;
    }
    else
    {
        LIBHAL_DEBUG("%s Successful", command);
    }
    sprintf(command, "ifconfig %s up", p_dev);
    if(RunSystemCommand(command) < 0)
    {
        LIBHAL_ERROR("%s faield", command);
        return -1;
    }
    else
    {
        LIBHAL_DEBUG("%s Successful", command);
    }
    LIBHAL_DEBUG("set mac success,begin save mac to config file");
    char dev_temp[128] = {0};
    strncpy(dev_temp, p_dev, sizeof(dev_temp) - 1);
    UPSTRING(dev_temp);
    if(!write_profile_string(dev_temp, MACADDR_KEY, p_mac, NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("write ipaddr to config file failed");
        return -2;
    }
    return 0;
}


/*****************************************************************************
 函 数 名  : NetWorkGetInterface
 功能描述  : 获取网络设备名称
 输入参数  : char(*p_dev)[32] :网络设备名称
             int *pDevSum  :网络设备个数
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月30日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int NetWorkGetInterface(NETWORK_INTERFACE_STRU *pstNetIf)
{
    if(NULL == pstNetIf)
    {
        LIBHAL_ERROR("the param error");
    }
    return get_net_dev(pstNetIf);
}


/*****************************************************************************
 函 数 名  : NetWorkGetConfigInfo
 功能描述  : 获取指定网络接口的Ip，NetMask，GateWay
 输入参数  : char *pdev
             NETWORK_ATTRIBUTE_STRU *pstNetAttr
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月30日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int NetWorkGetConfigInfo(char *pdev, NETWORK_ATTRIBUTE_STRU *pstNetAttr)
{
    if(NULL == pdev || NULL == pstNetAttr)
    {
        LIBHAL_ERROR("param error");
    }
    if(get_dev_ip(pdev, pstNetAttr->net_attr_ip) < 0)
    {
        LIBHAL_ERROR("Get_dev_mac failed \n");
        return -1;
    }
    if(get_dev_netmask(pdev, pstNetAttr->net_attr_netmask) < 0)
    {
        LIBHAL_ERROR("Get_dev_mac failed \n");
        return -1;
    }
    if(get_dev_gateway(pdev, pstNetAttr->net_attr_getway) < 0)
    {
        LIBHAL_ERROR("Get_dev_mac failed \n");
        return -1;
    }
    return 0;
}


int NetworCheckDev(char *p_dev)
{
    if(NULL == p_dev)
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    return check_net_dev(p_dev);
}
/* $Function        :   NetWorkSetIp(char *p_dev, char *p_ip)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:ip
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkSetIp(char *p_dev, char *p_ip)
{
    if((NULL == p_dev) || (NULL == p_ip) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    return  set_dev_ip(p_dev, p_ip);
}


/* $Function        :   int NetWorkSetNetMask(char *p_dev, char *p_netmask)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_netmask:netmask
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkSetNetMask(char *p_dev, char *p_netmask)
{
    if((NULL == p_dev) || (NULL == p_netmask) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -3;
    }
    return  set_dev_netmask(p_dev, p_netmask);
}

/* $Function        :   int  NetWorkSetDefaultGateWay(char *p_dev, char *p_gateway)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_gateway:gateway
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkSetDefaultGateWay(char *p_dev, char *p_gateway)
{
    if((NULL == p_dev) || (NULL == p_gateway) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -3;
    }
    return  set_dev_gateway(p_dev, p_gateway);
}


/*****************************************************************************
 函 数 名  : NetWorkSetDefaultGateWay
 功能描述  : 设置网卡默认网关
 输入参数  : char *p_dev
             char *p_gateway
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int NetWorkSetDefault(char *p_dev)
{
    if((NULL == p_dev) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -3;
    }
    if(0 == set_dev_gateway(p_dev, DEFAULT_GATEWAY))
    {
        if(!write_profile_string(DEFAULT_SECTION, DEFAULT_KEY, p_dev, NETWORK_CONFIG_PATH))
        {
            LIBHAL_ERROR("write default  Netdev to config file failed");
            return -2;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : NetWorkGetDefaultDev
 功能描述  : 获取默认网络设备
 输入参数  : char *p_dev
             char *p_gateway
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int NetWorkGetDefaultDev(char *pDefaultDev)
{
    if(NULL == pDefaultDev)
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -3;
    }
    if(!read_profile_string(DEFAULT_SECTION, DEFAULT_KEY, pDefaultDev
                            , NETWORK_BUF_SIZE, "", NETWORK_CONFIG_PATH))
    {
        LIBHAL_ERROR("read default dev from config file failed");
        return -2;
    }
    return 0;
}


/* $Function        :   int NetWorkSetMac(char *p_dev, char *p_mac)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_mac:mac
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkSetMac(char *p_dev, char *p_mac)
{
    if((NULL == p_dev) || (NULL == p_mac) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -3;
    }
    return  set_dev_mac(p_dev, p_mac);
}

/* $Function        :   NetWorkGetIp(char *p_dev, char *p_ip)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:ip
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkGetIp(char *p_dev, char *p_ip)
{
    if((NULL == p_dev) || (NULL == p_ip) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    return  get_dev_ip(p_dev, p_ip);
}

/* $Function        :   NetWorkGetGateWay(char *p_dev, char *p_gateway)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_p_gateway:p_gateway
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkGetGateWay(char *p_dev, char *p_gateway)
{
    if((NULL == p_dev) || (NULL == p_gateway) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    return  get_dev_gateway(p_dev, p_gateway);
}
/* $Function        :   int NetWorkGetMac(char *p_dev, char *p_mac)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:ip
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkGetMac(char *p_dev, char *p_mac)
{
    if((NULL == p_dev) || (NULL == p_mac) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter");
        return -1;
    }
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    return  get_dev_mac(p_dev, p_mac);
}

/* $Function        :   int NetWorkGetMac(char *p_dev, char *p_mac)
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，p_ip:ip
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkGetNetMask(char *p_dev, char *p_netmask)
{
    if((NULL == p_dev) || (NULL == p_netmask) || (strcmp(p_dev, "lo") == 0))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    return  get_dev_netmask(p_dev, p_netmask);
}


/* $Function        :   int NetWorkGetTxRx(char *p_dev, NetWorkFlow_T *network_flow  )
==   ===============================================================
==  Description     :   设置网卡ip
==  Argument        :   p_dev ：网卡名，network_flow:ip
==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int NetWorkGetFlow(char *p_dev, NetWorkFlow_T *p_network_flow)
{
    if((NULL == p_dev) || (NULL == p_network_flow))
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    if(check_net_dev(p_dev) == 0)
    {
        LIBHAL_WARN("the device not exist");
        return -1;
    }
    return  get_dev_flow(p_dev, p_network_flow);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define DNS_RESOLV_FILE_PATH "/etc/resolv.conf" 
#define DNS_TIMEOUT_MAX 5
#define DNS_HOST  0x01
#define DNS_CNAME 0x05
#define DNS_SERVER_PORT 53 


static int is_pointer(int in)
{
	return ((in & 0xc0) == 0xc0);
}

S32 GetDnsServerIP(char (*pDnsIP)[32])
{
    FILE *fp = NULL;
    char *pSpace = NULL;
    S8 as8DnsBuf[256] = {0};
    S32 s32DnsServerIPCnt = 0;
    
    if (NULL == (fp = fopen(DNS_RESOLV_FILE_PATH,"r")))
    {
        printf("open %s failed\n",DNS_RESOLV_FILE_PATH);
        return -1;
    }
    
    while (NULL != fgets(as8DnsBuf,sizeof(as8DnsBuf),fp))
    {
    
        pSpace = as8DnsBuf;
        if (*pSpace == '\n')
        {
            continue;
        }
        
        while(1)
        {
           if (isdigit(*pSpace) || ispunct(*pSpace))
           {
             sprintf(*pDnsIP,"%s",pSpace);
             /* 去掉行未的回车符 */
             *(*pDnsIP+(strlen(*pDnsIP)-1)) = '\0';
             s32DnsServerIPCnt++;
             pDnsIP++;
             break; 
           }
           pSpace++;
        }
    }
    return  s32DnsServerIPCnt;
}

void ParaseDnsName(unsigned char *chunk, unsigned char *ptr , char *out , int *len)
{
	int n ,  flag;
	char *pos = out + (*len);
 
	for(;;)
	{
		flag = (int)ptr[0];
		if(flag == 0)
			break;
		if(is_pointer(flag))
		{
			n = (int)ptr[1];
			ptr = chunk + n;
			ParaseDnsName(chunk , ptr , out , len);
			break;
		}
		else
		{
			ptr ++;
			memcpy(pos , ptr , flag);	
			pos += flag;
			ptr += flag;
			*len += flag;
			if((int)ptr[0] != 0)
			{
				memcpy(pos , "." , 1);
				pos += 1;
				(*len) += 1;
			}
		}
	}
 
}


S32 ParseDnsResponse(S32 s32SocketFd,S8 *ps8UrlIP)
{
 
	unsigned char buf[1024];
	unsigned char *ptr = buf;
	struct sockaddr_in addr;
	int n , i , flag , querys , answers;
	int type , ttl , datalen , len;
	char cname[128] , aname[128] , ip[20] ;
	unsigned char netip[4];
	size_t addr_len = sizeof(struct sockaddr_in);

	/* set recvfrom from server timeout */
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = DNS_TIMEOUT_MAX;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(s32SocketFd, &readfds);
    if (select(s32SocketFd+1,&readfds,NULL, NULL, &tv) > 0)
    {
        n = recvfrom(s32SocketFd , buf , sizeof(buf) , 0, (struct sockaddr*)&addr , &addr_len);
        if ( n < 0)
        {
            perror("dns recv");
            return -1;
        }

    }
    else
    {
        LIBHAL_WARN("Timeout!there is no data arrived!\n");
        return -1;
    }
 

	ptr += 4; /* move ptr to Questions */
	querys = ntohs(*((unsigned short*)ptr));
	ptr += 2; /* move ptr to Answer RRs */
	answers = ntohs(*((unsigned short*)ptr));
	ptr += 6; /* move ptr to Querys */
	/* move over Querys */
	for(i= 0 ; i < querys ; i ++)
	{
		for(;;)
		{
			flag = (int)ptr[0];
			ptr += (flag + 1);
			if(flag == 0)
			{
				break;
			}
		}
		ptr += 4;
	}
	/* now ptr points to Answers */
	for(i = 0 ; i < answers ; i ++)
	{
		bzero(aname , sizeof(aname));
		len = 0;
		ParaseDnsName(buf , ptr , aname , &len);
		ptr += 2; /* move ptr to Type*/
		type = htons(*((unsigned short*)ptr));
		ptr += 4; /* move ptr to Time to live */
		ttl = htonl(*((unsigned int*)ptr));
		ptr += 4; /* move ptr to Data lenth */
		datalen = ntohs(*((unsigned short*)ptr));
		ptr += 2; /* move ptr to Data*/
		if(type == DNS_CNAME)
		{
			bzero(cname , sizeof(cname));
			len = 0;
			ParaseDnsName(buf , ptr , cname , &len);
#ifdef DEEBUG
            printf("-------------------------------\n");
			printf("%s is an alias for %s\n" , aname , cname);
#endif
			ptr += datalen;
		}
		
		if(type == DNS_HOST)
		{
			bzero(ip , sizeof(ip));
			if(datalen == 4)
			{
				memcpy(netip , ptr , datalen);
				inet_ntop(AF_INET , netip , ip , sizeof(struct sockaddr));
				sprintf(ps8UrlIP,"%s",ip);
				
#ifdef DEEBUG		
                printf("%s has address %s\n" , aname , ip);
				printf("\tTime to live: %d minutes , %d seconds\n", ttl / 60 , ttl % 60);
#endif			
			}
			ptr += datalen;
		}
 
	}
	ptr += 2;
    return 0;
}
 


static void GenerateDnsRequest(const char *dns_name , unsigned char *buf , int *len)
{
	char *pos;
	unsigned char *ptr;
	int n;
 
	*len = 0;
	ptr = buf;	
	pos = (char*)dns_name; 
	for(;;)
	{
		n = strlen(pos) - (strstr(pos , ".") ? strlen(strstr(pos , ".")) : 0);
		*ptr ++ = (unsigned char)n;
		memcpy(ptr , pos , n);
		*len += n + 1;
		ptr += n;
		if(!strstr(pos , "."))
		{
			*ptr = (unsigned char)0;
			ptr ++;
			*len += 1;
			break;
		}
		pos += n + 1;
	}
	*((unsigned short*)ptr) = htons(1);
	*len += 2;
	ptr += 2;
	*((unsigned short*)ptr) = htons(1);
	*len += 2;
}
 
int SendDnsRequest(const char *dns_name,S32 s32SocketFd,struct sockaddr_in stDest)
{
 
	unsigned char request[256];
	unsigned char *ptr = request;
	unsigned char question[128];
	int question_len;
 
 
	GenerateDnsRequest(dns_name , question , &question_len);
 
	*((unsigned short*)ptr) = htons(0xff00);
	ptr += 2;
	*((unsigned short*)ptr) = htons(0x0100);
	ptr += 2;
	*((unsigned short*)ptr) = htons(1);
	ptr += 2;
	*((unsigned short*)ptr) = 0;
	ptr += 2;
	*((unsigned short*)ptr) = 0;
	ptr += 2;
	*((unsigned short*)ptr) = 0;
	ptr += 2;
	memcpy(ptr , question , question_len);
	ptr += question_len;
 
	if ( sendto(s32SocketFd , request , question_len + 12 , 0
	            , (struct sockaddr*)&stDest , sizeof(struct sockaddr)) < 0)
	{
	    LIBHAL_ERROR("send data error\n");
	    perror("dns");
	    return -1;
	}
	return 0;
}



/*****************************************************************************
 函 数 名  : NetWorkGetIpFromUrl
 功能描述  : 从收入的url中获取真实的ip
 输入参数  : S8 *ps8Url
             S8 *ps8UrlIP
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
S32 NetWorkGetIpFromUrl(S8 *ps8DomainName, S8 *ps8UrlIP)
{
    S32 s32SocketFd = 0;
    struct sockaddr_in stDest;
    S32 s32Ret = -1;
    S8 au8DnsIPAary[10][32];
    S32 s32DnsIPSum = 0;
    S32 s32Index = 0;
    
    if(NULL == ps8DomainName || NULL == ps8UrlIP) 
    {
       LIBHAL_ERROR("GetUrlIPFromDns param error!");
    }

    
    s32DnsIPSum = GetDnsServerIP(au8DnsIPAary);

    LIBHAL_INFO("Get DNS server ip from %s is %d\n",DNS_RESOLV_FILE_PATH, s32DnsIPSum);

    s32SocketFd = socket(AF_INET , SOCK_DGRAM , 0);
    if(s32SocketFd < 0)
    {
        perror("create socket failed");
        return -1;
    }
    /* 采用非阻塞模式 */
    if ( fcntl(s32SocketFd, F_SETFL, O_NONBLOCK) < 0)
    {
        printf("fcntl socket error!\n");
        return -1;
    }

    for (s32Index = 0; s32Index < s32DnsIPSum; s32Index++)
    {
        LIBHAL_INFO("%s\n",au8DnsIPAary[s32Index]);      
        bzero(&stDest , sizeof(stDest));
        
        stDest.sin_family = AF_INET;
        stDest.sin_port = htons(DNS_SERVER_PORT);
        stDest.sin_addr.s_addr = inet_addr(au8DnsIPAary[s32Index]);
     
        s32Ret = SendDnsRequest(ps8DomainName,s32SocketFd,stDest);
        s32Ret = ParseDnsResponse(s32SocketFd, ps8UrlIP);
        if (s32Ret < 0)
        {
            if (s32Index+1 == s32DnsIPSum)
            {
                goto fail;
            }
            else
            {
                continue;
            }
        }
        else
        {
            break;
        }
    }
    
    close(s32SocketFd);
 
    return 0;
fail:
    close(s32SocketFd);
    return -1;
}

/*****************************************************************************
 函 数 名  : NetWorkCheckLinkState
 功能描述  : 检查网线连接状态
 输入参数  : char *if_name
 输出参数  : 无
 返 回 值  : 1:success 0:down -1: failed
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月16日
    作    者   : 检查网线连接状态
    修改内容   : 新生成函数

*****************************************************************************/
S32 NetWorkCheckLinkState(char *if_name)
{
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;
    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *)&edata;
    if(NULL == if_name)
    {
        LIBHAL_ERROR("Invalid parameter\n");
        return -1;
    }
    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        LIBHAL_ERROR("creat socket failed");
        return -1;
    }
    if(ioctl(skfd, SIOCETHTOOL, &ifr) == -1)
    {
        close(skfd);
        LIBHAL_ERROR("ioctl failed");
        return -1;
    }
    close(skfd);
    return edata.data;
}

