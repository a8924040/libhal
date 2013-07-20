#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>

#include "Log.h"
#include "IR.h"
#include "GPIO.h"
unsigned char read_buf[4] = { 0 };
int main(int argc, char *argv[])
{
    unsigned char read_buf[4] = {0};
    WRITE_GPIO_DATA gpio_data;
    gpio_data.port = 'B';
    gpio_data.subpin = 0;
    gpio_data.val = 1;
    printf("ir_test Complie Date:%s Time:%s\n", __DATE__, __TIME__);
    if(init_system_log(DEBUG_LOG, ERROR_LOG, 0, NULL) < 0)
    {
        printf("netinit init log failed \n");
        exit(-1);
    }
    if(IrDeviceCreate() < 0)
    {
        LIBHAL_ERROR("creat ir device failed");
        return -1;
    }
    if(GpioDeviceCreate() < 0)
    {
        LIBHAL_ERROR("creat gpio devices failed");
    }
    if(IrDeviceInit() < 0)
    {
        LIBHAL_ERROR("init ir device failed");
        exit(-1);
    }
    while(1)
    {
        IrDeviceRead(read_buf);
        LIBHAL_DEBUG("read result is 0x%x,0x%x,0x%x,0x%x", read_buf[0], read_buf[1], read_buf[2], read_buf[3]);
        if(read_buf[3] == 0x67)
        {
            gpio_data.val = 1;
            LIBHAL_ALARM("turn on the beep");
            if(GpioDeviceWrite(&gpio_data) < 0)
            {
                LIBHAL_ALARM("write gpio devices failed");
            }
        }
        if(read_buf[3] == 0x27)
        {
            gpio_data.val = 0;
            LIBHAL_ALARM("turn off the beep");
            if(GpioDeviceWrite(&gpio_data) < 0)
            {
                LIBHAL_ALARM("write gpio devices failed");
            }
        }
        uint_msleep(500);
    }
    return 0;
}

