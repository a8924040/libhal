/*
**  ************************************************************************
**
**  File    Name  : COMMON.h
**  Description   : 通用包含头文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

    /* $Function        :   RunSystemCommand
    ==   ===============================================================
    ==  Description     :   模仿系统system命令
    ==  Argument        :   system 的命令
    ==  Return          :   0: 成功  -1: 失败
    ==  Modification    :   2011/12/24   tango_zhu  创建
    ==   ===============================================================
    */
    int  RunSystemCommand(const char *command);

    /* $Function        :   wireless_hotplug_thd
    ==   ===============================================================
    ==  Description     :   热插拔任务, 监听热插拔信息，并上报3G模块监控任务
    ==  Argument        :   无
    ==  Return          :   无
    ==  Modification    :   2011/7/19    huang_zhendi    创建
    ==   ===============================================================
    */
    void *wireless_hotplug_thd(void *arg);

    /* $Function        :   uint_sleep
    ==   ===============================================================
    ==  Description     :   秒级睡眠,不会被信号打断
    ==  Argument        :   [IN] sec: 睡眠时间，单位秒
    ==  Return          :   无
    ==  Modification    :   2011/7/19    huang_zhendi    创建
    ==   ===============================================================
    */
    void uint_sleep(int sec);

    /* $Function        :   uint_msleep
    ==   ===============================================================
    ==  Description     :   毫秒级睡眠,不会被信号打断
    ==  Argument        :   [IN] msec: 睡眠时间，单位毫秒
    ==  Return          :   无
    ==  Modification    :   2011/7/19    huang_zhendi    创建
    ==   ===============================================================
    */
    void uint_msleep(int msec);

    /* $Function        :   set_bit_mask
    ==   ===============================================================
    ==  Description     :   设置一位掩码
    ==  Argument        :   pval:设置值的指针，index 设置的位，val 设置的值
    ==  Return          :   无
    ==  Modification    :   2011/7/19    tango_zhu     创建
    ==   ===============================================================
    */
    void set_bit_mask(unsigned int *pval, unsigned char bit_index, unsigned char bit_val);
    /*****************************************************************************
    函 数 名  : CallForkProcess
    功能描述  : fork 一个进程
    输入参数  : char *pProcessPath:process 路径
             char **pArgv  参数 第一个参数必须是程序名
    输出参数  : 无
    返 回 值  :
    调用函数  :
    被调函数  :

    修改历史      :
    1.日    期   : 2012年11月17日
    作    者   : tango_zhu
    修改内容   : 新生成函数

    *****************************************************************************/
    int CallForkProcess(char *pProcessPath, char **pArgv);
#ifdef __cplusplus
}
#endif

#endif
