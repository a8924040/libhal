
/*
**  ************************************************************************
**
**  File    Name  : GPIO_IIC.h
**  Description   : GPIO_IIC相关头文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __GPIO_IIC_H__
#define __GPIO_IIC_H__

#ifdef __cplusplus
extern "C" {
#endif


    typedef struct
    {
        unsigned char slave_address;
        unsigned char sub_mod;
        unsigned char data[128];
        unsigned int  sub_address;
        unsigned int  len;
    } GPIO_IIC_CMD_DATA;

    /* $Function        :   GpioI2CDevicCreate(void)
    ==   ===============================================================
    ==  Description     :   获得gpio设备的dev路径/dev/gpio_i2c
    ==  Argument        :   无
    ==  Return          :   -2:设备已经打开：-1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioI2CDeviceCreate(void);

    /* $Function        :   GpioI2CDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   初始化红外设备
    ==  Argument        :   无
    ==  Return          :   -1： 初始化失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioI2CDeviceInit(void);

    /* $Function        :   int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
    ==   ===============================================================
    ==  Description     :   从gpio iic 设备读数据
    ==  Argument        :   pgpio_iic_cmd_data 符合 GPIO_IIC_CMD_DATA 数据格式的数据
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data);

    /* $Function        :   int GpioI2CDeviceWrite(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
    ==   ===============================================================
    ==  Description     :   从gpio iic 设备写数据
    ==  Argument        :   pgpio_iic_cmd_data 符合 GPIO_IIC_CMD_DATA 数据格式的数据
    ==  Return          :   -1： ioctl 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioI2CDeviceWrite(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data);

    /* $Function        :   GpioI2CDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   关闭gpio fd
    ==  Argument        :   无
    ==  Return          :   -1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioI2CDeviceClose(void);

    /* $Function        :   GpioI2CDeviceDestory(void)
    ==   ===============================================================
    ==  Description     :   获得gpio设备的dev路径/dev/gpio_i2c
    ==  Argument        :   无
    ==  Return          :   0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int GpioI2CDeviceDestory(void);

#ifdef __cplusplus
}
#endif

#endif
