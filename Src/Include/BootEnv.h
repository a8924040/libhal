/*
**  ********************************************************************************
**                                     DVRDEF
**                          Digital Video Recoder xp
**
**   (c) Copyright 1992-2008, ZheJiang Dahua Information Technology Stock CO.LTD.
**                            All Rights Reserved
**
**  File        : boot.h
**  Description : define for boot.c
**  Modify      : 2008-10-09        nils        Create the file
**  ********************************************************************************
*/

#ifndef _BOOTENV_H_
#define _BOOTENV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "crc32.h"
#include "Type.h"

    /* 现在单个环境变量最大长度为20多字节 */
#define MAX_ENV_LEN 64

    /* 通过resource  驱动获取的参数 */
#define ENV_NODE_SIZE_MAX    32
    typedef struct BOOT_ENV_RES
    {
        U32     u32EnvAddr;         /* uboot环境变量起始地址 */
        U32     u32EnvSize;         /* uboot环境变量大小 */
        S8      as8EnvNode[ENV_NODE_SIZE_MAX];     /* uboot 环境变量的节点路径 */
    } BOOT_ENV_RES_STRU;

    typedef struct BOOT_ENV_INFO
    {
        U32  u32Crc;          /* uboot环境变量的crc 校验值 */
        S8   s8Data[0];       /* uboot 环境变量数据指针 */
    } BOOT_ENV_INFO_STRU;

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
    int BootEnvInit(void);
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
    void BootDeviceDestroy(void);
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
    int BootSetPara(S8 *ps8Key, const S8 *ps8Value);
    /*****************************************************************************
     函 数 名  : BootEnvGetValue
     功能描述  : 向bootenv 设置值
     输入参数  : char *ps8Key:  需要得到的选项?
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
    int BootEnvGetValue(char *ps8Key, char *ps8Data, U32 u32Len);


#ifdef __cplusplus
}
#endif

#endif
