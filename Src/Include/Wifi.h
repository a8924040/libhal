/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : dvr_wlan.c
**  Description   : DVR WIFI接口
**  Modification  : 2011/10/14    huang_zhendi    创建
**  ************************************************************************
*/
#ifndef __WLAN_H__
#define __WLAN_H__

#ifdef __cplusplus
extern "C" {
#endif

    /*
    **  ************************************************************************
    **                             宏定义
    **  ************************************************************************
    */
#define MAC_LEN 32                  //MAC地址最大长度
#define APSSID_MAX_SIZE 64
#define APKEY_MAX_SIZE  64
#define AP_BOX_MAX   64   /* 最多支持64个AP信息 */

    typedef enum WPS_SUPPORT
    {
        WPS_INVALID,
        WPS_VALID
    }
    WPS_SUPPORT_ENUM;

    typedef enum AP_ENCRYPT
    {
        AP_ENCRYPT_OPEN,
        AP_ENCRYPT_WEP,
        AP_ENCRYPT_WPA,
        AP_ENCRYPT_WPA2,

    } AP_ENCRYPT_ENUM;

    typedef enum WLAN_CONNECT_STATE
    {
        INACTIVE,
        DISCONNECTED,
        COMPLETED,

    } WLAN_CONNECT_STATE_ENUM;

    typedef struct WLAN_AP_INFO
    {
        char APMAC[MAC_LEN];
        char APSSID[APSSID_MAX_SIZE];          /* AP 信息 */
        AP_ENCRYPT_ENUM APEncrypt;             /*加密类型: 不加密: "OPEN";  */
        WPS_SUPPORT_ENUM APWPS; /* 路由是否支持wps功能 */
        unsigned int  APSignal;/* ap 的信号强度 */
        unsigned int  APFreq;/* ap 的频率 */
        int  Reserve[4];
    } WLAN_AP_INFO_STRU;


    typedef struct WLAN_AP
    {

        unsigned int  APSum;/* ap 的数量 */
        WLAN_AP_INFO_STRU astAPBox[AP_BOX_MAX];

    } WLAN_AP_STRU;

    typedef struct WLAN_CONN_INFO
    {
        char APSSID[APSSID_MAX_SIZE];          /* AP 信息 */
        char APPassWord[APKEY_MAX_SIZE];
        AP_ENCRYPT_ENUM APEncrypt;            //安全类型: 不加密: "OPEN";
        int  Reserve[4];
    } WLAN_CONN_INFO_STRU;

    /*
    **  ************************************************************************
    **                             函数申明
    **  ************************************************************************
    */
    /*****************************************************************************
     函 数 名  : WlanCreat
     功能描述  : wlan 接口创建,如果wlan 存在则开启wpa_supplicant
     输入参数  : void
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanCreat(void);

    /*****************************************************************************
     函 数 名  : WlanDestory
     功能描述  : 销毁wlan wpa_supplicant 退出
     输入参数  : void
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanDestory(void);

    /*****************************************************************************
     函 数 名  : WlanScanAP
     功能描述  : 扫描wlan 热点
     输入参数  : WLAN_AP_STRU *pWlanAP
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanScanAP(WLAN_AP_STRU *pWlanAP);

    /*****************************************************************************
     函 数 名  : WlanConnectAP
     功能描述  : 连接wlan 热点
     输入参数  : WLAN_CONN_INFO_STRU WlanConnInfo
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanConnectAP(WLAN_CONN_INFO_STRU WlanConnInfo);

    /*****************************************************************************
     函 数 名  : WlanWpsConnectAP
     功能描述  : 使用wps方式连接wlan 热点
     输入参数  : WLAN_CONN_INFO_STRU *cfg
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanWpsConnectAP(WLAN_CONN_INFO_STRU *cfg);

    /*****************************************************************************
     函 数 名  : WlanReleaseAP
     功能描述  : 断开wlan 热点
     输入参数  : void
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanReleaseAP(void);

    /*****************************************************************************
     函 数 名  : WlanGetLinkState
     功能描述  : 获取wlan 热点链接状态
     输入参数  : void
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月21日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int WlanGetLinkState(void);

#ifdef __cplusplus
}
#endif

#endif //__WLAN_H__

