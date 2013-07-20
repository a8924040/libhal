/*
**  ************************************************************************
**
**  File    Name  : IR.h
**  Description   : �������ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
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
    ==  Description     :   ��ô����豸��dev·��/dev/ir
    ==  Argument        :   ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioDeviceCreate(void);

    /* $Function        :   GpioDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   ��ʼ�������豸
    ==  Argument        :   ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioDeviceInit(void);

    /* $Function        :   GpioDeviceRead(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   �Ӻ����豸������
    ==  Argument        :   pbuf �Ӻ����豸���������ݵ�buf
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioDeviceRead(unsigned char *pbuf);

    /* $Function        :   GpioDeviceWrite(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   �Ӻ����豸������
    ==  Argument        :   pbuf �������豸д���ݵ�buf
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioDeviceWrite(WRITE_GPIO_DATA *pgpio_data);

    /* $Function        :   GpioDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   �Ӻ����豸������
    ==  Argument        :   pbuf �������豸д���ݵ�buf
    ==  Return          :   -1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioDeviceClose(void);

    /* $Function        :   GpioDeviceDestory(void)
    ==   ===============================================================
    ==  Description     :   ��ô����豸��dev·��/dev/ir
    ==  Argument        :   ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpioDeviceDestory(void);

#ifdef __cplusplus
}
#endif

#endif
