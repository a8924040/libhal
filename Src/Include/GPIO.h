/*
**  ************************************************************************
**
**  File    Name  : IR.h
**  Description   : 红外相关头文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif


    typedef struct
    {
        unsigned char port;
        unsigned int  subpin;
        unsigned int  val;

    } WRITE_GPIO_DATA;

    /* $Function        :   GpioDeviceCreate(void)
    ==   ===============================================================
    ==  Description     :   获得串口设备的dev路径/dev/ir
    ==  Argument        :   无
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioDeviceCreate(void);

    /* $Function        :   GpioDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   初始化红外设备
    ==  Argument        :   无
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioDeviceInit(void);

    /* $Function        :   GpioDeviceRead(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   从红外设备读数据
    ==  Argument        :   pbuf 从红外设备读到的数据的buf
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioDeviceRead(unsigned char *pbuf);

    /* $Function        :   GpioDeviceWrite(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   从红外设备读数据
    ==  Argument        :   pbuf 往红外设备写数据的buf
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioDeviceWrite(WRITE_GPIO_DATA *pgpio_data);

    /* $Function        :   GpioDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   从红外设备读数据
    ==  Argument        :   pbuf 往红外设备写数据的buf
    ==  Return          :   -1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioDeviceClose(void);

    /* $Function        :   GpioDeviceDestory(void)
    ==   ===============================================================
    ==  Description     :   获得串口设备的dev路径/dev/ir
    ==  Argument        :   无
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioDeviceDestory(void);

#ifdef __cplusplus
}
#endif

#endif
