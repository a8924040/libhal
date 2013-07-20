/******************************************************************************

               Copyright (C), 2001-2100, Tango Co.Ltd.

 ******************************************************************************
  �� �� ��   : system.c
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : 2012��5��20��
  ����޸�   :
  ��������   : ϵͳʱ��,ϵͳ���ܣ�����ϵͳ���ر�ϵͳ��غ���
  �����б�   :
              SystemGetCaps
              SystemGetCPUUsage
              SystemGetCurrentTime
              SystemReboot
              SystemSetCurrentTime
              SystemShutdown
  �޸���ʷ   :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/
#include <mntent.h>
#include "CommonInclude.h"
#include "System.h"
#include "Log.h"
#include "Debug.h"

#define SYSTEM_MAPFILE_PATH "/var/tmp/system_debug.map"
#define SYSTEM_DEBUG_FIFO_PATH "/var/tmp/system_debug_fifo"
#define SYSTEM_LOGFILE_PATH "/var/tmp/sysinit.log"
#define THREAD_INFO_MAX      128

static THREAD_INFO_STRU *gpThreadInfoHead;

static void wait_child_exit(int sig)
{
    int pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        LIBHAL_INFO("%d exit,status %d", pid, status);
    }
}


void SystemInitPlatform(void)
{
    signal(SIGCHLD, wait_child_exit);
}

/*****************************************************************************
�� �� ��  : SystemGetCurrentTime
��������  : ����ϵͳʱ��
�������  : SYSTEM_TIME *pTime
�������  : ��
�� �� ֵ  :
���ú���  :
��������  :

�޸���ʷ      :
 1.��    ��   : 2012��5��20��
   ��    ��   : tango_zhu
   �޸�����   : �����ɺ���

*****************************************************************************/
int SystemGetCurrentTime(SYSTEM_TIME_STRU *pTime)
{
    struct timeval tval;
    struct tm time;
    if(gettimeofday(&tval, NULL))
    {
        LIBHAL_ERROR("get system time fail!\n");
        return -1;
    }
    if(NULL == localtime_r((time_t *) &tval, &time))
    {
        LIBHAL_ERROR("localtime failed\n");
        return -1;
    }
    pTime->second   = time.tm_sec;      /* seconds (1 - 60) */
    pTime->minute   = time.tm_min;          /* minutes (0 - 59) */
    pTime->hour = time.tm_hour;     /* hours (0 - 23)   */
    pTime->wday = time.tm_wday;     /* day of week      */
    pTime->day  = time.tm_mday;     /* day of the month */
    pTime->month    = time.tm_mon + 1;      /* month (0 - 11)   */
    pTime->year = time.tm_year + 1900;  /* year         */
    return 0;
}

/*****************************************************************************
 �� �� ��  : SystemSetCurrentTime
 ��������  : ����ϵͳʱ��
 �������  : SYSTEM_TIME *pTime
 �������  : ��
 �� �� ֵ  : -1:ʧ��
              0:�ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemSetCurrentTime(SYSTEM_TIME_STRU *pTime)
{
    struct tm       TimeNow;
    struct timeval  tval;
    int Moffset[] = { 0, 31, 59, 90, 120, 151,
                      81, 212, 243, 273, 304, 334
                    };
    if(NULL == pTime)
    {
        LIBHAL_ERROR("pTime is NULL!\n");
        return -1;
    }
    memset(&TimeNow, 0, sizeof(struct tm));
    memset(&tval, 0, sizeof(struct timeval));
    TimeNow.tm_sec   = pTime->second;       /* seconds      */
    TimeNow.tm_min   = pTime->minute;       /* minutes      */
    TimeNow.tm_hour = pTime->hour;      /* hours            */
    TimeNow.tm_wday = pTime->wday;      /* day of the week  */
    TimeNow.tm_mday = pTime->day;           /* day of the month */
    TimeNow.tm_mon   = pTime->month - 1;    /* month (0 - 11)   */
    TimeNow.tm_year = pTime->year - 1900;   /* year         */
    TimeNow.tm_isdst = 0;               /* daylight saving time */
    TimeNow.tm_yday  = Moffset[TimeNow.tm_mon] + TimeNow.tm_mday - 1;
    if((tval.tv_sec = mktime(&TimeNow)) < 0)
    {
        LIBHAL_ERROR("mktime failed");
        return -1;
    }
    if(settimeofday(&tval, NULL) < 0)
    {
        LIBHAL_ERROR("settimeofday failed");
        return -1;
    }
    return 0;
}

