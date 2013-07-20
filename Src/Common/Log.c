/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.cpp
**  Description   : ��־����
**  Modification  : 2011/7/19    huang_zhendi    ����
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              ͷ�ļ�
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
**                             �궨��
**  ************************************************************************
*/
#define LOG_FILE_SIZE 1024
#define LOG_BUF_SIZE 512

/*
**  ************************************************************************
**                             ȫ�ֱ�������
**  ************************************************************************
*/
static LOG_LEVEL terminal_log_level = INFO_LOG;      //Ҫ������ն˵���־�ȼ�
static unsigned int log_mark = 0xff;        //δ��
static LOG_LEVEL record_log_level = ERROR_LOG;  //Ҫ�������־�ļ�����־�ȼ�
static FILE *log_fd = NULL;

/* $Function        :   printf_log
==   ===============================================================
==  Description     :   ������־�ȼ������־
==  Argument        :   [IN] level: ��־�ȼ�
                        [IN] fmt: ��ӡ��ʽ
                        [IN] ...: ��������
==  Return          :   0���ɹ�  -1��ʧ��
==  Modification    :   2011/7/19    huang_zhendi    ����
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
    //����������ʹ����Ҳ������̫�������
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
        if(curr_pos + total_len > LOG_FILE_SIZE)    //������־�ļ���С
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
==  Description     :   ������־����ȼ�
==  Argument        :   [IN] terminal_level: Ҫ������ն˵���־�ȼ�
                        [IN] record_level: Ҫ�������־�ļ�����־�ȼ�
                        [IN] mark: δ��
==  Return          :   ��
==  Modification    :   2011/7/19    huang_zhendi    ����
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
==  Description     :   ���õ�����Ϣ����ȼ�
==  Argument        :   [IN] count: ��������
                        [IN] input: �ַ���ʽ�������������
==  Return          :   0: �ɹ� -1: ʧ��
==  Modification    :   2011/9/25    huang_zhendi    ����
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
==  Description     :   ������־�ȼ������־
==  Argument        :   [IN] terminal_level: Ҫ������ն˵���־�ȼ�
                        [IN] record_level: Ҫ�������־�ļ�����־�ȼ�
                        [IN] mark: δ��
                        [IN] log_path: log�ļ���·��
==  Return          :   0���ɹ�  -1��ʧ��
==  Modification    :   2011/7/19    huang_zhendi    ����
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

