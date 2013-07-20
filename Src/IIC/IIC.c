/*
**  ************************************************************************
**
**  File    Name  : IIC.c
**
**  Description   : IIC相关文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              头文件
**  ************************************************************************
*/
#include "CommonInclude.h"
#include "GPIO_IIC.h"
#include "Log.h"

int fd_gpio_i2c = -1;
#define GPIO_IIC_DEVICES "/dev/gpio_i2c"

#define GPIO_IIC_IOCTL_MAGIC   'I'
#define GPIO_IIC_IOCTL_WRITE  _IOW(GPIO_IIC_IOCTL_MAGIC, 0, int)
#define GPIO_IIC_IOCTL_READ   _IOW(GPIO_IIC_IOCTL_MAGIC, 1, int)



/* $Function        :   GpioI2CDevicCreate(void)
==   ===============================================================
==  Description     :   获得gpio设备的dev路径/dev/gpio_i2c
==  Argument        :   无
==  Return          :   -2:设备已经打开：-1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioI2CDeviceCreate(void)
{
    if(fd_gpio_i2c > 0)
    {
        return -2;//避免重复打开
    }
    if((fd_gpio_i2c = open(GPIO_IIC_DEVICES, O_RDWR)) <= 0)
    {
        LIBHAL_ERROR("open GPIO IIC devices failed");
        return -1;
    }
    return 0;
}


/* $Function        :   GpioI2CDeviceInit(void)
==   ===============================================================
==  Description     :   初始化红外设备
==  Argument        :   无
==  Return          :   -1： 初始化失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioI2CDeviceInit(void)
{
    return 0;
}

/* $Function        :   int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
==   ===============================================================
==  Description     :   从gpio iic 设备读数据
==  Argument        :   pgpio_iic_cmd_data 符合 GPIO_IIC_CMD_DATA 数据格式的数据
==  Return          :   -1： 设备为不在设备列表内 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
{
    int cmd =  GPIO_IIC_IOCTL_READ;
    if(pgpio_iic_cmd_data ->len > 128)
    {
        LIBHAL_ERROR("the gpio iic read max is 128");
        return -1;
    }
    if(ioctl(fd_gpio_i2c, cmd, pgpio_iic_cmd_data) < 0)
    {
        LIBHAL_ERROR("Call gpio ioctl read fail");
        return -1;
    }
    else
    {
        return 0;
    }
}

/* $Function        :   int GpioI2CDeviceWrite(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
==   ===============================================================
==  Description     :   从gpio iic 设备写数据
==  Argument        :   pgpio_iic_cmd_data 符合 GPIO_IIC_CMD_DATA 数据格式的数据
==  Return          :   -1： ioctl 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioI2CDeviceWrite(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
{
    int cmd =  GPIO_IIC_IOCTL_WRITE;
    if(pgpio_iic_cmd_data ->len > 128)
    {
        LIBHAL_ERROR("the gpio iic write max is 128");
        return -1;
    }
    if(ioctl(fd_gpio_i2c, cmd, pgpio_iic_cmd_data) < 0)
    {
        LIBHAL_ERROR("Call gpio iic write ioctl  fail");
        return -1;
    }
    else
    {
        return 0;
    }
}

/* $Function        :   GpioI2CDeviceClose(void)
==   ===============================================================
==  Description     :   关闭gpio fd
==  Argument        :   无
==  Return          :   -1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioI2CDeviceClose(void)
{
    return 0;
}

/* $Function        :   GpioI2CDeviceDestory(void)
==   ===============================================================
==  Description     :   获得gpio设备的dev路径/dev/gpio
==  Argument        :   无
==  Return          :   0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioI2CDeviceDestory(void)
{
    close(fd_gpio_i2c);    //
    return 0;
}
