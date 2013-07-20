/******************************************************************************

               Copyright (C), 2001-2100, Tango Co.Ltd.

 ******************************************************************************
  文 件 名   : GPS.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  生成日期   : 2012年5月20日
  最近修改   :
  功能描述   : gps 数据接收处理文件
  函数列表   :
              check_sum_gps
              get_dot_position
              GpsDeviceClose
              GpsDeviceCreate
              GpsDeviceDestory
              GpsDeviceInit
              GpsDeviceRead
              GpsDeviceWrite
              gps_data_process_thd
              process_gps_data
              process_gps_gpgga
              process_gps_gprmc
  修改历史   :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/
#include "CommonInclude.h"
#include "Uart.h"
#include "Log.h"
#include "GPS.h"

#define GPS_DATA_BUFFER_MAX   1024
#define GPS_DATA_READ_MAX     512
UartDevCtrl uart_dev_gps;
int flag_gps_thd_exit = 0;
int flag_gps_init = 0;
pthread_mutex_t g_gps_data_mutex;
GpsOrientData g_gps_orient_data;

/*****************************************************************************
 函 数 名  : GpsDeviceCreate
 功能描述  : 创建gps设备
 输入参数  : void
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GpsDeviceCreate(void)
{
    memset(&uart_dev_gps.uart_attr, 0, sizeof(UartAttribute));
    uart_dev_gps.uart_index = UartDeviceGps;
    if(flag_gps_init  == 0)
    {
        if(UartDeviceCreate(&uart_dev_gps) < 0)
        {
            LIBHAL_ERROR("creat UartDeviceGps failed");
            return -1;
        }
        flag_gps_init  = 1;
    }
    else
    {
        LIBHAL_ALARM("UartDeviceGps already created");
        return -2;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : get_dot_position
 功能描述  : 获得收入数据中逗号的位置
 输入参数  : char *pdata          输入数据
             unsigned char *pdot  逗号位置数组指针
 输出参数  : 无
 返 回 值  : static int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int get_dot_position(char *pdata, unsigned char *pdot)
{
    unsigned int len = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    if(pdata == NULL || pdot ==  NULL)
    {
        LIBHAL_ERROR("data can't be NULL");
        return -1;
    }
    len = strlen(pdata);
    for(i = 0; i < len; i++)
    {
        if(* (pdata + i) == ',')
        {
            * (pdot + j) = i;
            j++;
        }
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : check_sum_gps
 功能描述  : 检测gps 数据是否完整
 输入参数  : char *pdata
 输出参数  : 无
 返 回 值  : -1 数据不正确，0 成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int check_sum_gps(char *pdata)
{
    int checksum;
    char gps_temp[256];
    char temp;
    char *p;
    char *ptr;
    memset(gps_temp, 0, sizeof(gps_temp));
    gps_temp[0] = '$';
    strcat(gps_temp, pdata);    //组合成符合gps数据格式的数据
    if(NULL == (p = strstr(gps_temp, "*")))
    {
        LIBHAL_ALARM("the gps data is not valid");
        return -1;
    }
    p++;
    if(p == NULL)
    {
        return -1;
    }
    sscanf(p, "%x", &checksum);    //获得gps 校验码
    ptr = gps_temp;
    temp = *gps_temp;
    while(*ptr != '*')
    {
        temp ^= *ptr;
        ptr++;
    }
    if(checksum == temp)
    {
        return 0;
    }
    LIBHAL_ERROR("check gps data sum failed");
    return -1;
}
/*****************************************************************************
 函 数 名  : process_gps_gprmc
 功能描述  : 处理gprmc 数据
 输入参数  : char *pdata    数据
             unsigned char *pdot   存放逗号的位置
             GpsOrientData *pgps_orient_data  结构体指针
 输出参数  : 无
 返 回 值  : static int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int process_gps_gprmc(char *pdata, unsigned char *pdot, GpsOrientData *pgps_orient_data)
{
    if(NULL == pdata || NULL == pdot || NULL == pgps_orient_data)
    {
        LIBHAL_ERROR("the grmc data can't be null");
        return -1;
    }
    if(check_sum_gps(pdata) < 0)
    {
        LIBHAL_ERROR("check gps data failed");
        return -1;
    }
    if((strstr(pdata, "*")  == NULL) || (* (pdot + 9) == 0))
    {
        LIBHAL_ALARM("the gprmc data is not valid");
        return -1;
    }
    /*只time_utc 归0 */
    memset(pgps_orient_data->time_utc, 0, sizeof(pgps_orient_data->time_utc));
    /*数组pdot[0]和pdot[1]之间为time_utc */
    strncpy(pgps_orient_data->time_utc, pdata + * (pdot + 0) + 1, * (pdot + 1) - * (pdot + 0) - 1);
    /* 只data_vail归0 */
    memset(pgps_orient_data->data_vail, 0, sizeof(pgps_orient_data->data_vail));
    /*数组pdot[2]和pdot[1]之间为data_vail 定位有效性 */
    strncpy(pgps_orient_data->data_vail, pdata + * (pdot + 1) + 1, * (pdot + 2) - * (pdot + 1) - 1);
    /*只latitude归0 */
    memset(pgps_orient_data->latitude, 0, sizeof(pgps_orient_data->latitude));
    /*数组pdot[3]和pdot[2]之间为latitude */
    strncpy(pgps_orient_data->latitude, pdata + * (pdot + 2) + 1, * (pdot + 3) - * (pdot + 2) - 1);
    /*只pos_latitude归0 */
    memset(pgps_orient_data->pos_latitude, 0, sizeof(pgps_orient_data->pos_latitude));
    /*数组pdot[4]和pdot[3]之间为pos_latitude */
    strncpy(pgps_orient_data->pos_latitude, pdata + * (pdot + 3) + 1, * (pdot + 4) - * (pdot + 3) - 1);
    /*只longitud归0 */
    memset(pgps_orient_data->longitude, 0, sizeof(pgps_orient_data->longitude));
    /*数组pdot[5]和pdot[4]之间为longitud */
    strncpy(pgps_orient_data->longitude, pdata + * (pdot + 4) + 1, * (pdot + 5) - * (pdot + 4) - 1);
    /*只pos_latitude归0 */
    memset(pgps_orient_data->pos_longitude, 0, sizeof(pgps_orient_data->pos_longitude));
    /*数组pdot[6]和pdot[5]之间为pos_latitude */
    strncpy(pgps_orient_data->pos_longitude, pdata + * (pdot + 5) + 1, * (pdot + 6) - * (pdot + 5) - 1);
    /*只pos_latitude归0 */
    memset(pgps_orient_data->speed, 0, sizeof(pgps_orient_data->speed));
    /*数组pdot[7]和pdot[6]之间为speed */
    strncpy(pgps_orient_data->speed, pdata + * (pdot + 6) + 1, * (pdot + 7) - * (pdot + 6) - 1);
    /*只direction归0 */
    memset(pgps_orient_data->direction, 0, sizeof(pgps_orient_data->direction));
    /*数组pdot[7]和pdot[6]之间为direction */
    strncpy(pgps_orient_data->direction, pdata + * (pdot + 7) + 1, * (pdot + 8) - * (pdot + 7) - 1);
    /*只pos_latitude归0 */
    memset(pgps_orient_data->date_utc, 0, sizeof(pgps_orient_data->date_utc));
    /*数组pdot[7]和pdot[6]之间为date_utc */
    strncpy(pgps_orient_data->date_utc, pdata + * (pdot + 8) + 1, * (pdot + 9) - * (pdot + 8) - 1);
    return 0;
}

