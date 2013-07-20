/*
**  ************************************************************************
**
**  File    Name  : Uart.c
**  Description   : 串口相关文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              头文件
**  ************************************************************************
*/
#include "CommonInclude.h"
#include "Uart.h"
#include "Log.h"

/* $Function        :   UartDeviceCreate(UartDevCtrl *puart_dev)
==   ===============================================================
==  Description     :   获得串口设备的dev路径/dev/s3c2410_serial
==  Argument        :   UartDevCtrl 结构体
==  Return          :   -1： 设备为不在设备列表内 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int UartDeviceCreate(UartDevCtrl *puart_dev)
{
    if(puart_dev->uart_index == UartDeviceDebug)
    {
        memset(puart_dev->uart_dev, 0, sizeof(puart_dev->uart_dev));
        sprintf(puart_dev->uart_dev, UART_DEVICE_HEAD"%d", UartDeviceDebug);
        if(puart_dev->uart_attr.baudrate == 0)
        {
            puart_dev->uart_attr.baudrate = 115200 ;
            puart_dev->uart_attr.bits_data = ComDataBit8;
            puart_dev->uart_attr.parity = ComParityNone;
            puart_dev->uart_attr.bits_stop = ComStopBit1;
        }
        LIBHAL_DEBUG("will open uart dev is %s", puart_dev->uart_dev);
    }
    else if(puart_dev->uart_index == UartDeviceIr)
    {
        memset(puart_dev->uart_dev, 0, sizeof(puart_dev->uart_dev));
        sprintf(puart_dev->uart_dev, UART_DEVICE_HEAD"%d", UartDeviceIr);
        puart_dev->uart_attr.baudrate = 9600 ;
        puart_dev->uart_attr.bits_data = ComDataBit8;
        puart_dev->uart_attr.parity = ComParityNone;
        puart_dev->uart_attr.bits_stop = ComStopBit1;
        LIBHAL_DEBUG("will open uart dev is %s", puart_dev->uart_dev);
    }
    else if(puart_dev->uart_index == UartDeviceGps)
    {
        memset(puart_dev->uart_dev, 0, sizeof(puart_dev->uart_dev));
        sprintf(puart_dev->uart_dev, UART_DEVICE_HEAD"%d", UartDeviceGps);
        if(puart_dev->uart_attr.baudrate == 0)
        {
            puart_dev->uart_attr.baudrate = 9600 ;
            puart_dev->uart_attr.bits_data = ComDataBit8;
            puart_dev->uart_attr.parity = ComParityNone;
            puart_dev->uart_attr.bits_stop = ComStopBit1;
        }
        LIBHAL_DEBUG("will open uart dev is %s", puart_dev->uart_dev);
    }
    else
    {
        LIBHAL_ERROR("UartDeviceCreate failed");
        return -1;
    }
    return 0;
}

/* $Function        :   int UartDeviceOpen(UartDevCtrl *puart_dev)
==   ===============================================================
==  Description     :   打开串口设备获得串口fd
==  Argument        :   UartDevCtrl 结构体
==  Return          :   -1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/

int UartDeviceOpen(UartDevCtrl *puart_dev)
{
    if(NULL == puart_dev->uart_dev)
    {
        LIBHAL_ERROR("open dev is null");
        return -1;
    }
    puart_dev->fd = open(puart_dev->uart_dev, O_RDWR | O_NOCTTY);
    if(puart_dev->fd < 0)
    {
        LIBHAL_ERROR("open dev %s failed", puart_dev->uart_dev);
        return -1;
    }
    return 0;
}

/* $Function        :   int UartDeviceInit(UartDevCtrl *puart_dev)
==   ===============================================================
==  Description     :   设置串口属性
==  Argument        :   UartDevCtrl 结构体指针
==  Return          :   -1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/

int UartDeviceInit(UartDevCtrl *puart_dev)
{
    struct termios opt;
    if((puart_dev->fd < 0) || (puart_dev->fd == 0))
    {
        LIBHAL_ERROR("please open uart first");
        return -1;
    }
    memset(&opt, 0, sizeof(struct termios));
    tcgetattr(puart_dev->fd, &opt);
    cfmakeraw(&opt);            /* set raw mode */
    switch(puart_dev->uart_attr.baudrate)
    {
        case 50:
            cfsetispeed(&opt, B50);
            cfsetospeed(&opt, B50);
            break;
        case 75:
            cfsetispeed(&opt, B75);
            cfsetospeed(&opt, B75);
            break;
        case 110:
            cfsetispeed(&opt, B110);
            cfsetospeed(&opt, B110);
            break;
        case 134:
            cfsetispeed(&opt, B134);
            cfsetospeed(&opt, B134);
            break;
        case 150:
            cfsetispeed(&opt, B150);
            cfsetospeed(&opt, B150);
            break;
        case 200:
            cfsetispeed(&opt, B200);
            cfsetospeed(&opt, B200);
            break;
        case 300:
            cfsetispeed(&opt, B300);
            cfsetospeed(&opt, B300);
            break;
        case 600:
            cfsetispeed(&opt, B600);
            cfsetospeed(&opt, B600);
            break;
        case 1200:
            cfsetispeed(&opt, B1200);
            cfsetospeed(&opt, B1200);
            break;
        case 1800:
            cfsetispeed(&opt, B1800);
            cfsetospeed(&opt, B1800);
            break;
        case 2400:
            cfsetispeed(&opt, B2400);
            cfsetospeed(&opt, B2400);
            break;
        case 4800:
            cfsetispeed(&opt, B4800);
            cfsetospeed(&opt, B4800);
            break;
        case 9600:
            cfsetispeed(&opt, B9600);
            cfsetospeed(&opt, B9600);
            break;
        case 19200:
            cfsetispeed(&opt, B19200);
            cfsetospeed(&opt, B19200);
            break;
        case 38400:
            cfsetispeed(&opt, B38400);
            cfsetospeed(&opt, B38400);
            break;
        case 57600:
            cfsetispeed(&opt, B57600);
            cfsetospeed(&opt, B57600);
            break;
        case 115200:
            cfsetispeed(&opt, B115200);
            cfsetospeed(&opt, B115200);
            break;
        case 230400:
            cfsetispeed(&opt, B230400);
            cfsetospeed(&opt, B230400);
            break;
        case 460800:
            cfsetispeed(&opt, B460800);
            cfsetospeed(&opt, B460800);
            break;
        case 500000:
            cfsetispeed(&opt, B500000);
            cfsetospeed(&opt, B500000);
            break;
        case 576000:
            cfsetispeed(&opt, B576000);
            cfsetospeed(&opt, B576000);
            break;
        case 921600:
            cfsetispeed(&opt, B921600);
            cfsetospeed(&opt, B921600);
            break;
        case 1000000:
            cfsetispeed(&opt, B1000000);
            cfsetospeed(&opt, B1000000);
            break;
        case 1152000:
            cfsetispeed(&opt, B1152000);
            cfsetospeed(&opt, B1152000);
            break;
        case 1500000:
            cfsetispeed(&opt, B1500000);
            cfsetospeed(&opt, B1500000);
            break;
        case 2000000:
            cfsetispeed(&opt, B2000000);
            cfsetospeed(&opt, B2000000);
            break;
        case 2500000:
            cfsetispeed(&opt, B2500000);
            cfsetospeed(&opt, B2500000);
            break;
        case 3000000:
            cfsetispeed(&opt, B3000000);
            cfsetospeed(&opt, B3000000);
            break;
        case 3500000:
            cfsetispeed(&opt, B3500000);
            cfsetospeed(&opt, B3500000);
            break;
        case 4000000:
            cfsetispeed(&opt, B4000000);
            cfsetospeed(&opt, B4000000);
            break;
        default:
            LIBHAL_ALARM("Unsupported baudrate %d\n", puart_dev->uart_attr.baudrate);
            return -1;
    }
    switch(puart_dev->uart_attr.parity)
    {
        case ComParityNone:         /* none         */
            opt.c_cflag &= ~PARENB; /* disable parity   */
            opt.c_iflag &= ~INPCK;  /* disable parity check */
            break;
        case ComParityOdd:      /* odd          */
            opt.c_cflag |= PARENB;  /* enable parity    */
            opt.c_cflag |= PARODD;  /* odd          */
            opt.c_iflag |= INPCK;   /* enable parity check  */
            break;
        case ComParityEven:     /* even         */
            opt.c_cflag |= PARENB;  /* enable parity    */
            opt.c_cflag &= ~PARODD; /* even         */
            opt.c_iflag |= INPCK;   /* enable parity check  */
        default:
            LIBHAL_ALARM("Unsupported parity %d\n", puart_dev->uart_attr.parity);
            return -1;
    }
    opt.c_cflag &= ~CSIZE;
    switch(puart_dev->uart_attr.bits_data)
    {
        case 5:
            opt.c_cflag |= CS5;
            break;
        case 6:
            opt.c_cflag |= CS6;
            break;
        case 7:
            opt.c_cflag |= CS7;
            break;
        case 8:
            opt.c_cflag |= CS8;
            break;
        default:
            LIBHAL_ALARM("Unsupported data bits %d\n", puart_dev->uart_attr.bits_data);
            return -1;
    }
    opt.c_cflag &= ~CSTOPB;
    switch(puart_dev->uart_attr.bits_stop)
    {
        case ComStopBit1:
            opt.c_cflag &= ~CSTOPB;
            break;
        case ComStopBit2:
            opt.c_cflag |= CSTOPB;
            break;
        default:
            LIBHAL_ALARM("Unsupported stop bits %d\n", puart_dev->uart_attr.bits_stop);
            return -1;
    }
    opt.c_cc[VTIME] = 0;
    opt.c_cc[VMIN]  = 1;            /* block until data arrive */
    tcflush(puart_dev->fd, TCOFLUSH);    //清空发送缓存
    tcflush(puart_dev->fd, TCIFLUSH);    //清空接收缓存
    if(tcsetattr(puart_dev->fd, TCSANOW, &opt) < 0)
    {
        LIBHAL_ERROR("tcsetattr");
        return -1;
    }
    return 0;
}

