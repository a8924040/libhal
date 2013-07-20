/*
**  ************************************************************************
**
**  File    Name  : IIC.c
**
**  Description   : IIC����ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              ͷ�ļ�
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
==  Description     :   ���gpio�豸��dev·��/dev/gpio_i2c
==  Argument        :   ��
==  Return          :   -2:�豸�Ѿ��򿪣�-1�� ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioI2CDeviceCreate(void)
{
    if(fd_gpio_i2c > 0)
    {
        return -2;//�����ظ���
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
==  Description     :   ��ʼ�������豸
==  Argument        :   ��
==  Return          :   -1�� ��ʼ��ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioI2CDeviceInit(void)
{
    return 0;
}

/* $Function        :   int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
==   ===============================================================
==  Description     :   ��gpio iic �豸������
==  Argument        :   pgpio_iic_cmd_data ���� GPIO_IIC_CMD_DATA ���ݸ�ʽ������
==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
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
==  Description     :   ��gpio iic �豸д����
==  Argument        :   pgpio_iic_cmd_data ���� GPIO_IIC_CMD_DATA ���ݸ�ʽ������
==  Return          :   -1�� ioctl ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
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
==  Description     :   �ر�gpio fd
==  Argument        :   ��
==  Return          :   -1�� ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioI2CDeviceClose(void)
{
    return 0;
}

/* $Function        :   GpioI2CDeviceDestory(void)
==   ===============================================================
==  Description     :   ���gpio�豸��dev·��/dev/gpio
==  Argument        :   ��
==  Return          :   0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioI2CDeviceDestory(void)
{
    close(fd_gpio_i2c);    //
    return 0;
}
