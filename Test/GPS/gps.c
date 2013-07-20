#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "Log.h"
#include "GPS.h"
#include <time.h>
#include "System.h"

#define LOG_FILE_PATH        "/var/lib3glog"                  //3g接口库LOG文件路径
#define MAP_FILE_PATH "/nfs/net3g.map"   //进程map文件默认路径
#define DEBUG_FIFO_PATH "/var/tmp/net3g_fifo"   //调试命令输入fifo文件路径
#define DESTZONE    "TZ=Asia/Shanghai" /** our time zone **/

int  main()
{
    GpsOrientData gps_orient_data;
    SYSTEM_TIME_STRU time_gps;
    int settime_flag = 0;
    printf("********->the program compiled at %s,%s<-********\n", __DATE__, __TIME__);
    memset(&gps_orient_data, 0, sizeof(GpsOrientData));
    if(init_system_log(WARNING_LOG, ERROR_LOG, 0, NULL) < 0)
    {
        printf("Fail to Open log file  --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
        exit(-1);
    }
    GpsDeviceCreate();
    GpsDeviceInit();
    while(1)
    {
        memset(&gps_orient_data, 0, sizeof(GpsOrientData));
        GpsDeviceRead(&gps_orient_data);
        LIBHAL_ALARM("latitude = %s,longitude = %s,altitude = %s"
                     "date_utc = %s,time_utc = %s ", gps_orient_data.latitude
                     , gps_orient_data.longitude, gps_orient_data.altitude
                     , gps_orient_data.date_utc, gps_orient_data.time_utc);
        if(strlen(gps_orient_data.time_utc) > 0)
        {
            sscanf(gps_orient_data.time_utc, "%2d%2d%2d", &time_gps.hour,
                   &time_gps.minute, &time_gps.second);
            time_gps.hour = (time_gps.hour + 8) % 24;
            LIBHAL_ALARM("time_gps.tm_hour=%d,time_gps.tm_min=%d,time_gps.tm_sec=%d"
                         , time_gps.hour, time_gps.minute, time_gps.second);
        }
        if(strlen(gps_orient_data.date_utc) > 0)
        {
            sscanf(gps_orient_data.date_utc, "%2d%2d%2d", &time_gps.day,
                   &time_gps.month, &time_gps.year);
            time_gps.year += 2000;
            if(settime_flag == 0)
            {
                SystemSetCurrentTime(&time_gps);
                settime_flag = 1;
            }
        }
        sleep(1);
    }
    return 0;
}
