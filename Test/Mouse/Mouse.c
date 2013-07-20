#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "Log.h"
#include "GPS.h"
#include <time.h>
#include "System.h"
#include "Mouse.h"
#include "Touchscreen.h"

static int touch_adjdot(void)
{
    printf("touch  INDEX_LU point\n");
    TouchScreenAdjDot(20, 20, INDEX_LU);
    printf("touch  INDEX_LD point\n");
    sleep(0.5);
    TouchScreenAdjDot(20, 800, INDEX_LD);
    printf("touch  INDEX_RD point\n");
    sleep(0.5);
    TouchScreenAdjDot(1204, 800, INDEX_RD);
    printf("touch  INDEX_M point\n");
    sleep(0.5);
    TouchScreenAdjDot(602, 400, INDEX_M);
    printf("end .....\n");
    return 0;
}


static void *get_mouse_data_thd(void *arg)
{
    MOUSE_DATA data;
    memset(&data, 0, sizeof(MOUSE_DATA));
    while(1)
    {
        if(MouseGetData(&data) < 0)
        {
            sleep(1);
        }
        else
        {
            printf("key:%u x:%d y:%d z:%d ax:%x ay:%x\n", \
                   data.key, data.x, data.y, \
                   data.z, data.ax, data.ay);
            if(data.key == 0x2)
            {
                printf("TouchScreen get Esc signal\n");
                TouchScreenAdjEsc();
            }
            sleep(1);
        }
    }
}


int  main()
{
    pthread_t get_mouse_data_tid;
    printf("*****->the program compiled at %s,%s<-*****\n", __DATE__, __TIME__);
    MouseCreate();
    if(pthread_create(&get_mouse_data_tid, NULL, get_mouse_data_thd, NULL) != 0)
    {
        printf("Fail to start wireless watch thread! error: %d", errno);
        return -1;
    }
    touch_adjdot();
    printf("********->calicate touchscreen finished********\n");
    pthread_join(get_mouse_data_tid, NULL);
    return 0;
}
