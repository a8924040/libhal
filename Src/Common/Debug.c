/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>

#include "Log.h"
#include "Debug.h"

/*
**  ************************************************************************
**                              宏文件
**  ************************************************************************
*/
#define MAX_PARAM_COUNT 5       //调试命令输入的最大参数个数

#define MIN_SYMBOL_SECTION 3    //支持解析的符号在map文件中的最小段数
#define MAX_SYMBOL_SECTION 4    //支持解析的符号在map文件中的最大段数

#define VAR_TYPE_COUNT sizeof(type_str)/sizeof(type_str[0]) //支持的符号变量类型

#define MAX_CMD_COUNT 16 //调试命令个数
#define MAX_CMD_LEN   64 //命令字符串最大长度
/*
**  ************************************************************************
**                             函数申明
**  ************************************************************************
*/

/*
**  ************************************************************************
**                              结构体/类型定义
**  ************************************************************************
*/
typedef void (*DEBUG_FUNC)();                                //函数指针

typedef struct                      //调试命令结构体
{
    char name[MAX_CMD_LEN];                //调试命令字符串
    DEBUG_CMD_FUNC func;            //调试命令执行的操作
} DEBUG_CMD;

/*
**  ************************************************************************
**                              全局变量定义
**  ************************************************************************
*/
static char map_path[128];  //保存map文件路径
static char debuf_fifo_path[128];  //保存map文件路径

const char type_str[][32] = //符号变量类型字符串
{
    "char",
    "int16",
    "int32",
    "str",
};

DEBUG_CMD debug_cmd[MAX_CMD_COUNT]; //调试命令数组

/*
**  ************************************************************************
**                             函数定义
**  ************************************************************************
*/
/* $Function        :   set_map_path
==   ===============================================================
==  Description     :   设置MAP文件的路径
==  Argument        :   [IN] count: 参数个数
                        [IN] input: 字符形式的输入参数数组
==  Return          :   0: 成功 -1: 失败
==  Modification    :   2011/9/25    huang_zhendi    创建
==   ===============================================================
*/
static int set_map_path(int count, char *input[])
{
    if(count != 1)
    {
        return -1;
    }
    strcpy(map_path, input[0]);
    return 0;
}

/* $Function        :   deal_cmd_in_table
==   ===============================================================
==  Description     :   解析输入函数的参数并执行
==  Argument        :   [IN] count: 参数个数
                        [IN] input: 字符形式的输入参数数组
==  Return          :   0: 找到并执行命令 -1: 未找到命令
==  Modification    :   2011/9/25    huang_zhendi    创建
==   ===============================================================
*/
static int deal_cmd_in_table(int count, char *input[])
{
    int i;
    for(i = 0; i < MAX_CMD_COUNT; i++)
    {
        if(debug_cmd[i].func == NULL)
        {
            continue;
        }
        if(strlen(debug_cmd[i].name) != strlen(input[0]))
        {
            continue;
        }
        if(strcmp(input[0], debug_cmd[i].name) == 0)
        {
            if(debug_cmd[i].func(count - 1, ++input) < 0)
            {
                LIBHAL_PRINTF("Fail to exec command");
            }
            else
            {
                LIBHAL_PRINTF("Success to exec command");
            }
            return 0;
        }
    }
    return -1;
}

/* $Function        :   function_deal
==   ===============================================================
==  Description     :   解析输入函数的参数并执行
==  Argument        :   [IN] pfun: 要执行的函数
                        [IN] argc: 参数个数
                        [IN] argv: 字符形式的输入参数数组
==  Return          :   无
==  Modification    :   2011/9/6    huang_zhendi    创建
==   ===============================================================
*/
static int function_deal(DEBUG_FUNC pfun, int argc, char *argv[])
{
    int i;
    int param[MAX_PARAM_COUNT - 1];
    for(i = 0; i < argc; i++)
    {
        if(argv[i][0] == '"')    //参数为字符串
        {
            argv[i]++;
            param[i] = (int)(argv[i]);
            continue;
        }
        if(sscanf(argv[i], "%x", &param[i]) == EOF)       //参数为数值
        {
            return -1;
        }
    }
    switch(argc)
    {
        case 0:
            pfun();
            break;
        case 1:
            pfun(param[0]);
            break;
        case 2:
            pfun(param[0], param[1]);
            break;
        case 3:
            pfun(param[0], param[1], param[2]);
            break;
        case 4:
            pfun(param[0], param[1], param[2], param[3]);
            break;
        default:
            LIBHAL_PRINTF("Too many parameters!");
    }
    return 0;
}

