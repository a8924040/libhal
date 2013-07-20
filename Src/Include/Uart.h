/*
**  ************************************************************************
**
**  File    Name  : UART.h
**  Description   : �������ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#define UART_DEVICE_MAX 3
#define UART_DEVICE_HEAD "/dev/s3c2410_serial"
    typedef enum
    {
        UartDeviceDebug = 0,
        UartDeviceIr = 1,
        UartDeviceGps = 2
    }
                    UartDevice_e; //������;����

    typedef enum
    {
        ComStopBit1 = 1,
        ComStopBit2 = 2
    } UartStopBit_e; //����stopbit����

    typedef enum
    {
        ComParityNone = 0,
        ComParityOdd = 1,
        ComParityEven = 2,
        ComParityMark,
        ComParitySpace
    } UartParity_e; //����stopbit����

    typedef enum
    {
        ComDataBit5 = 5,
        ComDataBit6 ,
        ComDataBit7 ,
        ComDataBit8

    } UartDataBit_e; //����databit����

    typedef struct
    {
        int baudrate;
        int bits_data;
        int bits_stop;
        int parity;
    } UartAttribute;

    typedef struct __uart_dev
    {
        int fd;                   /* tty file descriptor */
        char uart_dev[32];
        UartDevice_e  uart_index;
        UartAttribute uart_attr;
    } UartDevCtrl;

    /* $Function        :   UartDeviceCreate(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   ��ô����豸��dev·��/dev/s3c2410_serial
    ==  Argument        :   UartDevCtrl �ṹ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceCreate(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceOpen(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   �򿪴����豸��ô���fd
    ==  Argument        :   UartDevCtrl �ṹ��
    ==  Return          :   -1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceOpen(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceInit(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   ���ô�������
    ==  Argument        :   UartDevCtrl �ṹ��ָ��
    ==  Return          :   -1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceInit(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   ����ʽ������ֱ������Ҫ������ݳ���
    ==  Argument        :   UartDevCtrl �ṹ��,buffer �洢���������ݣ�length Ҫ����ĳ���
    ==  Return          :   -1�� ʧ��  read_position: ��������
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length);

    /* $Function        :   int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   ����ʽ������ֱ������Ҫ������ݳ���
    ==  Argument        :   UartDevCtrl �ṹ��,buffer �洢���������ݣ�length Ҫ����ĳ���
    ==  Return          :   -1�� ʧ��  read_position: ��������
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length);

    /* $Function        :   int  UartDeviceReadNoblock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   ������ʽ������ֱ������Ҫ������ݳ��ȣ�����ʱ���Ϊ90ms
    ==  Argument        :   UartDevCtrl �ṹ��,buffer �洢���������ݣ�length Ҫ����ĳ���
    ==  Return          :   -1�� ʧ��  read_position: ��������
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceReadNoblock(UartDevCtrl uart_dev, void *buffer, int length);

    /* $Function        :   int UartDeviceWriteBlock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   ����ʽд����ֱ��д��Ҫ������ݳ���
    ==  Argument        :   UartDevCtrl �ṹ��,buffer �洢д�����ݣ�length Ҫ��д�ĳ���
    ==  Return          :   -1�� ʧ��  write_position: д������
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */

    int UartDeviceWriteBlock(UartDevCtrl uart_dev, void *buffer, int length);
    /* $Function        :   int UartDeviceWriteNoblock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   ������ʽд����ֱ��д��Ҫ������ݳ���
    ==  Argument        :   UartDevCtrl �ṹ��,buffer �洢д�����ݣ�length Ҫ��д�ĳ���
    ==  Return          :   -1�� ʧ��  write_position: д������
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceWriteNoblock(UartDevCtrl uart_dev, void *buffer, int length);


    /* $Function        :   int UartDeviceClose(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   �رմ����豸
    ==  Argument        :  ���ô���fdΪ-1
    ==  Return          :   -1�� ʧ��  0: �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceClose(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceClose(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   ���ٴ����豸
    ==  Argument        :  ���ô���UartDevCtrl ΪĬ��ֵ
    ==  Return          :   -1�� ʧ��  0: �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int UartDeviceDestory(UartDevCtrl *puart_dev);
#ifdef __cplusplus
}
#endif

#endif