/* $Function        :   int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length)
==   ===============================================================
==  Description     :   阻塞式读串口直到读到要求的数据长度
==  Argument        :   UartDevCtrl 结构体,buffer 存储读到的数据，length 要求读的长度
==  Return          :   -1： 失败  read_position: 读到长度
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/

int UartDeviceReadBlock(UartDevCtrl uart_dev, void *buffer, int length)
{
    int read_length;
    int read_position = 0;
    if(buffer == NULL || length <= 0 || uart_dev.fd < 0)
    {
        LIBHAL_ERROR("UartDeviceReadBlock : Input params invailed\n");
        return -1;
    }
    while(length)
    {
        read_length = read(uart_dev.fd, buffer + read_position, length);
        if(read_length < 0)
        {
            LIBHAL_ERROR(" readlength < 0\n");
            return -1;
        }
        read_position += read_length;
        length -= read_length;
        usleep(10);
    }
    return read_position;
}

/* $Function        :   int  UartDeviceReadNoblock(UartDevCtrl uart_dev, void *buffer, int length)
==   ===============================================================
==  Description     :   非阻塞式读串口直到读到要求的数据长度，读到时间最长为90ms
==  Argument        :   UartDevCtrl 结构体,buffer 存储读到的数据，length 要求读的长度
==  Return          :   -1： 失败  read_position: 读到长度
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int UartDeviceReadNoblock(UartDevCtrl uart_dev, void *buffer, int length)
{
    int read_length = 0;
    int ret;
    struct timeval time_out;
    fd_set mask;
    int try_times = 3;
    unsigned int total_length = length;
    if(buffer == NULL || length <= 0 || uart_dev.fd < 0)
    {
        LIBHAL_ERROR("UartDeviceRead : Input params invailed\n");
        return -1;
    }
    while(read_length != total_length)
    {
        time_out.tv_sec  = 0;
        time_out.tv_usec = 30000;//wait for (3 * 30)ms
        FD_ZERO(&mask);
        FD_SET(uart_dev.fd, &mask);
        ret = select(uart_dev.fd + 1, &mask, NULL, NULL, &time_out);
        if(ret <= 0)
        {
            if(--try_times == 0)
            {
                return read_length;
            }
            continue;
        }
        ret = read(uart_dev.fd, buffer + read_length, total_length - read_length);
        if(ret < 0)
        {
            LIBHAL_ERROR("readlength < 0\n");
            return -1;
        }
        read_length += ret;
    }
    return read_length;
}

/* $Function        :   int UartDeviceWriteBlock(UartDevCtrl uart_dev, void *buffer, int length)
==   ===============================================================
==  Description     :   阻塞式写串口直到写到要求的数据长度
==  Argument        :   UartDevCtrl 结构体,buffer 存储写的数据，length 要求写的长度
==  Return          :   -1： 失败  write_position: 写到长度
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int UartDeviceWriteBlock(UartDevCtrl uart_dev, void *buffer, int length)
{
    int write_length;
    int write_position = 0;
    if(buffer == NULL || length <= 0 || uart_dev.fd < 0)
    {
        LIBHAL_ERROR("Input params invailed\n");
        return -1;
    }
    while(length)
    {
        write_length = write(uart_dev.fd, buffer + write_position, length);
        if(write_length < 0)
        {
            LIBHAL_ERROR("UartDeviceWriteBlock : writelength < 0,  Length = %d\n", length);
            return -1;
        }
        write_position += write_length;
        length -= write_length;
        usleep(10);
    }
    return write_position;
}

/* $Function        :   int UartDeviceWriteNoblock(UartDevCtrl uart_dev, void *buffer, int length)
==   ===============================================================
==  Description     :   非阻塞式写串口直到写到要求的数据长度
==  Argument        :   UartDevCtrl 结构体,buffer 存储写的数据，length 要求写的长度
==  Return          :   -1： 失败  write_position: 写到长度
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int UartDeviceWriteNoblock(UartDevCtrl uart_dev, void *buffer, int length)
{
    int write_length;
    if(buffer == NULL || length <= 0 || uart_dev.fd < 0)
    {
        LIBHAL_ERROR("Input params invailed\n");
        return -1;
    }
    write_length = write(uart_dev.fd, buffer, length);
    if(write_length < 0)
    {
        LIBHAL_ERROR("UartDeviceWrite : writelength < 0, Length = %d\n", length);
        return -1;
    }
    return write_length;
}

/* $Function        :   int UartDeviceClose(UartDevCtrl *puart_dev)
==   ===============================================================
==  Description     :   关闭串口设备
==  Argument        :  设置串口fd为-1
==  Return          :   -1： 失败  0: 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int UartDeviceClose(UartDevCtrl *puart_dev)
{
    if(puart_dev->uart_index >= UART_DEVICE_MAX || puart_dev->fd < 0)
    {
        LIBHAL_ERROR("uart_dev.uart_index out of range or uart_dev.fd is not valid\n");
        return -1;
    }
    if(puart_dev->uart_index  > 0)
    {
        close(puart_dev->fd);
        puart_dev->fd = -1;
    }
    return 0;
}

/* $Function        :   int UartDeviceClose(UartDevCtrl *puart_dev)
==   ===============================================================
==  Description     :   销毁串口设备
==  Argument        :  设置串口UartDevCtrl 为默认值
==  Return          :   -1： 失败  0: 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int UartDeviceDestory(UartDevCtrl *puart_dev)
{
    if(puart_dev->uart_index >= UART_DEVICE_MAX || puart_dev->uart_index < 0)
    {
        LIBHAL_ERROR("uart_dev.uart_index is not valid\n");
        return -1;
    }
    memset(puart_dev->uart_dev, 0, sizeof(puart_dev->uart_dev));
    memset(&puart_dev->uart_attr, 0, sizeof(UartAttribute));
    puart_dev->uart_index = -1;
    puart_dev->fd = -1;
    return 0;
}