/*****************************************************************************
 �� �� ��  : SystemGetCPUUsage
 ��������  : ���ϵͳcpuռ����
 �������  : void
 �������  : ��
 �� �� ֵ  : -1 ʧ��
              0 �ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemGetCPUUsage(void)
{
    FILE *fp;
    char line[128];
    unsigned long long u_ticks;
    unsigned long long n_ticks;
    unsigned long long s_ticks;
    unsigned long long i_ticks;
    unsigned long long t_ticks = 0;
    int ret = 0;
    fp = fopen("/proc/stat", "r");
    if(NULL == fp)
    {
        LIBHAL_ERROR("Open /proc/stat Failed\n");
        return -1;
    }
    if(NULL == fgets(line, 128, fp))
    {
        LIBHAL_ERROR("Read /proc/stat Failed\n");
        fclose(fp);
        return -2;
    }
    else
    {
        if(fscanf(fp, "cpu%*d %llu %llu %llu %llu\n",
                  &u_ticks, &n_ticks, &s_ticks, &i_ticks) != 4)
        {
            LIBHAL_ERROR("Fscanf Failed On /proc/stat for cpu\n");
            fclose(fp);
            return -3;
        }
        else
        {
            t_ticks = (u_ticks + s_ticks + i_ticks + n_ticks);
        }
        ret = 100 - (i_ticks) * 100 / t_ticks;
    }
    fclose(fp);
    return ret;
}

/*****************************************************************************
 �� �� ��  : SystemGetCaps
 ��������  : ���ϵͳ������ݰ���ϵͳ���ڴ�ͻ�ʣ�µ��ڴ�
 �������  : SYSTEM_CAPS *pCaps
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��20��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemGetCaps(SYSTEM_CAPS_STRU *pCaps)
{
    struct sysinfo SysInfor;
    if(sysinfo(&SysInfor) < 0)
    {
        LIBHAL_ERROR("SystemGetCaps Failed\n");
        return -1;
    }
    pCaps->MemoryTotal = SysInfor.totalram;
    pCaps->MemoryLeft = SysInfor.freeram;
    return 0;
}
/*****************************************************************************
 �� �� ��  : SystemReboot
 ��������  : ����ϵͳ
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
void SystemReboot(void)
{
}
/*****************************************************************************
 �� �� ��  : SystemShutdown
 ��������  : �ر�ϵͳ
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
void SystemShutdown(void)
{
}

/*****************************************************************************
 �� �� ��  : SystemSetBlankInterVal
 ��������  : �����������̨��ʾʱ��
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��1��
    ��    ��   : tanago_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
S32 SystemSetBlankInterVal(int timeval)
{
    S32 s32FdConsole;
    S8 s8Witebuf[32] = {0};
    s32FdConsole = open("/dev/tty0", O_RDWR);
    if(s32FdConsole < 0)
    {
        LIBHAL_ALARM("open /dev/tty0 failed!");
        return -1;
    }
    sprintf(s8Witebuf, "/033[9;%d]", timeval);
    write(s32FdConsole, s8Witebuf, strlen(s8Witebuf));/* ����Interval ʱ�� */
    close(s32FdConsole);
    return 0;
}
/*****************************************************************************
 �� �� ��  : SystemAddThreadInfo
 ��������  : �洢���̴洢��Ϣ��������Ϣ������
 �������  : THREAD_INFO_STRU *thread_info
             char *p_thread_name
             unsigned int thread_id
 �������  : ��
 �� �� ֵ  :  -1:fail  0:success
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��21��
    ��    ��   : thread_info
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemAddThreadInfo(THREAD_INFO_STRU *thread_info, char *p_thread_name
                        , unsigned int thread_id)
{
    THREAD_INFO_STRU *temp_thread_info = NULL;
    if(NULL == p_thread_name)
    {
        LIBHAL_ALARM("Thread name can't be NULL");
        return -1;
    }
    while(thread_info ->pthread_info_next != NULL)    /* �ҵ�thread_id Ϊ�յĴ洢�ռ� */
    {
        thread_info = thread_info ->pthread_info_next;
    }
    temp_thread_info = (THREAD_INFO_STRU *) malloc(THREAD_INFO_MAX * sizeof(THREAD_INFO_STRU));
    if(NULL == temp_thread_info)
    {
        LIBHAL_WARN("Fail to malloc thread_info memory! error: %d", errno);
        return -1;
    }
    memset(temp_thread_info, 0, THREAD_INFO_MAX * sizeof(THREAD_INFO_STRU));
    strncpy(temp_thread_info->thread_name, p_thread_name, strlen(p_thread_name));
    temp_thread_info->thread_id = thread_id;
    temp_thread_info->pthread_info_next = NULL;
    thread_info ->pthread_info_next = temp_thread_info;
    return 0;
}

