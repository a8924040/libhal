#include <sys/stat.h>
#include <unistd.h>
#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "Log.h"
#include "usbinfo.h"
int  main()
{
    USB_DEV_DESC desc[4];
    unsigned int count = 0;
    int i, j;
    InitUsbInfo();
    GetUsbDevInfo(desc, &count);
    printf("find dev count = %d\n", count);
    for(i = 0; i < count; i++)
    {
        printf("xxxxxxxxxxxxxxxxxxxxx USB DEVICE NO:%dxxxxxxxxxxxxxxxxxxxx\n", i);
        printf("manufacturer = %s, product = %s,vendor = %x,product_id = %x,"
               "subdev_count =  %d, inerface_index = %d\n"
               , desc[i].manufacturer, desc[i].product, desc[i].vendor
               , desc[i].product_id , desc[i].subdev_count, desc[i].inerface_index);
        for(j = 0; j < desc[i].subdev_count; j++)
        {
            printf("name = %s, driver_type = %d \n"
                   , desc[i].subdev[j].name, desc[i].subdev[j].driver_type);
        }
        printf("bus = %d, level = %d, port = %d,parent = %d\n"
               , desc[i].usb_dev_info.bus, desc[i].usb_dev_info.lev
               , desc[i].usb_dev_info.port, desc[i].usb_dev_info.prnt);
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }
    return 0;
}



