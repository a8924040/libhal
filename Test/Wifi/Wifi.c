#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <Wifi.h>
#include <System.h>
#include "NetWork.h"
int  main(int argc, char **argv)
{
    printf("wifi test compile at %s %s\n", __DATE__, __TIME__);
    SystemInitPlatform();
    WLAN_AP_STRU WlanAP;
    WLAN_CONN_INFO_STRU WlanConnInfo;
    memset(&WlanAP, 0, sizeof(WlanAP));
    if(WlanCreat() < 0)
    {
        LIBHAL_ERROR("Wlan creat failed!");
        return -1;
    }
    uint_sleep(3);
    int i, j ;
    for(i = 0; i < 5; i++)
    {
        WlanScanAP(&WlanAP);
        if(WlanAP.APSum > 0)
        {
            break;
        }
        sleep(1);
    }
    memset(&WlanAP, 0, sizeof(WlanAP));
    WlanScanAP(&WlanAP);
    printf("get WlanAP.APSum = %d\n", WlanAP.APSum);
    for(j = 0; j < WlanAP.APSum; j++)
    {
        printf("ApName:%s,Freq:%d,Signal:%d,APMac:%s,Encry:%d,WPS_support:%s\n"
               , WlanAP.astAPBox[j].APSSID, WlanAP.astAPBox[j].APFreq
               , WlanAP.astAPBox[j].APSignal, WlanAP.astAPBox[j].APMAC
               , WlanAP.astAPBox[j].APEncrypt
               , WlanAP.astAPBox[j].APWPS ? "NO" : "YES");
    }
    sprintf(WlanConnInfo.APSSID, "%s", "ztg");
    sprintf(WlanConnInfo.APPassWord, "%s", "201053402");
    WlanConnInfo.APEncrypt = AP_ENCRYPT_WPA2;
    WlanConnectAP(WlanConnInfo);
    uint_sleep(15);
    if(COMPLETED ==  WlanGetLinkState())
    {
        printf("OK connect to ap\n");
        NetWorkSetIp("wlan0", "192.168.2.134");
        NetWorkSetNetMask("wlan0", "255.255.255.0");
        NetWorkSetDefaultGateWay("wlan0", "192.168.2.1");
        //NetWorkSetDefault("wlan0");
    }
#if 0
    sleep(20);
    WlanReleaseAP();
    uint_sleep(5);
    WlanConnectAP(WlanConnInfo);
    uint_sleep(5);
    if(COMPLETED ==  WlanGetLinkState())
    {
        printf("OK connect to ap\n");
        NetWorkSetIp("wlan0", "192.168.1.134");
        NetWorkSetNetMask("wlan0", "255.255.255.0");
        NetWorkSetGateWay("wlan0", "192.168.1.1");
        NetWorkSetDefault("wlan0");
    }
    uint_sleep(10);
    WlanDestory();
#endif
    return 0;
}

