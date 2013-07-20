/*
**  ************************************************************************
**
**  File    Name  : NETWORK.h
**  Description   : 网络接口相关头文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __NETWORK_H__
#define __NETWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct
    {
        unsigned long long Rx_bytes;
        unsigned long long Tx_bytes;

    } NetWorkFlow_T;

    typedef struct NETWORK_ATTRIBUTE
    {
        char net_attr_ip[32];
        char net_attr_mac[32];
        char net_attr_getway[32];
        char net_attr_netmask[32];
    } NETWORK_ATTRIBUTE_STRU;

    typedef struct NETWORK_INTERFACE
    {
        int NetWorkInterfaceSum;
        char NetWorkInterface[16][32];
    } NETWORK_INTERFACE_STRU;
    /* $Function        :   NetWorkSetIp(char *p_dev, char *p_ip)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_ip:ip
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkSetIp(char *p_dev, char *p_ip);

    /* $Function        :   int NetWorkSetNetMask(char *p_dev, char *p_netmask)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_netmask:netmask
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkSetNetMask(char *p_dev, char *p_netmask);

    /* $Function        :   int  NetWorkSetDefaultGateWay(char *p_dev, char *p_gateway)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_gateway:gateway
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int  NetWorkSetDefaultGateWay(char *p_dev, char *p_gateway);

    /* $Function        :   int NetWorkSetMac(char *p_dev, char *p_mac)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_mac:mac
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkSetMac(char *p_dev, char *p_mac);

    /* $Function        :   NetWorkGetIp(char *p_dev, char *p_ip)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_ip:ip
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkGetIp(char *p_dev, char *p_ip);

    /* $Function        :   NetWorkGetGateWay(char *p_dev, char *p_gateway)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_p_gateway:p_gateway
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkGetGateWay(char *p_dev, char *p_gateway);

    /* $Function        :   int NetWorkGetMac(char *p_dev, char *p_mac)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_ip:ip
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkGetMac(char *p_dev, char *p_mac);

    /* $Function        :   int NetWorkGetMac(char *p_dev, char *p_mac)
    ==   ===============================================================
    ==  Description     :   设置网卡ip
    ==  Argument        :   p_dev ：网卡名，p_ip:ip
    ==  Return          :   -3：参数无效，-2：保存在配置文件失败，-1： 设置失败  0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int NetWorkGetNetMask(char *p_dev, char *p_netmask);

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
    int NetWorkGetIpFromUrl(char *ps8Url, char *ps8UrlIP);
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
    int NetWorkGetDefaultDev(char *pDefaultDev);
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
    int CheckIpConflic(char *interface,  char *ipaddr);

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
    int NetWorkCheckLinkState(char *if_name);


#ifdef __cplusplus
}
#endif

#endif
