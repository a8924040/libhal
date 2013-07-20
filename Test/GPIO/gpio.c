#include <stdio.h>
#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>

#include "IniFile.h"
#include "Log.h"
#include "GPIO.h"

int  main()
{
    WRITE_GPIO_DATA gpio_data;
    gpio_data.port = 'B';
    gpio_data.subpin = 0;
    gpio_data.val = 1;
    if(init_system_log(INFO_LOG, ERROR_LOG, 0, NULL) < 0)
    {
        printf("Fail to Open log file  --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
        exit(-1);
    }
    if(GpioDeviceCreate() < 0)
    {
        LIBHAL_ALARM("creat gpio devices failed");
    }
    if(GpioDeviceWrite(&gpio_data) == 0)
    {
        LIBHAL_ALARM("write gpio devices succcess");
    }
    GpioDeviceDestory();
    return 0;
}