/*****************************************************************************
 �� �� ��  : SystemDeleteThreadInfo
 ��������  : ɾ�����̴�
 �������  : THREAD_INFO_STRU *thread_info
             char *p_thread_name
             unsigned int thread_id
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��22��
    ��    ��   : thread_info
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemDeleteThreadInfo(THREAD_INFO_STRU *thread_info, char *p_thread_name
                           , unsigned int thread_id)
{
    THREAD_INFO_STRU *pthread_info = NULL;
    THREAD_INFO_STRU *pthread_info_pri = NULL;
    pthread_info = thread_info;
    int ret = -1;
    if(NULL == p_thread_name)
    {
        LIBHAL_ALARM("Thread name can't be NULL");
        ret =  -1;
        goto out;
    }
    while(pthread_info->thread_id != thread_id && pthread_info != NULL)    /* �ҵ�thread_id Ϊ�յĴ洢�ռ� */
    {
        pthread_info_pri = pthread_info;
        pthread_info = pthread_info->pthread_info_next;
    }
    if(NULL != pthread_info)
    {
        if(NULL != pthread_info->pthread_info_next)    /* �ҵ���λ��Ϊ�м�λ�� */
        {
            pthread_info_pri->pthread_info_next = pthread_info->pthread_info_next;
            free(pthread_info);    //�ͷŽڵ�
        }
        else if(NULL == pthread_info->pthread_info_next)
        {
            pthread_info_pri->pthread_info_next = NULL;
            free(pthread_info);    //�ͷŽڵ�
        }
        ret = 0;
    }
    else
    {
        ret = -1; /*û�ҵ�Ҫ�ҵ�����*/
    }
out:
    return ret;
}
/*****************************************************************************
 �� �� ��  : print_thread_info
 ��������  : ���յ�pritnt_thd_info ����ʱ��ӡ�߳���Ϣ
 �������  : int count
             char *input[]
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��22��
    ��    ��   : thread_info
    �޸�����   : �����ɺ���

*****************************************************************************/
static int print_thread_info(int count, char *input[])
{
    THREAD_INFO_STRU *p_thread_info;
    p_thread_info = gpThreadInfoHead;
    while(p_thread_info != NULL)
    {
        printf("Thread Name: %s\n", p_thread_info->thread_name);
        printf("Thread ID: %d\n", p_thread_info->thread_id);
        p_thread_info = p_thread_info->pthread_info_next;
    }
    return 0;
}


/*****************************************************************************
 �� �� ��  : SystemCheckPartionMounted
 ��������  : ���Ҫ���ص�Ŀ¼�Ƿ��Ѿ�����
 �������  : char *pmount_partion
 �������  : ��
 �� �� ֵ  : 0:���û��mount������mount -1:��ʾ�Ѿ���mount
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��1��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemCheckPartionMounted(char *pmount_partion)
{
    FILE *f;
    struct mntent *mnt;
    if((f = setmntent("/proc/mounts", "r")) == NULL)
    {
        LIBHAL_ALARM("Setmntent Failed");
        return -2;
    }
    while((mnt = getmntent(f)) != NULL)
    {
        if(strcmp(pmount_partion, mnt->mnt_fsname) == 0)
        {
            LIBHAL_ALARM("aleardy contains a mounted node.");
            return -1;
        }
    }
    endmntent(f);
    return 0;
}


/*****************************************************************************
 �� �� ��  : SystemDeviceInit
 ��������  : ��ʼ��ϵͳ����ϵͳ��־��ϵͳ��������
 �������  : THREAD_INFO_STRU *thread_info
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��21��
    ��    ��   : thread_info
    �޸�����   : �����ɺ���

*****************************************************************************/
int SystemDeviceInit(THREAD_INFO_STRU **thread_info)
{
    if(init_system_log(INFO_LOG, INFO_LOG, 0, SYSTEM_LOGFILE_PATH) < 0)
    {
        printf("Fail to Open log file --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
        exit(-1);
    }
    if(init_system_debug((char *) SYSTEM_MAPFILE_PATH, (char *) SYSTEM_DEBUG_FIFO_PATH) < 0)
    {
        printf("Fail to Open map file  --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
        exit(-1);
    }
    if(register_system_debug_cmd("print_thd_info", print_thread_info) < 0)
    {
        printf("Fail to register debug_cmde--print_thd_info --%s(%d)%s\n", __FILE__
               , __LINE__, __FUNCTION__);
        return -1;
    }
    *thread_info = (THREAD_INFO_STRU *) malloc(THREAD_INFO_MAX * sizeof(THREAD_INFO_STRU));
    if(NULL == thread_info)
    {
        LIBHAL_WARN("Fail to malloc thread_info memory! error: %d", errno);
        return -1;
    }
    memset(*thread_info, 0, THREAD_INFO_MAX * sizeof(THREAD_INFO_STRU));
    gpThreadInfoHead = *thread_info;/* ����һ�������ڴ�ӡ������Ϣʱʹ�� */
    gpThreadInfoHead->pthread_info_next = NULL;
    return 0;
}


