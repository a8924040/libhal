/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.h
**  Description   : 串口定义
**  Modification  : 2011/7/19    tango_zhu  创建
**  ************************************************************************
*/
#ifndef __GPS_H__
#define __GPS_H__

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct
    {
        char latitude[32];              /*!<  纬度*/
        char longitude[32];             /*!<  经度*/
        char pos_latitude[4];           /*南北纬N or S*/
        char pos_longitude[4];          /*东西经W or E*/
        char data_vail[4];              /*!<  定位有效性*/
        char satellite_vail_sum[4];                 /*有效卫星*/
        char altitude[32];          /*!<  高度*/
        char speed[32];                 /*!<  速度*/
        char direction[32];
        char time_utc[32];        /*utc 时间*/
        char date_utc[32];        /*utc 日期*/
    } GpsOrientData, *pGpsOrientData;


    /* $Function        :   int GpsDeviceCreate(void)
    ==   ===============================================================
    ==  Description     :   获得串口设备的dev路径/dev/s3c2410_serial2
    ==  Argument        :   无
    ==  Return          :   -2:设备已经打开：-1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpsDeviceCreate(void);

    /* $Function        :   IrDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   初始化红外设备
    ==  Argument        :   无
    ==  Return          :   -1： 初始化失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpsDeviceInit(void);

    /* $Function        :   GpsDeviceWrite(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :
    ==  Argument        :
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpsDeviceWrite(unsigned char *pbuf);

    /* $Function        :   int GpsDeviceRead(GpsOrientData *pgps_orient_data)
    ==   ===============================================================
    ==  Description     :   从gps设备读导航数据
    ==  Argument        :   pgps_orient_data GpsOrientData结构体指针
    ==  Return          :    0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpsDeviceRead(GpsOrientData *pgps_orient_data);

    /* $Function        :   GpsDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   关闭gps设备
    ==  Argument        :   pbuf 往红外设备写数据的buf
    ==  Return          :   -1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpsDeviceClose(void);


#ifdef __cplusplus
}
#endif

#endif
