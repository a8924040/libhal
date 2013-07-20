
/*
**  ************************************************************************
**
**  File    Name  : GPIO_IIC.h
**  Description   : GPIO_IIC���ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
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
    ==  Description     :   ���gpio�豸��dev·��/dev/gpio_i2c
    ==  Argument        :   ��
    ==  Return          :   -2:�豸�Ѿ��򿪣�-1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioI2CDeviceCreate(void);

    /* $Function        :   GpioI2CDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   ��ʼ�������豸
    ==  Argument        :   ��
    ==  Return          :   -1�� ��ʼ��ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioI2CDeviceInit(void);

    /* $Function        :   int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
    ==   ===============================================================
    ==  Description     :   ��gpio iic �豸������
    ==  Argument        :   pgpio_iic_cmd_data ���� GPIO_IIC_CMD_DATA ���ݸ�ʽ������
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioI2CDeviceRead(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data);

    /* $Function        :   int GpioI2CDeviceWrite(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data)
    ==   ===============================================================
    ==  Description     :   ��gpio iic �豸д����
    ==  Argument        :   pgpio_iic_cmd_data ���� GPIO_IIC_CMD_DATA ���ݸ�ʽ������
    ==  Return          :   -1�� ioctl ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioI2CDeviceWrite(GPIO_IIC_CMD_DATA *pgpio_iic_cmd_data);

    /* $Function        :   GpioI2CDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   �ر�gpio fd
    ==  Argument        :   ��
    ==  Return          :   -1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioI2CDeviceClose(void);

    /* $Function        :   GpioI2CDeviceDestory(void)
    ==   ===============================================================
    ==  Description     :   ���gpio�豸��dev·��/dev/gpio_i2c
    ==  Argument        :   ��
    ==  Return          :   0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioI2CDeviceDestory(void);

#ifdef __cplusplus
}
#endif

#endif
