/*
**  ************************************************************************
**
**  File    Name  : NETWORK.h
**  Description   : ����ӿ����ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
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
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_ip:ip
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkSetIp(char *p_dev, char *p_ip);

    /* $Function        :   int NetWorkSetNetMask(char *p_dev, char *p_netmask)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_netmask:netmask
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkSetNetMask(char *p_dev, char *p_netmask);

    /* $Function        :   int  NetWorkSetDefaultGateWay(char *p_dev, char *p_gateway)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_gateway:gateway
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int  NetWorkSetDefaultGateWay(char *p_dev, char *p_gateway);

    /* $Function        :   int NetWorkSetMac(char *p_dev, char *p_mac)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_mac:mac
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkSetMac(char *p_dev, char *p_mac);

    /* $Function        :   NetWorkGetIp(char *p_dev, char *p_ip)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_ip:ip
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkGetIp(char *p_dev, char *p_ip);

    /* $Function        :   NetWorkGetGateWay(char *p_dev, char *p_gateway)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_p_gateway:p_gateway
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkGetGateWay(char *p_dev, char *p_gateway);

    /* $Function        :   int NetWorkGetMac(char *p_dev, char *p_mac)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_ip:ip
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkGetMac(char *p_dev, char *p_mac);

    /* $Function        :   int NetWorkGetMac(char *p_dev, char *p_mac)
    ==   ===============================================================
    ==  Description     :   ��������ip
    ==  Argument        :   p_dev ����������p_ip:ip
    ==  Return          :   -3��������Ч��-2�������������ļ�ʧ�ܣ�-1�� ����ʧ��  0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int NetWorkGetNetMask(char *p_dev, char *p_netmask);

    /*****************************************************************************
     �� �� ��  : NetWorkGetIpFromUrl
     ��������  : �������url�л�ȡ��ʵ��ip
     �������  : S8 *ps8Url
                 S8 *ps8UrlIP
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��3��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int NetWorkGetIpFromUrl(char *ps8Url, char *ps8UrlIP);
    /*****************************************************************************
     �� �� ��  : NetWorkGetDefaultDev
     ��������  : ��ȡĬ�������豸
     �������  : char *p_dev
                 char *p_gateway
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int NetWorkGetDefaultDev(char *pDefaultDev);
    /*****************************************************************************
     �� �� ��  : CheckIpConflic
     ��������  : ���ip�Ƿ���ã��Ƿ��ͻ
     �������  : char *interface
                 char *ipaddr
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��3��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int CheckIpConflic(char *interface,  char *ipaddr);

    /*****************************************************************************
     �� �� ��  : NetWorkCheckLinkState
     ��������  : �����������״̬
     �������  : char *if_name
     �������  : ��
     �� �� ֵ  : 1:success 0:down -1: failed
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��16��
        ��    ��   : �����������״̬
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int NetWorkCheckLinkState(char *if_name);


#ifdef __cplusplus
}
#endif

#endif
