/*
**  ************************************************************************
**
**  File    Name  : gpio.c
**
**  Description   : gpio相关文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              头文件
**  ************************************************************************
*/
#include "CommonInclude.h"
#include "GPIO.h"
#include "Log.h"

int fd_gpio = -1;
#define GPIODEVICES "/dev/gpio"

#define GPIO_IOCTL_MAGIC   'g'
#define GPIO_IOCTL_WRITE  _IOW(GPIO_IOCTL_MAGIC, 0, int)
#define GPIO_IOCTL_READ  _IOW(GPIO_IOCTL_MAGIC, 1, int)

/* $Function        :   GpioDeviceCreate(void)
==   ===============================================================
==  Description     :   获得gpio设备的dev路径/dev/gpio
==  Argument        :   无
==  Return          :   -2:设备已经打开：-1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioDeviceCreate(void)
{
    if(fd_gpio > 0)
    {
        return -2;//避免重复打开
    }
    if((fd_gpio = open(GPIODEVICES, O_RDWR)) <= 0)
    {
        LIBHAL_ERROR("open GPIO devices failed");
        return -1;
    }
    return 0;
}


/* $Function        :   GpioDeviceInit(void)
==   ===============================================================
==  Description     :   初始化红外设备
==  Argument        :   无
==  Return          :   -1： 初始化失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioDeviceInit(void)
{
    return 0;
}

/* $Function        :   IrDeviceRead(unsigned char *pbuf)
==   ===============================================================
==  Description     :   从gpio设备读数据
==  Argument        :   pbuf 从红外设备读到的数据的buf
==  Return          :   -1： 设备为不在设备列表内 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioDeviceRead(unsigned char *pbuf)
{
    return 0;
}

/* $Function        :   GpioDeviceWrite(unsigned char *pbuf)
==   ===============================================================
==  Description     :   从gpio设备读数据
==  Argument        :   pgpio_data 往红外设备写数据的WRITE_GPIO_DATA
==  Return          :   -1： ioctl 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioDeviceWrite(WRITE_GPIO_DATA *pgpio_data)
{
    int cmd =  GPIO_IOCTL_WRITE;
    if(ioctl(fd_gpio, cmd, pgpio_data) < 0)
    {
        LIBHAL_ERROR("Call gpio ioctl  fail");
        return -1;
    }
    else
    {
        return 0;
    }
}

/* $Function        :   GpioDeviceClose(void)
==   ===============================================================
==  Description     :   关闭gpio fd
==  Argument        :   无
==  Return          :   -1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioDeviceClose(void)
{
    return 0;
}

/* $Function        :   GpioDeviceDestory(void)
==   ===============================================================
==  Description     :   获得gpio设备的dev路径/dev/gpio
==  Argument        :   无
==  Return          :   0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int GpioDeviceDestory(void)
{
    close(fd_gpio);    //
    return 0;
}

