/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : dvr_wlan.c
**  Description   : DVR WIFI�ӿ�
**  Modification  : 2011/10/14    huang_zhendi    ����
**  ************************************************************************
*/
#ifndef __WLAN_H__
#define __WLAN_H__

#ifdef __cplusplus
extern "C" {
#endif

    /*
    **  ************************************************************************
    **                             �궨��
    **  ************************************************************************
    */
#define MAC_LEN 32                  //MAC��ַ��󳤶�
#define APSSID_MAX_SIZE 64
#define APKEY_MAX_SIZE  64
#define AP_BOX_MAX   64   /* ���֧��64��AP��Ϣ */

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
        char APSSID[APSSID_MAX_SIZE];          /* AP ��Ϣ */
        AP_ENCRYPT_ENUM APEncrypt;             /*��������: ������: "OPEN";  */
        WPS_SUPPORT_ENUM APWPS; /* ·���Ƿ�֧��wps���� */
        unsigned int  APSignal;/* ap ���ź�ǿ�� */
        unsigned int  APFreq;/* ap ��Ƶ�� */
        int  Reserve[4];
    } WLAN_AP_INFO_STRU;


    typedef struct WLAN_AP
    {

        unsigned int  APSum;/* ap ������ */
        WLAN_AP_INFO_STRU astAPBox[AP_BOX_MAX];

    } WLAN_AP_STRU;

    typedef struct WLAN_CONN_INFO
    {
        char APSSID[APSSID_MAX_SIZE];          /* AP ��Ϣ */
        char APPassWord[APKEY_MAX_SIZE];
        AP_ENCRYPT_ENUM APEncrypt;            //��ȫ����: ������: "OPEN";
        int  Reserve[4];
    } WLAN_CONN_INFO_STRU;

    /*
    **  ************************************************************************
    **                             ��������
    **  ************************************************************************
    */
    /*****************************************************************************
     �� �� ��  : WlanCreat
     ��������  : wlan �ӿڴ���,���wlan ��������wpa_supplicant
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanCreat(void);

    /*****************************************************************************
     �� �� ��  : WlanDestory
     ��������  : ����wlan wpa_supplicant �˳�
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanDestory(void);

    /*****************************************************************************
     �� �� ��  : WlanScanAP
     ��������  : ɨ��wlan �ȵ�
     �������  : WLAN_AP_STRU *pWlanAP
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanScanAP(WLAN_AP_STRU *pWlanAP);

    /*****************************************************************************
     �� �� ��  : WlanConnectAP
     ��������  : ����wlan �ȵ�
     �������  : WLAN_CONN_INFO_STRU WlanConnInfo
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanConnectAP(WLAN_CONN_INFO_STRU WlanConnInfo);

    /*****************************************************************************
     �� �� ��  : WlanWpsConnectAP
     ��������  : ʹ��wps��ʽ����wlan �ȵ�
     �������  : WLAN_CONN_INFO_STRU *cfg
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanWpsConnectAP(WLAN_CONN_INFO_STRU *cfg);

    /*****************************************************************************
     �� �� ��  : WlanReleaseAP
     ��������  : �Ͽ�wlan �ȵ�
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanReleaseAP(void);

    /*****************************************************************************
     �� �� ��  : WlanGetLinkState
     ��������  : ��ȡwlan �ȵ�����״̬
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��21��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int WlanGetLinkState(void);

#ifdef __cplusplus
}
#endif

#endif //__WLAN_H__

