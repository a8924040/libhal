/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : boot.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : 实现对uboot 环境变量进行读取和改写
  最近修改   :
              BootDeviceDestroy
              BootEnvGetValue
              BootEnvInit
              BootSetPara
              main
              PrintBootEnv
              SaveBootEnv
              UpdateBootEnvCrc
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "BootEnv.h"
#include "CommonDef.h"
#include "CommonInclude.h"
#include "Flash.h"
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
#define BOOT_ENV_DEV "/dev/mtd1"

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
static S32 gs32BootEnvFd = -1;
static U32 gu32BootEvnOffset = 0;
static U32 gu32BootEnvSize = 0x20000;
static pthread_mutex_t BootEnvInitMutex = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************
 函 数 名  : BootEnvInit
 功能描述  : 初始化全局变量 bootenv  文件描述符 从resource驱动获得bootenv
             地址和大小
 输入参数  : void
 输出参数  :
 返 回 值  :
 调用函数  : 0:success
             -1: fail
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int BootEnvInit(void)
{
    pthread_mutex_lock(&BootEnvInitMutex);
    gs32BootEnvFd = open(BOOT_ENV_DEV, O_RDWR | O_SYNC);
    if(gs32BootEnvFd < 0)
    {
        LIBHAL_ERROR("Open uboot param section  falied!\n");
        goto fail;
    }
    pthread_mutex_unlock(&BootEnvInitMutex);
    return 0;
fail:
    if(gs32BootEnvFd > 0)
    {
        close(gs32BootEnvFd);
    }
    gs32BootEnvFd  =  -1;
    pthread_mutex_unlock(&BootEnvInitMutex);
    return -1;
}


/*****************************************************************************
 函 数 名  : BootDeviceDestroy
 功能描述  : 销毁bootenv 设备 bootenv 文件描述符 设置为-1
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
void BootDeviceDestroy(void)
{
    if(gs32BootEnvFd > 0)
    {
        close(gs32BootEnvFd);
    }
    gs32BootEnvFd  =  -1;
    gu32BootEvnOffset = 0;
    gu32BootEnvSize = 0;
}

/*****************************************************************************
 函 数 名  : BootEnvGetValue
 功能描述  : 向bootenv 设置值
 输入参数  : char *ps8Key:  需要得到的选项

             char *ps8Data: 选项的键值
             U32 u32Len     buff长度
 输出参数  : 无
 返 回 值  : 0:成功
             -1: 获取错误
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int BootEnvGetValue(char *ps8Key, char *ps8Data, U32 u32Len)
{
    S8 *ps8BootEnvBuf =  NULL;
    S32 s32RetVal = -1;
    S8 *ps8BootEnvData = NULL;
    BOOT_ENV_INFO_STRU *pstBootEnvInfo;
    if(NULL == ps8Key  || NULL == ps8Data || u32Len <= 0)
    {
        LIBHAL_ERROR("BootEnvGetValue param error!\n");
        goto fail;
    }
    if((gs32BootEnvFd < 0) || (gu32BootEnvSize == 0))
    {
        if(BootEnvInit() < 0)
        {
            LIBHAL_ERROR("boot env get value init failed!");
            goto fail;
        }
    }
    ps8BootEnvBuf = (S8 *) malloc(gu32BootEnvSize);
    if(NULL == ps8BootEnvBuf)
    {
        LIBHAL_ERROR(" malloc BootEnvBuf failed!");
        goto fail;
    }
    lseek(gs32BootEnvFd, gu32BootEvnOffset, SEEK_SET);
    s32RetVal = read(gs32BootEnvFd, ps8BootEnvBuf, gu32BootEnvSize);
    if(s32RetVal != gu32BootEnvSize)
    {
        LIBHAL_ERROR("read boot evn faild!\n");
        BootDeviceDestroy();
        goto fail;
    }
    pstBootEnvInfo = (BOOT_ENV_INFO_STRU *) ps8BootEnvBuf;
    ps8BootEnvData = pstBootEnvInfo->s8Data;
    /* 先找到要找的第一个字母，然后用strstr 对比字符串是否相同 */
    do
    {
        /* 遇到两个\0 \0 为标志bootenv 结束 */
        if(*ps8BootEnvData == '\0' && * (ps8BootEnvData + 1) == '\0')
        {
            goto fail;/* 找不到要找到key 那么直接return -1 */
        }
        ps8BootEnvData++;
    }
    while(!(*ps8BootEnvData == *ps8Key
            && (0 == strncmp(ps8BootEnvData, ps8Key, strlen(ps8Key)))));
    /* 找到需要搜索的key */
    ps8BootEnvData += strlen(ps8Key) + 1;
    u32Len = strlen(ps8BootEnvData) > u32Len ? u32Len : strlen(ps8BootEnvData);
    strncpy(ps8Data, ps8BootEnvData, u32Len);
