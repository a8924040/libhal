/******************************************************************************

               Copyright (C), 2001-2100, Tango Co.Ltd.

 ******************************************************************************
  �� �� ��   : GPS.c
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : 2012��5��20��
  ����޸�   :
  ��������   : gps ���ݽ��մ����ļ�
  �����б�   :
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
  �޸���ʷ   :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

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
 �� �� ��  : GpsDeviceCreate
 ��������  : ����gps�豸
 �������  : void
 �������  : ��
 �� �� ֵ  : int
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : get_dot_position
 ��������  : ������������ж��ŵ�λ��
 �������  : char *pdata          ��������
             unsigned char *pdot  ����λ������ָ��
 �������  : ��
 �� �� ֵ  : static int
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : check_sum_gps
 ��������  : ���gps �����Ƿ�����
 �������  : char *pdata
 �������  : ��
 �� �� ֵ  : -1 ���ݲ���ȷ��0 �ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    strcat(gps_temp, pdata);    //��ϳɷ���gps���ݸ�ʽ������
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
    sscanf(p, "%x", &checksum);    //���gps У����
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
 �� �� ��  : process_gps_gprmc
 ��������  : ����gprmc ����
 �������  : char *pdata    ����
             unsigned char *pdot   ��Ŷ��ŵ�λ��
             GpsOrientData *pgps_orient_data  �ṹ��ָ��
 �������  : ��
 �� �� ֵ  : static int
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /*ֻtime_utc ��0 */
    memset(pgps_orient_data->time_utc, 0, sizeof(pgps_orient_data->time_utc));
    /*����pdot[0]��pdot[1]֮��Ϊtime_utc */
    strncpy(pgps_orient_data->time_utc, pdata + * (pdot + 0) + 1, * (pdot + 1) - * (pdot + 0) - 1);
    /* ֻdata_vail��0 */
    memset(pgps_orient_data->data_vail, 0, sizeof(pgps_orient_data->data_vail));
    /*����pdot[2]��pdot[1]֮��Ϊdata_vail ��λ��Ч�� */
    strncpy(pgps_orient_data->data_vail, pdata + * (pdot + 1) + 1, * (pdot + 2) - * (pdot + 1) - 1);
    /*ֻlatitude��0 */
    memset(pgps_orient_data->latitude, 0, sizeof(pgps_orient_data->latitude));
    /*����pdot[3]��pdot[2]֮��Ϊlatitude */
    strncpy(pgps_orient_data->latitude, pdata + * (pdot + 2) + 1, * (pdot + 3) - * (pdot + 2) - 1);
    /*ֻpos_latitude��0 */
    memset(pgps_orient_data->pos_latitude, 0, sizeof(pgps_orient_data->pos_latitude));
    /*����pdot[4]��pdot[3]֮��Ϊpos_latitude */
    strncpy(pgps_orient_data->pos_latitude, pdata + * (pdot + 3) + 1, * (pdot + 4) - * (pdot + 3) - 1);
    /*ֻlongitud��0 */
    memset(pgps_orient_data->longitude, 0, sizeof(pgps_orient_data->longitude));
    /*����pdot[5]��pdot[4]֮��Ϊlongitud */
    strncpy(pgps_orient_data->longitude, pdata + * (pdot + 4) + 1, * (pdot + 5) - * (pdot + 4) - 1);
    /*ֻpos_latitude��0 */
    memset(pgps_orient_data->pos_longitude, 0, sizeof(pgps_orient_data->pos_longitude));
    /*����pdot[6]��pdot[5]֮��Ϊpos_latitude */
    strncpy(pgps_orient_data->pos_longitude, pdata + * (pdot + 5) + 1, * (pdot + 6) - * (pdot + 5) - 1);
    /*ֻpos_latitude��0 */
    memset(pgps_orient_data->speed, 0, sizeof(pgps_orient_data->speed));
    /*����pdot[7]��pdot[6]֮��Ϊspeed */
    strncpy(pgps_orient_data->speed, pdata + * (pdot + 6) + 1, * (pdot + 7) - * (pdot + 6) - 1);
    /*ֻdirection��0 */
    memset(pgps_orient_data->direction, 0, sizeof(pgps_orient_data->direction));
    /*����pdot[7]��pdot[6]֮��Ϊdirection */
    strncpy(pgps_orient_data->direction, pdata + * (pdot + 7) + 1, * (pdot + 8) - * (pdot + 7) - 1);
    /*ֻpos_latitude��0 */
    memset(pgps_orient_data->date_utc, 0, sizeof(pgps_orient_data->date_utc));
    /*����pdot[7]��pdot[6]֮��Ϊdate_utc */
    strncpy(pgps_orient_data->date_utc, pdata + * (pdot + 8) + 1, * (pdot + 9) - * (pdot + 8) - 1);
    return 0;
}

