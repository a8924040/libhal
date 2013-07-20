/*
**  ************************************************************************
**
**  File    Name  : IR.h
**  Description   : 红外相关头文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __IR_H__
#define __IR_H__

#ifdef __cplusplus
extern "C" {
#endif

    /* $Function        :   IrDeviceCreate(void)
    ==   ===============================================================
    ==  Description     :   获得串口设备的dev路径/dev/ir
    ==  Argument        :   无
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int IrDeviceCreate(void);

    /* $Function        :   IrDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   初始化红外设备
    ==  Argument        :   无
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int IrDeviceInit(void);

    /* $Function        :   IrDeviceRead(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   从红外设备读数据
    ==  Argument        :   pbuf 从红外设备读到的数据的buf
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int IrDeviceRead(unsigned char *pbuf);

    /* $Function        :   IrDeviceWrite(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   从红外设备读数据
    ==  Argument        :   pbuf 往红外设备写数据的buf
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int IrDeviceWrite(unsigned char *pbuf);

    /* $Function        :   IrDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   从红外设备读数据
    ==  Argument        :   pbuf 往红外设备写数据的buf
    ==  Return          :   -1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int IrDeviceClose(void);

    /* $Function        :   IrDeviceDestory(void)
    ==   ===============================================================
    ==  Description     :   获得串口设备的dev路径/dev/ir
    ==  Argument        :   无
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int IrDeviceDestory(void);

#ifdef __cplusplus
}
#endif

#endif
