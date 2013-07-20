/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.h
**  Description   : 日志头文件
**  Modification  : 2011/7/19    huang_zhendi    创建
**  ************************************************************************
*/
#ifndef __LOG_H__
#define __LOG_H__
#include "CommonDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    /*
    **  ************************************************************************
    **                             宏定义
    **  ************************************************************************
    */
#define __LIBHAL_DEBUG__

#define LIBHAL_PRINTF(fmt, ...)     printf(fmt "--%s(%d)%s\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)

    /*输出LOG等级高于当前LOG等级的信息  */
#ifdef __LIBHAL_DEBUG__
#define LIBHAL_LOG                  printf_log
#define LIBHAL_INFO(fmt, ...)       printf_log(INFO_LOG, CYAN "< LIBHAL_INFO > " fmt "--%s(%d)%s"BLANK, ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)
#define LIBHAL_DEBUG(fmt, ...)      printf_log(DEBUG_LOG, BLUE "< LIBHAL_DEBUG > " fmt "--%s(%d)%s"BLANK, ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)
#define LIBHAL_WARN(fmt, ...)       printf_log(WARNING_LOG, YELLOW  "< LIBHAL_WARNING > " fmt "--%s(%d)%s"BLANK, ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)
#define LIBHAL_ALARM(fmt, ...)      printf_log(ALARM_LOG, GREEN "< LIBHAL_ALARM > " fmt "--%s(%d)%s"BLANK, ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)
#define LIBHAL_ERROR(fmt, ...)      printf_log(ERROR_LOG, RED  "< LIBHAL_ERROR > " fmt "--%s(%d)%s"BLANK, ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)
#else
#define LIBHAL_LOG
#define LIBHAL_INFO(fmt, ...)
#define LIBHAL_DEBUG(fmt, ...)
#define LIBHAL_WARN(fmt, ...)
#define LIBHAL_ALARM(fmt, ...)
#define LIBHAL_ERROR(fmt, ...)
#endif

    /*
    **  ************************************************************************
    **                             结构体定义
    **  ************************************************************************
    */
    // log信息等级
    typedef enum
    {
        ERROR_LOG,
        ALARM_LOG,
        WARNING_LOG,
        DEBUG_LOG,
        INFO_LOG,
        LOG_LEVEL_COUNT
    }
    LOG_LEVEL;

    /*
    **  ************************************************************************
    **                             函数申明
    **  ************************************************************************
    */
    /* $Function        :   init_system_log
    ==   ===============================================================
    ==  Description     :   根据日志等级输出日志
    ==  Argument        :   [IN] terminal_level: 要输出到终端的日志等级
                            [IN] record_level: 要输出到日志文件的日志等级
                            [IN] mark: 未用
                            [IN] log_path: log文件的路径
    ==  Return          :   0：成功  -1：失败
    ==  Modification    :   2011/7/19    huang_zhendi    创建
    ==   ===============================================================
    */
    int init_system_log(LOG_LEVEL terminal_level, LOG_LEVEL record_level, int mark, char *log_path);

    /* $Function        :   printf_log
    ==   ===============================================================
    ==  Description     :   根据日志等级输出日志
    ==  Argument        :   [IN] level: 日志等级
                            [IN] fmt: 打印格式
                            [IN] ...: 其它参数
    ==  Return          :   0：成功  -1：失败
    ==  Modification    :   2011/7/19    huang_zhendi    创建
    ==   ===============================================================
    */
    int printf_log(LOG_LEVEL level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif //__LOG_H__

