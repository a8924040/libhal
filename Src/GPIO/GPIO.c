/*
**  ************************************************************************
**
**  File    Name  : gpio.c
**
**  Description   : gpio����ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              ͷ�ļ�
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
==  Description     :   ���gpio�豸��dev·��/dev/gpio
==  Argument        :   ��
==  Return          :   -2:�豸�Ѿ��򿪣�-1�� ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioDeviceCreate(void)
{
    if(fd_gpio > 0)
    {
        return -2;//�����ظ���
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
==  Description     :   ��ʼ�������豸
==  Argument        :   ��
==  Return          :   -1�� ��ʼ��ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioDeviceInit(void)
{
    return 0;
}

/* $Function        :   IrDeviceRead(unsigned char *pbuf)
==   ===============================================================
==  Description     :   ��gpio�豸������
==  Argument        :   pbuf �Ӻ����豸���������ݵ�buf
==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioDeviceRead(unsigned char *pbuf)
{
    return 0;
}

/* $Function        :   GpioDeviceWrite(unsigned char *pbuf)
==   ===============================================================
==  Description     :   ��gpio�豸������
==  Argument        :   pgpio_data �������豸д���ݵ�WRITE_GPIO_DATA
==  Return          :   -1�� ioctl ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
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
==  Description     :   �ر�gpio fd
==  Argument        :   ��
==  Return          :   -1�� ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioDeviceClose(void)
{
    return 0;
}

/* $Function        :   GpioDeviceDestory(void)
==   ===============================================================
==  Description     :   ���gpio�豸��dev·��/dev/gpio
==  Argument        :   ��
==  Return          :   0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int GpioDeviceDestory(void)
{
    close(fd_gpio);    //
    return 0;
}