/*****************************************************************************
 函 数 名  : process_gps_gpgga
 功能描述  : 处理gpgga 数据
 输入参数  : char *pdata     数据
             unsigned char *pdot     存放逗号的位置
             GpsOrientData *pgps_orient_data  结构体指针
 输出参数  : 无
 返 回 值  : static int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int process_gps_gpgga(char *pdata, unsigned char *pdot, GpsOrientData *pgps_orient_data)
{
    if(NULL == pdata || NULL == pdot || NULL == pgps_orient_data)
    {
        LIBHAL_ERROR("the gpgga data can't be null");
        return -1;
    }
    if(check_sum_gps(pdata) < 0)
    {
        LIBHAL_ERROR("check gps data failed");
        return -1;
    }
    if((strstr(pdata, "*")  == NULL) || (* (pdot + 9) == 0))
    {
        LIBHAL_ALARM("the gpgga data is not valid");
        return -1;
    }
    /*只time_utc 归0 */
    memset(pgps_orient_data->altitude, 0, sizeof(pgps_orient_data->altitude));
    /*数组pdot[9]和pdot[8]之间为altitude */
    strncpy(pgps_orient_data->altitude, pdata + * (pdot + 8) + 1, * (pdot + 9) - * (pdot + 8) - 1);
    /*只检测数据有效性归0 */
    memset(pgps_orient_data->satellite_vail_sum, 0, sizeof(pgps_orient_data->satellite_vail_sum));
    /* 数组pdot[9]和pdot[8]之间为altitude */
    strncpy(pgps_orient_data->satellite_vail_sum, pdata + * (pdot + 5) + 1, * (pdot + 6) - * (pdot + 5) - 1);
    return 0;
}
/*****************************************************************************
 函 数 名  : process_gps_data
 功能描述  : 处理gps数据
 输入参数  : char *pdata  gps 数据
 输出参数  : 无
 返 回 值  : static int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int process_gps_data(char *pdata)
{
    unsigned char pdot[128] = {0};
    int ret = 0;
    if(pdata == NULL)
    {
        LIBHAL_ALARM("the data is null");
        return  -1;
    }
    if(strstr(pdata, "GPRMC") != NULL)
    {
        memset(pdot, 0, sizeof(pdot));
        get_dot_position(pdata, pdot);
        pthread_mutex_lock(&g_gps_data_mutex);
        ret = process_gps_gprmc(pdata, pdot, &g_gps_orient_data);
        pthread_mutex_unlock(&g_gps_data_mutex);
    }
    if(strstr(pdata, "GPGGA") != NULL)
    {
        memset(pdot, 0, sizeof(pdot));
        get_dot_position(pdata, pdot);
        pthread_mutex_lock(&g_gps_data_mutex);
        ret = process_gps_gpgga(pdata, pdot, &g_gps_orient_data);
        pthread_mutex_unlock(&g_gps_data_mutex);
    }
    return ret;
}
/*****************************************************************************
 函 数 名  : gps_data_process_thd
 功能描述  : 获得gps 数据并且处理线程
 输入参数  : void *arg
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static void *gps_data_process_thd(void *arg)
{
    char *ptrgps;
    char *ptr_tmp;
    char buffer_remain[2 * GPS_DATA_BUFFER_MAX ] = { 0 };
    char buffer[GPS_DATA_BUFFER_MAX ] = { 0 };
    char *ptr_gps_tmp = NULL;
    char *ptr1 = NULL;
    int read_len = 0;
    int flag_head = 0;
    pthread_detach(pthread_self());
    while(1)
    {
        if(flag_gps_thd_exit == 0)
        {
            read_len = UartDeviceReadBlock(uart_dev_gps, buffer, GPS_DATA_READ_MAX);
            buffer[read_len] = '\0';
            ptr_tmp = buffer;
            if(buffer_remain[0] != 0)
            {
                strcat(buffer_remain, ptr_tmp);
                ptr_tmp = buffer_remain;
            }
            while((ptrgps = strtok(ptr_tmp, "$")) != NULL)
            {
                ptr_tmp = NULL;
                ptr_gps_tmp = ptrgps;
                if((flag_head == 0) && (strstr(ptr_gps_tmp, "GP") != NULL))
                {
                    flag_head = 1;
                }
                if(flag_head == 1)
                {
                    if(((ptr1 = strstr(ptr_gps_tmp, "GP")) != NULL))
                    {
                        if(strstr(ptr1, "\n") != NULL)
                        {
                            LIBHAL_DEBUG("strtok %s", ptr1);
                            //printf("befor process data the data is %s",ptr1);
                            process_gps_data(ptr1);
                        }
                        else
                        {
                            /*拷贝剩余的数据到buffer 中和下一次接受数据 */
                            sprintf(buffer_remain, "%s", ptr1);
                        }
                    }
                }
            }
            memset(buffer, 0, sizeof(buffer));
            sleep(1);
        }
        else
        {
            break;
        }
    }
    pthread_exit(NULL);
}
/*****************************************************************************
 函 数 名  : GpsDeviceInit
 功能描述  :  初始化gps设备
 输入参数  : void
 输出参数  : 无
 返 回 值  : -1:失败 0:成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GpsDeviceInit(void)
{
    pthread_t gps_data_process_tid;
    flag_gps_thd_exit = 0;
    memset(&g_gps_orient_data, 0, sizeof(GpsOrientData));
    if(UartDeviceOpen(&uart_dev_gps) < 0)
    {
        LIBHAL_ERROR("open UartDeviceGps failed");
    }
    uart_dev_gps.uart_attr.baudrate = 9600;
    uart_dev_gps.uart_attr.bits_data = ComDataBit8;
    uart_dev_gps.uart_attr.bits_stop = ComStopBit1;
    uart_dev_gps.uart_attr.parity = ComParityNone;
    if(UartDeviceInit(&uart_dev_gps) < 0)
    {
        LIBHAL_ERROR("open UartDeviceGps failed");
    }
    pthread_mutex_init(&g_gps_data_mutex, NULL);
    /*创建gps 数据处理线程 */
    if(pthread_create(&gps_data_process_tid, NULL, gps_data_process_thd, NULL) != 0)
    {
        LIBHAL_ERROR("Fail to create gps_data_process_thd error: %d", errno);
        return -1;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : GpsDeviceRead
 功能描述  : 从gps设备读数据
 输入参数  : GpsOrientData *pgps_orient_data
 输出参数  : 无
 返 回 值  : -1:fail 0:success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GpsDeviceRead(GpsOrientData *pgps_orient_data)
{
    if(flag_gps_init != 1)
    {
        LIBHAL_ERROR("need to open gps devices first");
        return -1;
    }
    memset(pgps_orient_data, 0, sizeof(GpsOrientData));
    pthread_mutex_lock(&g_gps_data_mutex);
    memcpy(pgps_orient_data, &g_gps_orient_data, sizeof(GpsOrientData));
    pthread_mutex_unlock(&g_gps_data_mutex);
    return 0;
}

/*****************************************************************************
 函 数 名  : GpsDeviceWrite
 功能描述  : gps 设备不支持写 直接返回成功
 输入参数  : unsigned char *pbuf
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GpsDeviceWrite(unsigned char *pbuf)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : GpsDeviceClose
 功能描述  : 关闭gps设备 退出gps读数据线程
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GpsDeviceClose(void)
{
    flag_gps_thd_exit = 1;//退出gps读线程
    return UartDeviceClose(&uart_dev_gps);
}

/*****************************************************************************
 函 数 名  : GpsDeviceDestory
 功能描述  : 销毁gps 失败 整个设备退出
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GpsDeviceDestory(void)
{
    flag_gps_init = 0;
    return UartDeviceDestory(&uart_dev_gps);
}
