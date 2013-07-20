#include <stdio.h>
#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>

#include "IniFile.h"
#include "Log.h"
#include "GPIO_IIC.h"

int  main()
{
    GPIO_IIC_CMD_DATA gpio_iic_data;
    int i_read = 0;
    int i_write = 0;
    unsigned char i = 0;
    memset(&gpio_iic_data, 0, sizeof(GPIO_IIC_CMD_DATA));
    gpio_iic_data.slave_address = 0xa0;
    gpio_iic_data.len = 32;
    gpio_iic_data.sub_mod = 1;
    gpio_iic_data.sub_address = 0x00;
    if(init_system_log(INFO_LOG, ERROR_LOG, 0, NULL) < 0)
    {
        printf("Fail to Open log file  --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
        exit(-1);
    }
    if(GpioI2CDeviceCreate() < 0)
    {
        LIBHAL_ALARM("creat gpio devices failed");
    }
    LIBHAL_DEBUG("begin to write gpio iic\n");
    for(i_write = 0; i_write < 16; i_write++)
    {
        gpio_iic_data.data[i_write] = i_write;
    }
    for(i_write = 0; i_write < 8; i_write++)
    {
        for(i = 0; i < 16; i++)
        {
            gpio_iic_data.data[i] = i_write * 16 + i;
        }
        gpio_iic_data.len = 16;
        if(i_write == 0)
        {
            gpio_iic_data.sub_address = 0;
        }
        else
        {
            gpio_iic_data.sub_address += 16;
        }
        if(GpioI2CDeviceWrite(&gpio_iic_data) == 0)
        {
            LIBHAL_ALARM("write gpio iic devices succcess");
        }
        usleep(10);
    }
    gpio_iic_data.len = 128;
    gpio_iic_data.sub_address = 0;
    memset(gpio_iic_data.data, 0, 128);
    if(GpioI2CDeviceRead(&gpio_iic_data) == 0)
    {
        LIBHAL_ALARM("read gpio iic devices succcess");
    }
    for(i_read = 0; i_read < gpio_iic_data.len; i_read ++)
    {
        printf("%4x", gpio_iic_data.data[i_read]);
        if((i_read + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
    GpioI2CDeviceDestory();
    return 0;
}