/*****************************************************************************
 �� �� ��  : process_gps_gpgga
 ��������  : ����gpgga ����
 �������  : char *pdata     ����
             unsigned char *pdot     ��Ŷ��ŵ�λ��
             GpsOrientData *pgps_orient_data  �ṹ��ָ��
 �������  : ��
 �� �� ֵ  : static int
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /*ֻtime_utc ��0 */
    memset(pgps_orient_data->altitude, 0, sizeof(pgps_orient_data->altitude));
    /*����pdot[9]��pdot[8]֮��Ϊaltitude */
    strncpy(pgps_orient_data->altitude, pdata + * (pdot + 8) + 1, * (pdot + 9) - * (pdot + 8) - 1);
    /*ֻ���������Ч�Թ�0 */
    memset(pgps_orient_data->satellite_vail_sum, 0, sizeof(pgps_orient_data->satellite_vail_sum));
    /* ����pdot[9]��pdot[8]֮��Ϊaltitude */
    strncpy(pgps_orient_data->satellite_vail_sum, pdata + * (pdot + 5) + 1, * (pdot + 6) - * (pdot + 5) - 1);
    return 0;
}
/*****************************************************************************
 �� �� ��  : process_gps_data
 ��������  : ����gps����
 �������  : char *pdata  gps ����
 �������  : ��
 �� �� ֵ  : static int
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : gps_data_process_thd
 ��������  : ���gps ���ݲ��Ҵ����߳�
 �������  : void *arg
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
                            /*����ʣ������ݵ�buffer �к���һ�ν������� */
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
 �� �� ��  : GpsDeviceInit
 ��������  :  ��ʼ��gps�豸
 �������  : void
 �������  : ��
 �� �� ֵ  : -1:ʧ�� 0:�ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /*����gps ���ݴ����߳� */
    if(pthread_create(&gps_data_process_tid, NULL, gps_data_process_thd, NULL) != 0)
    {
        LIBHAL_ERROR("Fail to create gps_data_process_thd error: %d", errno);
        return -1;
    }
    return 0;
}

/*****************************************************************************
 �� �� ��  : GpsDeviceRead
 ��������  : ��gps�豸������
 �������  : GpsOrientData *pgps_orient_data
 �������  : ��
 �� �� ֵ  : -1:fail 0:success
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : GpsDeviceWrite
 ��������  : gps �豸��֧��д ֱ�ӷ��سɹ�
 �������  : unsigned char *pbuf
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int GpsDeviceWrite(unsigned char *pbuf)
{
    return 0;
}

/*****************************************************************************
 �� �� ��  : GpsDeviceClose
 ��������  : �ر�gps�豸 �˳�gps�������߳�
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int GpsDeviceClose(void)
{
    flag_gps_thd_exit = 1;//�˳�gps���߳�
    return UartDeviceClose(&uart_dev_gps);
}

/*****************************************************************************
 �� �� ��  : GpsDeviceDestory
 ��������  : ����gps ʧ�� �����豸�˳�
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int GpsDeviceDestory(void)
{
    flag_gps_init = 0;
    return UartDeviceDestory(&uart_dev_gps);
}
