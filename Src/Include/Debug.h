/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : debug.h
**  Description   : 日志头文件
**  Modification  : 2011/9/6    huang_zhendi    创建
**  ************************************************************************
*/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

    /*
    **  ************************************************************************
    **                             变量类型定义
    **  ************************************************************************
    */
    typedef int (*DEBUG_CMD_FUNC)(int count, char *input[]);      //调试命令执行的操作

    /*
    **  ************************************************************************
    **                             函数申明
    **  ************************************************************************
    */
    /* $Function        :   int init_system_debug(char *mapfile, char *debugfifo)
    ==   ===============================================================
    ==  Description     :   初始化调试资源
    ==  Argument        :   [IN] mapfile: 进程MAP文件路径
                            [IN] debugfifo: 调试输入fifo文件路径
    ==  Return          :   0: 成功  -1: 失败
    ==  Modification    :   2011/9/6    huang_zhendi    创建
    ==   ===============================================================
    */
    int init_system_debug(char *mapfile, char *debugfifo);

    /* $Function        :   register_system_debug_cmd
    ==   ===============================================================
    ==  Description     :   注册一条调试命令
    ==  Argument        :   [IN] cmd: 命令字符串
                            [IN] func: 命令执行函数
    ==  Return          :   0: 成功 -1: 失败
    ==  Modification    :   2011/9/25    huang_zhendi    创建
    ==   ===============================================================
    */
    int register_system_debug_cmd(char *cmd, DEBUG_CMD_FUNC func);

#ifdef __cplusplus
}
#endif

#endif //__DEBUG_H__

