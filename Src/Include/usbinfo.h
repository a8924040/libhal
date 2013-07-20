/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : Usbinfo.h
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : usb信息获取
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月18日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/

#ifndef __USB_INFO_H__
#define __USB_INFO_H__
#include "CommonInclude.h"
#ifdef __cplusplus
extern "C" {
#endif

#define WIRELESS_POWER_ON               1
#define WIRELESS_POWER_OFF          0
#define USB_MAX_NUM         16      /* USB端口最大数 */
#define IIC_MAX_PIN         16      /* IIC总线最大数 */
#define ALARM_MAX_PIN       32      /* 报警端口最大数 */


#define USB_SUBDEV_COUNT    8                       //一个USB接口对应的设备个数
#define USB_DEV_NAME       12                   //USB设备名长度
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
        S32 s32PortIndex;           /* USB端口逻辑序号 */
        S32 s32UsbBus;              /* USB一级总线地址 */
        S32 s32UsbLev;              /* USB二级总线地址 */
        S32 s32UsbPort;             /* USB三级总线地址 */
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
        USB_SERIAL = 0,                                     //usb转串口
        USB_WIFI,                                           //usb wifi设备
        USB_UNKNOWN                                         //未知usb设备
    } USB_DRIVER_TYPE;                                      //usb设备驱动类型

    typedef struct USB_DRIVER_INFO
    {
        USB_DRIVER_TYPE     driver_type;
        char                driver_name[64];/* 例如usbserial_generic */
    } USB_DRIVER_INFO_STRU;

    typedef struct USB_PORT_RES
    {
        S32             s32UsbNum;                  /* USB端口数 */
        USB_PORT_STRU   astUsbPort[USB_MAX_NUM];    /* USB端口信息 */
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



    // USB子设备描述
    typedef struct
    {
        USB_DRIVER_TYPE     driver_type;
        char                name[USB_DEV_NAME];          //例如：ttyUSB0 wlan0
    } USB_SUBDEV_DESC;

    // USB设备描述 256字节
    typedef struct
    {
        char            manufacturer[32];   //厂商描述
        char            product[32];        //产品描述
        unsigned int    vendor;             //厂商标识
        unsigned int    product_id;         //产品ID
        unsigned int    inerface_index;     //端口编号0-4
        USB_SUBDEV_DESC subdev[USB_SUBDEV_COUNT];
        unsigned int    subdev_count;       //USB设备个数
        USB_DEV_INFO    usb_dev_info;
        char            reserved[32];        //< 保留
    } USB_DEV_DESC;


    /*****************************************************************************
     函 数 名  : GetUsbDevInfo
     功能描述  : 获取USB 设备信息
     输入参数  : USB_DEV_DESC *desc   无线模块数组信息
                 unsigned int *count  无线模块的模块个数
     输出参数  : 无
     返 回 值  : =0    成功
                 <0    失败
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年7月13日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int GetUsbDevInfo(USB_DEV_DESC *pstDesc, unsigned int *pu32Count);



    /*****************************************************************************
     函 数 名  : InitUsbInfo
     功能描述  : 初始化usb接口
     输入参数  : void
     输出参数  : 无
     返 回 值  : 0:成功
                 -1: 失败
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年7月23日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/

    int InitUsbInfo(void);


    /*****************************************************************************
     函 数 名  : power_on_multusb
     功能描述  : 开启USB模块电源 针对多模块
     输入参数  : int index 模块编号
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年7月23日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int power_on_multusb(int index);

    /*****************************************************************************
     函 数 名  : power_off_multusb
     功能描述  : 关闭USB模块电源 （针对多模块）
     输入参数  : int index 模块编号
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年7月23日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int power_off_multusb(int index);

    /*****************************************************************************
     函 数 名  : reset_multusb_module
     功能描述  : 重启USB模块无阻塞版本（针对多模块）
     输入参数  : int index 模块编号
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年7月23日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int reset_multusb_module(int index);

    /*****************************************************************************
     函 数 名  : reset_usbhub_module
     功能描述  : 重启外置usb hub
     输入参数  : int index: hub 编号
     输出参数  : 无
     返 回 值  : 0: 成功
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年7月23日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int reset_usbhub_module(int index);


#ifdef __cplusplus
}
#endif

#endif
