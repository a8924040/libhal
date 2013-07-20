#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

    typedef struct SYSTEM_TIME /* 系统时间结构体 */
    {
        int second;     /* seconds (1 - 60) */
        int minute;     /* minutes (0 - 59) */
        int hour;           /* hours (0 - 23)   */
        int wday;           /* day of week      */
        int day;            /* day of the month */
        int month;      /* month (0 - 11)   */
        int year;      /* year          */
    } SYSTEM_TIME_STRU;

    typedef struct SYSTEM_CAPS /* 系统性能结构体 */
    {

        unsigned long MemoryTotal;    //Total usable main memory size
        unsigned long MemoryLeft;     //Available memory size

    } SYSTEM_CAPS_STRU;

    typedef struct THREAD_INFO /* 创建的线程名和线程id */
    {
        char thread_name[32];
        unsigned int thread_id;
        struct THREAD_INFO *pthread_info_next;
    } THREAD_INFO_STRU;


    /*****************************************************************************
    函 数 名  : SystemGetCurrentTime
    功能描述  : 设置系统时间
    输入参数  : SYSTEM_TIME *pTime
    输出参数  : 无
    返 回 值  : -1:失败
                 0:成功

    修改历史      :
     1.日    期   : 2012年5月20日
       作    者   : tango_zhu
       修改内容   : 新生成函数

    *****************************************************************************/
    int SystemGetCurrentTime(SYSTEM_TIME_STRU *pTime);

    /*****************************************************************************
     函 数 名  : SystemSetCurrentTime
     功能描述  : 设置系统时间
     输入参数  : SYSTEM_TIME *pTime
     输出参数  : 无
     返 回 值  : -1:失败
                  0:成功

     修改历史      :
      1.日    期   : 2012年5月20日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/

    int SystemSetCurrentTime(SYSTEM_TIME_STRU *pTime);
    /*****************************************************************************
     函 数 名  : SystemGetCPUUsage
     功能描述  : 获得系统cpu占用率
     输入参数  : void
     输出参数  : 无
     返 回 值  : -1 失败
                  0 成功

     修改历史      :
      1.日    期   : 2012年5月20日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int SystemGetCPUUsage(void);
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
    int SystemDeviceInit(THREAD_INFO_STRU **thread_info);
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
                               , unsigned int thread_id);
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
                            , unsigned int thread_id);

    /*****************************************************************************
     函 数 名  : SystemCheckPartionMounted
     功能描述  : 检查要挂载的目录是否已经挂载
     输入参数  : char *pmount_partion
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年6月1日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int SystemCheckPartionMounted(char *pmount_partion);

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
    int SystemSetBlankInterVal(int timeval);


    /*****************************************************************************
     函 数 名  : SystemInitPlatform
     功能描述  : 系统初始化函数
     输入参数  : void
     输出参数  : 无
     返 回 值  : void
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年11月19日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    void SystemInitPlatform(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