#ifdef DEBUG
    LIBHAL_DEBUG(" u32Len = 0x%x, ps8Data is %s\n", u32Len, ps8Data);
#endif
    free(ps8BootEnvBuf);
    return 0;
fail:
    if(NULL != ps8BootEnvBuf)
    {
        free(ps8BootEnvBuf);
    }
    return -1;
}

/*****************************************************************************
 函 数 名  : PrintBootEnv
 功能描述  : 打印全部环境变量的值 只用于调试
 输入参数  : S8 *ps8BootEnvBuf
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
void PrintBootEnv(S8 *ps8BootEnvBuf)
{
    int i;
    for(i = 0; i < gu32BootEnvSize; i ++)
    {
        if(* (ps8BootEnvBuf + i) == '\0' && * (ps8BootEnvBuf + i + 1) == '\0')
        {
            break;
        }
        if(* (ps8BootEnvBuf + i) == '\0')
        {
            printf("the value is %s\n", ps8BootEnvBuf + i + 1);
        }
        if(i == 0)
        {
            printf("the value is %s\n", ps8BootEnvBuf);
        }
    }
}

/*****************************************************************************
 函 数 名  : UpdateBootEnvCrc
 功能描述  : 设置键值后更新bootenv 的crc 值
 输入参数  : BOOT_ENV_INFO_STRU  *pstBootEnvInfo
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
void UpdateBootEnvCrc(BOOT_ENV_INFO_STRU  *pstBootEnvInfo)
{
    pstBootEnvInfo->u32Crc = crc32((U32) 0, (U8 *) pstBootEnvInfo->s8Data
                                   , gu32BootEnvSize - sizeof(long));
}

/*****************************************************************************
 函 数 名  : SaveBootEnv
 功能描述  : 将要设置的环境变量值写入flash
 输入参数  : const char *buf
             unsigned long addr  地址
             unsigned long cnt   长度
 输出参数  : 无
 返 回 值  : 0:成功
             -1:失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int SaveBootEnv(const S8 *ps8BootEnvBuf, U32 u32BootEnvOffset)
{
    S32 s32WriteSum = 0;
    S32 s32BlockSum = 0;
    if(NULL == ps8BootEnvBuf || u32BootEnvOffset < 0)
    {
        LIBHAL_ERROR("SaveBootEnv parma error!");
        return -1;
    }
    s32BlockSum = GetRegionCount("mtd1");
    LIBHAL_DEBUG("s32BlockSum is %d", s32BlockSum);
    NonRegionErase(gs32BootEnvFd, 0, s32BlockSum, 0);
    lseek(gs32BootEnvFd, gu32BootEvnOffset, SEEK_SET);
    LIBHAL_DEBUG("Writing ENV to flash!");
    s32WriteSum = write(gs32BootEnvFd, ps8BootEnvBuf, gu32BootEnvSize);
    return s32WriteSum == gu32BootEnvSize ? 0 : -1;
}

/*****************************************************************************
 函 数 名  : BootSetPara
 功能描述  : 向bootevn 环境变量改变键值或增加选项
 输入参数  : S8 *ps8Key 选项
             S8 *ps8Value  键值
 输出参数  : 无
 返 回 值  : 0 成功
             -1 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int BootSetPara(S8 *ps8Key, const S8 *ps8Value)
{
    S8 *ps8BootEnvBuf =  NULL;
    S32 s32RetVal = -1;
    BOOT_ENV_INFO_STRU *pstBootEnvInfo = NULL;
    S8 *ps8BootEnvData = NULL;
    if(NULL == ps8Key  || NULL == ps8Value)
    {
        LIBHAL_ERROR("BootSetPara param error!\n");
        goto fail;
    }
    if((gs32BootEnvFd < 0) || (gu32BootEnvSize == 0))
    {
        if(BootEnvInit() < 0)
        {
            LIBHAL_ERROR("boot env get value init failed!");
            goto fail;
        }
    }
    ps8BootEnvBuf = (S8 *) malloc(gu32BootEnvSize);
    if(NULL == ps8BootEnvBuf)
    {
        LIBHAL_ERROR(" malloc BootEnvBuf failed!");
        goto fail;
    }
    memset(ps8BootEnvBuf, 0, gu32BootEnvSize);
    lseek(gs32BootEnvFd, gu32BootEvnOffset, SEEK_SET);
    s32RetVal = read(gs32BootEnvFd, ps8BootEnvBuf, gu32BootEnvSize);
    if(s32RetVal != gu32BootEnvSize)
    {
        LIBHAL_ERROR("read boot evn faild!\n");
        BootDeviceDestroy();
        goto fail;
    }
    pstBootEnvInfo = (BOOT_ENV_INFO_STRU *) ps8BootEnvBuf;
    ps8BootEnvData = pstBootEnvInfo->s8Data;
#ifdef DEBUG
    LIBHAL_DEBUG("Old BootEnvCrc is 0x%x\n", pstBootEnvInfo->u32Crc);
    LIBHAL_DEBUG("Read BootEnv From boot env is:\n");
    PrintBootEnv(pstBootEnvInfo->s8Data);
#endif
    /* 先找到和要找的第一个字母，然后用strncmp 对比字符串是否相同 */
    do
    {
        /* 遇到两个\0 \0 为标志bootenv 结束 */
        if(*ps8BootEnvData == '\0' && * (ps8BootEnvData + 1) == '\0')
        {
            if((ps8BootEnvData - pstBootEnvInfo->s8Data)
                    > gu32BootEnvSize - (strlen(ps8Key) +  strlen(ps8Value) + 3))
            {
                LIBHAL_ERROR("there is no enough flash space for save!\n");
                goto fail;
            }
            else
            {
                sprintf(ps8BootEnvData + 1, "%s=%s", ps8Key, ps8Value);
                LIBHAL_DEBUG("will set value is %s\n", ps8BootEnvData + 1);
                goto saveenv;
            }
        }
        ps8BootEnvData++;
    }
    while(!(*ps8BootEnvData == *ps8Key
            && (0 == strncmp(ps8BootEnvData, ps8Key, strlen(ps8Key)))));
    sprintf(ps8BootEnvData, "%s=%s", ps8Key, ps8Value);
#ifdef DEBUG
    LIBHAL_DEBUG("New BootEnv  is:\n");
    PrintBootEnv(pstBootEnvInfo->s8Data);
#endif
saveenv:
    LIBHAL_INFO("Will save %s = %s", ps8Key, ps8Value);
    LIBHAL_INFO("Save environment to flash ,Please wait ......");
    LIBHAL_INFO("Warning!! Don't power off!");
    /* 对内容进行crc校验 */
    UpdateBootEnvCrc(pstBootEnvInfo);
    LIBHAL_DEBUG("New BootEnvCrc is 0x%x\n", pstBootEnvInfo->u32Crc);
    if(SaveBootEnv(ps8BootEnvBuf, gu32BootEvnOffset) < 0)
    {
        LIBHAL_ERROR("Save environmeent to flash fail\n");
        goto fail;
    }
    LIBHAL_DEBUG("save boot env success!");
    free(ps8BootEnvBuf);
    return 0;
fail:
    if(NULL != ps8BootEnvBuf)
    {
        free(ps8BootEnvBuf);
    }
    return -1;
}




