/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.cpp
**  Description   : 日志程序
**  Modification  : 2011/7/19    huang_zhendi    创建
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              头文件
**  ************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "Log.h"
#include "Debug.h"

/*
**  ************************************************************************
**                             宏定义
**  ************************************************************************
*/
#define LOG_FILE_SIZE 1024
#define LOG_BUF_SIZE 512

/*
**  ************************************************************************
**                             全局变量定义
**  ************************************************************************
*/
static LOG_LEVEL terminal_log_level = INFO_LOG;      //要输出到终端的日志等级
static unsigned int log_mark = 0xff;        //未用
static LOG_LEVEL record_log_level = ERROR_LOG;  //要输出到日志文件的日志等级
static FILE *log_fd = NULL;

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
int printf_log(LOG_LEVEL level, const char *fmt, ...)
{
    va_list vaList;
    int len, total_len = 0;
    char buf[LOG_BUF_SIZE];
    char *pbuf;
    long curr_pos;
    time_t timep;
    struct tm *curr_time;
    //不加锁，即使出错也不会有太大的问题
    if((level > terminal_log_level) && (level > record_log_level))
    {
        return 0;
    }
    pbuf = buf;
    va_start(vaList, fmt);
    len = vsnprintf(pbuf, sizeof(buf), fmt, vaList);
    va_end(vaList);
    if(len < 0)
    {
        return -1;
    }
    total_len += len;
    if(level <= terminal_log_level)
    {
        printf("%s\n", buf);
    }
    if((level <= record_log_level) && (log_fd != NULL))
    {
        time(&timep);
        curr_time = localtime(&timep);
        pbuf += len;
        len = snprintf(pbuf, sizeof(buf) - total_len, " %04d-%02d-%02d %02d:%02d:%02d\n",
                       curr_time->tm_year + 1900, curr_time->tm_mon + 1, curr_time->tm_mday,
                       curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
        if(len < 0)
        {
            return -1;
        }
        total_len += len;
        curr_pos = ftell(log_fd);
        if(curr_pos + total_len > LOG_FILE_SIZE)    //限制日志文件大小
        {
            rewind(log_fd);
        }
        if(fwrite(buf, sizeof(char), total_len, log_fd) != total_len)
        {
            return -1;
        }
        fflush(log_fd);
    }
    return 0;
}

/* $Function        :   config_log
==   ===============================================================
==  Description     :   设置日志输出等级
==  Argument        :   [IN] terminal_level: 要输出到终端的日志等级
                        [IN] record_level: 要输出到日志文件的日志等级
                        [IN] mark: 未用
==  Return          :   无
==  Modification    :   2011/7/19    huang_zhendi    创建
==   ===============================================================
*/
static void config_log(LOG_LEVEL terminal_level, LOG_LEVEL record_level, int mark)
{
    if(terminal_level < LOG_LEVEL_COUNT)
    {
        terminal_log_level = terminal_level;
    }
    if(record_level < LOG_LEVEL_COUNT)
    {
        record_log_level = record_level;
    }
    log_mark = mark;
}

/* $Function        :   set_debug_level
==   ===============================================================
==  Description     :   设置调试信息输出等级
==  Argument        :   [IN] count: 参数个数
                        [IN] input: 字符形式的输入参数数组
==  Return          :   0: 成功 -1: 失败
==  Modification    :   2011/9/25    huang_zhendi    创建
==   ===============================================================
*/
static int set_debug_level(int count, char *input[])
{
    int debug_level;
    if(count != 1)
    {
        return -1;
    }
    sscanf(input[0], "%d", &debug_level);
    config_log(debug_level, LOG_LEVEL_COUNT, 0xff);
    return 0;
}

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
int init_system_log(LOG_LEVEL terminal_level, LOG_LEVEL record_level, int mark, char *log_path)
{
    terminal_log_level = terminal_level;
    record_log_level = record_level;
    log_mark = mark;
    if(log_path != NULL)
    {
        log_fd = fopen(log_path, "w+");
        if(log_fd == NULL)
        {
            printf("Fail to Open log file  --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
            return -1;
        }
        if(register_system_debug_cmd("set_debug_level", set_debug_level) < 0)
        {
            printf("Fail to Open log file  --%s(%d)%s\n", __FILE__, __LINE__, __FUNCTION__);
            return -1;
        }
    }
    return 0;
}

