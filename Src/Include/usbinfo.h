/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  �� �� ��   : Usbinfo.h
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : usb��Ϣ��ȡ
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��18��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __USB_INFO_H__
#define __USB_INFO_H__
#include "CommonInclude.h"
#ifdef __cplusplus
extern "C" {
#endif

#define WIRELESS_POWER_ON               1
#define WIRELESS_POWER_OFF          0
#define USB_MAX_NUM         16      /* USB�˿������ */
#define IIC_MAX_PIN         16      /* IIC��������� */
#define ALARM_MAX_PIN       32      /* �����˿������ */


#define USB_SUBDEV_COUNT    8                       //һ��USB�ӿڶ�Ӧ���豸����
#define USB_DEV_NAME       12                   //USB�豸������
#define WLAN_USBNAME      "wlan"
#define MAX_USB_NUM        5

    typedef struct
    {
        int     bus;
        int     lev;
        int     prnt;
        int     port;
    } USB_DEV_INFO;


    struct usb_powerport
    {
        int prod_type;
        int bus;
        int lev;
        int powerport;
    };

    typedef struct USB_PORT
    {
        S32 s32PortIndex;           /* USB�˿��߼���� */
        S32 s32UsbBus;              /* USBһ�����ߵ�ַ */
        S32 s32UsbLev;              /* USB�������ߵ�ַ */
        S32 s32UsbPort;             /* USB�������ߵ�ַ */
        S32 s32Reserv[4];
    } USB_PORT_STRU;

    /* add by heliangbin  */
    typedef enum KERNERL_VER
    {
        LINUX_2_6_18 = 18,
        LINUX_2_6_23 = 23,
    } KERNERL_VER_STRU;

    typedef struct KERNEL_VERSION
    {
        int ver_first;
        int ver_sec;
        int ver_third;
    } KERNERL_VERSION_STRU;
    typedef enum
    {
        USB_SERIAL = 0,                                     //usbת����
        USB_WIFI,                                           //usb wifi�豸
        USB_UNKNOWN                                         //δ֪usb�豸
    } USB_DRIVER_TYPE;                                      //usb�豸��������

    typedef struct USB_DRIVER_INFO
    {
        USB_DRIVER_TYPE     driver_type;
        char                driver_name[64];/* ����usbserial_generic */
    } USB_DRIVER_INFO_STRU;

    typedef struct USB_PORT_RES
    {
        S32             s32UsbNum;                  /* USB�˿��� */
        USB_PORT_STRU   astUsbPort[USB_MAX_NUM];    /* USB�˿���Ϣ */
    } USB_PORT_RES_STRU;


    typedef struct
    {
        int  index;
        int  bus;
        int  lev;
        char subdev_num[8];
        int  subdev_count;
    } subdev_info;

    typedef struct FILE_FIND
    {
        char *file_name;
        char *path_name;
        char *result_buff;
        int  buff_length;
        int (* deal_result)(struct FILE_FIND *find_pt);
    } FILE_FIND;



    // USB���豸����
    typedef struct
    {
        USB_DRIVER_TYPE     driver_type;
        char                name[USB_DEV_NAME];          //���磺ttyUSB0 wlan0
    } USB_SUBDEV_DESC;

    // USB�豸���� 256�ֽ�
    typedef struct
    {
        char            manufacturer[32];   //��������
        char            product[32];        //��Ʒ����
        unsigned int    vendor;             //���̱�ʶ
        unsigned int    product_id;         //��ƷID
        unsigned int    inerface_index;     //�˿ڱ��0-4
        USB_SUBDEV_DESC subdev[USB_SUBDEV_COUNT];
        unsigned int    subdev_count;       //USB�豸����
        USB_DEV_INFO    usb_dev_info;
        char            reserved[32];        //< ����
    } USB_DEV_DESC;


    /*****************************************************************************
     �� �� ��  : GetUsbDevInfo
     ��������  : ��ȡUSB �豸��Ϣ
     �������  : USB_DEV_DESC *desc   ����ģ��������Ϣ
                 unsigned int *count  ����ģ���ģ�����
     �������  : ��
     �� �� ֵ  : =0    �ɹ�
                 <0    ʧ��
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��13��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int GetUsbDevInfo(USB_DEV_DESC *pstDesc, unsigned int *pu32Count);



    /*****************************************************************************
     �� �� ��  : InitUsbInfo
     ��������  : ��ʼ��usb�ӿ�
     �������  : void
     �������  : ��
     �� �� ֵ  : 0:�ɹ�
                 -1: ʧ��
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��23��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/

    int InitUsbInfo(void);


    /*****************************************************************************
     �� �� ��  : power_on_multusb
     ��������  : ����USBģ���Դ ��Զ�ģ��
     �������  : int index ģ����
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��23��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int power_on_multusb(int index);

    /*****************************************************************************
     �� �� ��  : power_off_multusb
     ��������  : �ر�USBģ���Դ ����Զ�ģ�飩
     �������  : int index ģ����
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��23��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int power_off_multusb(int index);

    /*****************************************************************************
     �� �� ��  : reset_multusb_module
     ��������  : ����USBģ���������汾����Զ�ģ�飩
     �������  : int index ģ����
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��23��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int reset_multusb_module(int index);

    /*****************************************************************************
     �� �� ��  : reset_usbhub_module
     ��������  : ��������usb hub
     �������  : int index: hub ���
     �������  : ��
     �� �� ֵ  : 0: �ɹ�
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��23��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int reset_usbhub_module(int index);


#ifdef __cplusplus
}
#endif

#endif
