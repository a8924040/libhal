/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.h
**  Description   : ��־ͷ�ļ�
**  Modification  : 2011/7/19    huang_zhendi    ����
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
    **                             �궨��
    **  ************************************************************************
    */
#define __LIBHAL_DEBUG__

#define LIBHAL_PRINTF(fmt, ...)     printf(fmt "--%s(%d)%s\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)

    /*���LOG�ȼ����ڵ�ǰLOG�ȼ�����Ϣ  */
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
    **                             �ṹ�嶨��
    **  ************************************************************************
    */
    // log��Ϣ�ȼ�
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
    **                             ��������
    **  ************************************************************************
    */
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
    int init_system_log(LOG_LEVEL terminal_level, LOG_LEVEL record_level, int mark, char *log_path);

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
    int printf_log(LOG_LEVEL level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif //__LOG_H__

