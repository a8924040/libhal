/**
* @file
* @brief test ini file api
* @author Deng Yangjun
* @date 2007-1-9
* @version 0.2
*/
#include "IniFile.h"
#include "NetWork.h"

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>


int wlan0_check(void)
{
    FILE *fp = NULL;
    char line_buf[256] = { 0   };
    if((fp = fopen("/proc/net/dev", "r")) == NULL)
    {
        LIBHAL_ERROR("open /proc/net/dev failed");
        return -1;
    }
    while(fgets(line_buf, sizeof(line_buf), fp) != NULL)
    {
        if(strstr(line_buf, "wlan0") != NULL)
        {
            fclose(fp);
            return 1;
        }
        memset(line_buf, 0, sizeof(line_buf));
    }
    fclose(fp);
    return 0;
}


int main(int argc, char *argv[])
{
    char DefaultNetDev[32] = {0};
    char DefaultDevIp[32] = {0};
    char DefaultDevMask[32] = {0};
    char DefaultDevGateway[32] = {0};
    LIBHAL_PRINTF("netinit Complie Date:%s Time:%s\n", __DATE__, __TIME__);
    if(access("/mnt/mtd/Config/network.conf", F_OK) != 0)
    {
        if(mkdir("/mnt/mtd/Config", 0777) < 0)
        {
            LIBHAL_ERROR("creat /mnt/mtd/Config failed");
        }
    }
    printf("mkdir /var/tmp\n");
    if(argc != 5)
    {
        if(NetWorkGetDefaultDev(DefaultNetDev) == 0
                && NetWorkGetIp(DefaultNetDev, DefaultDevIp) == 0
                && NetWorkGetNetMask(DefaultNetDev, DefaultDevMask) == 0
                && NetWorkGetGateWay(DefaultNetDev, DefaultDevGateway) == 0
          )
        {
            LIBHAL_ALARM("now set network %s attribute:\n"
                         "ip: %s\n"
                         "netmask: %s\n"
                         "gateway: %s\n", DefaultNetDev, DefaultDevIp
                         , DefaultDevMask, DefaultDevGateway);
            NetWorkSetIp(DefaultNetDev, DefaultDevIp);
            NetWorkSetNetMask(DefaultNetDev, DefaultDevMask);
            NetWorkSetDefaultGateWay(DefaultNetDev, DefaultDevGateway);
        }
        else
        {
            LIBHAL_ALARM("netinit pramer must include netdev,ip,netmask,gateway");
            LIBHAL_ALARM("example netinit eth0 192.168.2.20"
                         " 255.255.255.0 192.168.2.1");
            LIBHAL_ALARM("now set network attribute to defualt value\n"
                         "ip: 192.168.2.21 \n"
                         "netmask: 255.255.255.0\n"
                         "gateway: 192.168.2.1\n");
            NetWorkSetIp("eth0", "192.168.2.21");
            NetWorkSetNetMask("eth0", "255.255.255.0");
            NetWorkSetDefaultGateWay("eth0", "192.168.2.1");
        }
        return 0;
    }
    strncpy(DefaultNetDev, argv[1], sizeof(DefaultNetDev) - 1);
    strncpy(DefaultDevIp, argv[2], sizeof(DefaultDevIp) - 1);
    strncpy(DefaultDevMask, argv[3], sizeof(DefaultDevMask) - 1);
    strncpy(DefaultDevGateway, argv[4], sizeof(DefaultDevGateway) - 1);
    LIBHAL_DEBUG("begin to set network attribute");
    NetWorkSetIp(DefaultNetDev, DefaultDevIp);
    NetWorkSetNetMask(DefaultNetDev, DefaultDevMask);
    NetWorkSetDefaultGateWay(DefaultNetDev, DefaultDevGateway);
    NetWorkGetNetMask(DefaultNetDev, DefaultDevMask);
    LIBHAL_DEBUG("get netmask from config file is %s", DefaultDevMask);
    NetWorkGetGateWay(DefaultNetDev, DefaultDevGateway);
    LIBHAL_DEBUG("get gateway from config file is %s", DefaultDevGateway);
    NetWorkGetIp(DefaultNetDev, DefaultDevIp);
    LIBHAL_DEBUG("get ip from config file is %s", DefaultDevIp);
    return 0;
}
