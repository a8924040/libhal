/******************************************************************************

               Copyright (C), 2001-2100, Tango Co.Ltd.

 ******************************************************************************
  文 件 名   : system.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  生成日期   : 2012年5月20日
  最近修改   :
  功能描述   : 系统时间,系统性能，重启系统，关闭系统相关函数
  函数列表   :
              SystemGetCaps
              SystemGetCPUUsage
              SystemGetCurrentTime
              SystemReboot
              SystemSetCurrentTime
              SystemShutdown
  修改历史   :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 创建文件

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
函 数 名  : SystemGetCurrentTime
功能描述  : 设置系统时间
输入参数  : SYSTEM_TIME *pTime
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年5月20日
   作    者   : tango_zhu
   修改内容   : 新生成函数

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
 函 数 名  : SystemSetCurrentTime
 功能描述  : 设置系统时间
 输入参数  : SYSTEM_TIME *pTime
 输出参数  : 无
 返 回 值  : -1:失败
              0:成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : SystemGetCPUUsage
 功能描述  : 获得系统cpu占用率
 输入参数  : void
 输出参数  : 无
 返 回 值  : -1 失败
              0 成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : SystemGetCaps
 功能描述  : 获得系统相关数据包括系统总内存和还剩下的内存
 输入参数  : SYSTEM_CAPS *pCaps
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月20日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : SystemReboot
 功能描述  : 重启系统
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
void SystemReboot(void)
{
}
/*****************************************************************************
 函 数 名  : SystemShutdown
 功能描述  : 关闭系统
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
void SystemShutdown(void)
{
}

/*****************************************************************************
 函 数 名  : SystemSetBlankInterVal
 功能描述  : 设置虚拟控制台显示时间
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月1日
    作    者   : tanago_zhu
    修改内容   : 新生成函数

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
    write(s32FdConsole, s8Witebuf, strlen(s8Witebuf));/* 设置Interval 时间 */
    close(s32FdConsole);
    return 0;
}
/*****************************************************************************
 函 数 名  : SystemAddThreadInfo
 功能描述  : 存储进程存储信息到进程信息缓存中
 输入参数  : THREAD_INFO_STRU *thread_info
             char *p_thread_name
             unsigned int thread_id
 输出参数  : 无
 返 回 值  :  -1:fail  0:success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月21日
    作    者   : thread_info
    修改内容   : 新生成函数

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
    while(thread_info ->pthread_info_next != NULL)    /* 找到thread_id 为空的存储空间 */
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
 函 数 名  : SystemDeleteThreadInfo
 功能描述  : 删除进程存
 输入参数  : THREAD_INFO_STRU *thread_info
             char *p_thread_name
             unsigned int thread_id
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月22日
    作    者   : thread_info
    修改内容   : 新生成函数

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
    while(pthread_info->thread_id != thread_id && pthread_info != NULL)    /* 找到thread_id 为空的存储空间 */
    {
        pthread_info_pri = pthread_info;
        pthread_info = pthread_info->pthread_info_next;
    }
    if(NULL != pthread_info)
    {
        if(NULL != pthread_info->pthread_info_next)    /* 找到的位置为中间位置 */
        {
            pthread_info_pri->pthread_info_next = pthread_info->pthread_info_next;
            free(pthread_info);    //释放节点
        }
        else if(NULL == pthread_info->pthread_info_next)
        {
            pthread_info_pri->pthread_info_next = NULL;
            free(pthread_info);    //释放节点
        }
        ret = 0;
    }
    else
    {
        ret = -1; /*没找到要找的数据*/
    }
out:
    return ret;
}
/*****************************************************************************
 函 数 名  : print_thread_info
 功能描述  : 当收到pritnt_thd_info 命令时打印线程信息
 输入参数  : int count
             char *input[]
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月22日
    作    者   : thread_info
    修改内容   : 新生成函数

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
 函 数 名  : SystemCheckPartionMounted
 功能描述  : 检查要挂载的目录是否已经挂载
 输入参数  : char *pmount_partion
 输出参数  : 无
 返 回 值  : 0:检测没有mount过可以mount -1:表示已经被mount
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月1日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : SystemDeviceInit
 功能描述  : 初始化系统包括系统日志和系统调试命令
 输入参数  : THREAD_INFO_STRU *thread_info
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月21日
    作    者   : thread_info
    修改内容   : 新生成函数

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
    gpThreadInfoHead = *thread_info;/* 保存一个副本在打印进程信息时使用 */
    gpThreadInfoHead->pthread_info_next = NULL;
    return 0;
}


