/*
**  ************************************************************************
**
**  File    Name  : UART.h
**  Description   : 串口相关头文件
**  Modification  : 2011/11/24   tango_zhu   创建
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
                    UartDevice_e; //串口用途定义

    typedef enum
    {
        ComStopBit1 = 1,
        ComStopBit2 = 2
    } UartStopBit_e; //串口stopbit定义

    typedef enum
    {
        ComParityNone = 0,
        ComParityOdd = 1,
        ComParityEven = 2,
        ComParityMark,
        ComParitySpace
    } UartParity_e; //串口stopbit定义

    typedef enum
    {
        ComDataBit5 = 5,
        ComDataBit6 ,
        ComDataBit7 ,
        ComDataBit8

    } UartDataBit_e; //串口databit定义

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
    ==  Description     :   获得串口设备的dev路径/dev/s3c2410_serial
    ==  Argument        :   UartDevCtrl 结构体
    ==  Return          :   -1： 设备为不在设备列表内 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceCreate(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceOpen(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   打开串口设备获得串口fd
    ==  Argument        :   UartDevCtrl 结构体
    ==  Return          :   -1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceOpen(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceInit(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   设置串口属性
    ==  Argument        :   UartDevCtrl 结构体指针
    ==  Return          :   -1： 失败 0： 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceInit(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   阻塞式读串口直到读到要求的数据长度
    ==  Argument        :   UartDevCtrl 结构体,buffer 存储读到的数据，length 要求读的长度
    ==  Return          :   -1： 失败  read_position: 读到长度
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length);

    /* $Function        :   int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   阻塞式读串口直到读到要求的数据长度
    ==  Argument        :   UartDevCtrl 结构体,buffer 存储读到的数据，length 要求读的长度
    ==  Return          :   -1： 失败  read_position: 读到长度
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length);

    /* $Function        :   int  UartDeviceReadNoblock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   非阻塞式读串口直到读到要求的数据长度，读到时间最长为90ms
    ==  Argument        :   UartDevCtrl 结构体,buffer 存储读到的数据，length 要求读的长度
    ==  Return          :   -1： 失败  read_position: 读到长度
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceReadNoblock(UartDevCtrl uart_dev, void *buffer, int length);

    /* $Function        :   int UartDeviceWriteBlock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   阻塞式写串口直到写到要求的数据长度
    ==  Argument        :   UartDevCtrl 结构体,buffer 存储写的数据，length 要求写的长度
    ==  Return          :   -1： 失败  write_position: 写到长度
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */

    int UartDeviceWriteBlock(UartDevCtrl uart_dev, void *buffer, int length);
    /* $Function        :   int UartDeviceWriteNoblock(UartDevCtrl uart_dev, void *buffer, int length)
    ==   ===============================================================
    ==  Description     :   非阻塞式写串口直到写到要求的数据长度
    ==  Argument        :   UartDevCtrl 结构体,buffer 存储写的数据，length 要求写的长度
    ==  Return          :   -1： 失败  write_position: 写到长度
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceWriteNoblock(UartDevCtrl uart_dev, void *buffer, int length);


    /* $Function        :   int UartDeviceClose(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   关闭串口设备
    ==  Argument        :  设置串口fd为-1
    ==  Return          :   -1： 失败  0: 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceClose(UartDevCtrl *puart_dev);

    /* $Function        :   int UartDeviceClose(UartDevCtrl *puart_dev)
    ==   ===============================================================
    ==  Description     :   销毁串口设备
    ==  Argument        :  设置串口UartDevCtrl 为默认值
    ==  Return          :   -1： 失败  0: 成功
    ==  Modification  : 2011/11/24   tango_zhu   创建
    ==   ===============================================================
    */
    int UartDeviceDestory(UartDevCtrl *puart_dev);
#ifdef __cplusplus
}
#endif

#endif