/* $Function        :   variable_deal
==   ===============================================================
==  Description     :   解析输入变量的参数并显示或设置
==  Argument        :   [IN] pfun: 要执行的函数
                        [IN] argc: 参数个数
                        [IN] argv: 字符形式的输入参数数组
==  Return          :   无
==  Modification    :   2011/9/6    huang_zhendi    创建
==   ===============================================================
*/
static int variable_deal(void *pvar, int argc, char *argv[])
{
    int i;
    if(argc < 1)    //至少有一个类型参数
    {
        return -1;
    }
    for(i = 0; i < VAR_TYPE_COUNT; i++)
    {
        if(strlen(argv[0]) != strlen(type_str[i]))
        {
            continue;
        }
        if(strcmp(argv[0], type_str[i]) == 0)
        {
            break;
        }
    }
    if(i == VAR_TYPE_COUNT)
    {
        return -1;
    }
    switch(i)
    {
        case 0:
            //printf("%c\n", (char)*pvar);
            break;
        case 1:
            //printf("%0xx\n", *pvar);
            break;
        case 2:
            LIBHAL_PRINTF("current value: 0x%x\n", * ((int *) pvar));
            if(argc == 2)
            {
                if(sscanf(argv[1], "%x", (int *) pvar) == EOF)
                {
                    return -1;
                }
                LIBHAL_PRINTF("new value: 0x%x\n", * ((int *) pvar));
            }
            break;
        case 3:
            LIBHAL_PRINTF("current value: %s\n", (char *) pvar);
            if(argc == 2)
            {
                strcpy((char *) pvar, argv[1]);
                LIBHAL_PRINTF("new value: %s\n", (char *) pvar);
            }
            break;
    }
    return 0;
}

/* $Function        :   deal_cmd_in_mapfile
==   ===============================================================
==  Description     :   解析输入函数的参数并执行
==  Argument        :   [IN] map_fd: map文件文件描述符
                        [IN] count: 参数个数
                        [IN] input: 字符形式的输入参数数组
==  Return          :   无
==  Return          :   0: 成功 -1: 失败
==  Modification    :   2011/9/6    huang_zhendi    创建
==   ===============================================================
*/
static int deal_cmd_in_mapfile(int count, char *input[])
{
    int i;
    DEBUG_FUNC pfunc;
    void *pvar;
    char map[256];
    char *p[MAX_SYMBOL_SECTION];
    FILE *map_fd;
    if(access(map_path, F_OK) < 0)
    {
        LIBHAL_PRINTF("Map file is not exist! error: %s!", strerror(errno));
        return -1;
    }
    map_fd = fopen(map_path, "r");
    if(map_fd == NULL)
    {
        LIBHAL_PRINTF("error: %s!", strerror(errno));
        return -1;
    }
    rewind(map_fd);
    while(fgets(map, sizeof(map), map_fd))
    {
        map[strlen(map) - 1] = 0;    //去掉换行符
        i = 0;
        p[0] = strtok(map, " \t");
        do
        {
            if(p[i] == NULL)
            {
                break;
            }
            p[++i] = strtok(NULL, " \t");
        }
        while(i < MAX_SYMBOL_SECTION);
        //map文件中变量符号分3段，函数分4段
        if((i == MAX_SYMBOL_SECTION) && strtok(NULL, " \t"))
        {
            continue;
        }
        if(i < MIN_SYMBOL_SECTION)
        {
            continue;
        }
        //map文件符号类型为单字节
        if(strlen(p[1]) != 1)
        {
            continue;
        }
        switch(toupper(p[1][0]))
        {
            case 'T':
                if(strlen(p[2]) != strlen(input[0]))
                {
                    continue;
                }
                if(strcmp(input[0], p[2]) == 0)
                {
                    sscanf(p[0], "%p", &pfunc);
                    if(function_deal(pfunc, count - 1, &input[1]) < 0)
                    {
                        LIBHAL_PRINTF("Fail to exec command");
                    }
                    else
                    {
                        LIBHAL_PRINTF("Success to exec command");
                    }
                    fclose(map_fd);
                    return 0;
                }
                break;
            case 'B':
            case 'D':
                if(strlen(p[2]) != strlen(input[0]))
                {
                    continue;
                }
                if(strcmp(input[0], p[2]) == 0)
                {
                    sscanf(p[0], "%p", &pvar);
                    if(variable_deal(pvar, count - 1, &input[1]) < 0)
                    {
                        LIBHAL_PRINTF("Fail to exec command");
                    }
                    else
                    {
                        LIBHAL_PRINTF("Success to exec command");
                    }
                    fclose(map_fd);
                    return 0;
                }
                break;
            default:
                break;
        }
    }
    fclose(map_fd);
    return -1;
}

