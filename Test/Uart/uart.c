#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "Log.h"
#include "GPS.h"
#include <time.h>
#include "System.h"
#include "Uart.h"

int  main()
{
    UartDevCtrl uart_dev;
    unsigned char buf = 0xa2;
    unsigned char buffer[10] ={0};
    uart_dev.uart_index = UartDeviceIr;
    UartDeviceCreate(&uart_dev);
    UartDeviceOpen(&uart_dev);
    while(1)
    {
        UartDeviceWriteBlock(uart_dev, &buf, 1);
        UartDeviceReadBlock(uart_dev, buffer, 10);
        printf("receive buffer is %s\n",buffer);
        memset(buffer,0,10);
        usleep(500000);
    }
    return 0;
}