/* $Function        :   deal_debug_cmd
==   ===============================================================
==  Description     :   解析输入命令并处理
==  Argument        :   [IN] count: 参数个数
                        [IN] input: 字符形式的输入参数数组
==  Return          :   0: 命令存在 -1: 命令不存在
==  Modification    :   2011/9/25    huang_zhendi    创建
==   ===============================================================
*/
static int deal_debug_cmd(int count, char *input[])
{
    if(deal_cmd_in_table(count, input) == 0)
    {
        return 0;
    }
    if(deal_cmd_in_mapfile(count, input) == 0)
    {
        return 0;
    }
    return -1;
}

/* $Function        :   debug_thd
==   ===============================================================
==  Description     :   调试线程
==  Argument        :   无
==  Return          :   无
==  Modification    :   2011/9/6   huang_zhendi    创建
==   ===============================================================
*/
static void *system_debug_thd(void *arg)
{
    FILE *fifo_fd;
    char buf[256];
    char *input[MAX_PARAM_COUNT];
    int i;
    pthread_detach(pthread_self());
    while(1)
    {
        fifo_fd = fopen(debuf_fifo_path, "r");
        if(fifo_fd == NULL)
        {
            LIBHAL_PRINTF("Fail to open commad fifo! error: %s!", strerror(errno));
            break;
        }
        if(fgets(buf, sizeof(buf), fifo_fd) == NULL)
        {
            fclose(fifo_fd);
            continue;
        }
        buf[strlen(buf) - 1] = 0;    //去掉换行符
        i = 0;
        input[0] = strtok(buf, " \t");
        do
        {
            if(input[i] == NULL)
            {
                break;
            }
            input[++i] = strtok(NULL, " \t");
        }
        while(i < MAX_PARAM_COUNT);
        if((i == MAX_PARAM_COUNT)  && strtok(NULL, " "))
        {
            LIBHAL_PRINTF("Too many parameters!");
            fclose(fifo_fd);
            continue;
        }
        if(deal_debug_cmd(i, input) < 0)
        {
            LIBHAL_PRINTF("Input command is invalid!");
        }
        fclose(fifo_fd);
    }
    return (void *)(-1);
}

/* $Function        :   register_system_debug_cmd
==   ===============================================================
==  Description     :   注册一条调试命令
==  Argument        :   [IN] cmd: 命令字符串
                        [IN] func: 命令执行函数
==  Return          :   0: 成功 -1: 失败
==  Modification    :   2011/9/25    huang_zhendi    创建
==   ===============================================================
*/
int register_system_debug_cmd(char *cmd, DEBUG_CMD_FUNC func)
{
    int i;
    if(strlen(cmd) > (MAX_CMD_COUNT - 1))
    {
        LIBHAL_ERROR("Command string too long!");
        return -1;
    }
    if(func == NULL)
    {
        LIBHAL_ERROR("Command function can't be NULL!");
        return -1;
    }
    for(i = 0; i < MAX_CMD_COUNT; i++)
    {
        if(debug_cmd[i].func == NULL)
        {
            strcpy(debug_cmd[i].name, cmd);
            debug_cmd[i].func = func;
            return 0;
        }
    }
    return -1;
}

/* $Function        :   init_system_debug
==   ===============================================================
==  Description     :   初始化调试资源
==  Argument        :   [IN] mapfile: 进程MAP文件路径
                        [IN] debugfifo: 调试输入fifo文件路径
==  Return          :   0: 成功  -1: 失败
==  Modification    :   2011/9/6    huang_zhendi    创建
==   ===============================================================
*/
int init_system_debug(char *mapfile, char *debugfifo)
{
    pthread_t system_debug_tid;
    strcpy(map_path, mapfile);
    strcpy(debuf_fifo_path, debugfifo);
    if(access(debuf_fifo_path, F_OK) < 0)
    {
        if(mkfifo(debuf_fifo_path, O_RDWR | O_CREAT | O_TRUNC) < 0)
        {
            LIBHAL_ERROR("Fail to creat command fifo! error: %s!", strerror(errno));
            return -1;
        }
    }
    if(register_system_debug_cmd("set_map_path", set_map_path) < 0)
    {
        LIBHAL_ERROR("Fail to creat register set_map_path!");
        return -1;
    }
    if(pthread_create(&system_debug_tid, NULL, system_debug_thd, NULL) != 0)
    {
        LIBHAL_ERROR("Fail to start debug_thd thread! error: %s", strerror(errno));
        return -1;
    }
    return 0;
}

