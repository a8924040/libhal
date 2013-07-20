/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  �� �� ��   : boot.c
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : ʵ�ֶ�uboot �����������ж�ȡ�͸�д
  ����޸�   :
              BootDeviceDestroy
              BootEnvGetValue
              BootEnvInit
              BootSetPara
              main
              PrintBootEnv
              SaveBootEnv
              UpdateBootEnvCrc
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "BootEnv.h"
#include "CommonDef.h"
#include "CommonInclude.h"
#include "Flash.h"
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/
#define BOOT_ENV_DEV "/dev/mtd1"

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/
static S32 gs32BootEnvFd = -1;
static U32 gu32BootEvnOffset = 0;
static U32 gu32BootEnvSize = 0x20000;
static pthread_mutex_t BootEnvInitMutex = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************
 �� �� ��  : BootEnvInit
 ��������  : ��ʼ��ȫ�ֱ��� bootenv  �ļ������� ��resource�������bootenv
             ��ַ�ʹ�С
 �������  : void
 �������  :
 �� �� ֵ  :
 ���ú���  : 0:success
             -1: fail
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : BootDeviceDestroy
 ��������  : ����bootenv �豸 bootenv �ļ������� ����Ϊ-1
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : BootEnvGetValue
 ��������  : ��bootenv ����ֵ
 �������  : char *ps8Key:  ��Ҫ�õ���ѡ��

             char *ps8Data: ѡ��ļ�ֵ
             U32 u32Len     buff����
 �������  : ��
 �� �� ֵ  : 0:�ɹ�
             -1: ��ȡ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /* ���ҵ�Ҫ�ҵĵ�һ����ĸ��Ȼ����strstr �Ա��ַ����Ƿ���ͬ */
    do
    {
        /* ��������\0 \0 Ϊ��־bootenv ���� */
        if(*ps8BootEnvData == '\0' && * (ps8BootEnvData + 1) == '\0')
        {
            goto fail;/* �Ҳ���Ҫ�ҵ�key ��ôֱ��return -1 */
        }
        ps8BootEnvData++;
    }
    while(!(*ps8BootEnvData == *ps8Key
            && (0 == strncmp(ps8BootEnvData, ps8Key, strlen(ps8Key)))));
    /* �ҵ���Ҫ������key */
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
 �� �� ��  : PrintBootEnv
 ��������  : ��ӡȫ������������ֵ ֻ���ڵ���
 �������  : S8 *ps8BootEnvBuf
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : UpdateBootEnvCrc
 ��������  : ���ü�ֵ�����bootenv ��crc ֵ
 �������  : BOOT_ENV_INFO_STRU  *pstBootEnvInfo
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
void UpdateBootEnvCrc(BOOT_ENV_INFO_STRU  *pstBootEnvInfo)
{
    pstBootEnvInfo->u32Crc = crc32((U32) 0, (U8 *) pstBootEnvInfo->s8Data
                                   , gu32BootEnvSize - sizeof(long));
}

/*****************************************************************************
 �� �� ��  : SaveBootEnv
 ��������  : ��Ҫ���õĻ�������ֵд��flash
 �������  : const char *buf
             unsigned long addr  ��ַ
             unsigned long cnt   ����
 �������  : ��
 �� �� ֵ  : 0:�ɹ�
             -1:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : BootSetPara
 ��������  : ��bootevn ���������ı��ֵ������ѡ��
 �������  : S8 *ps8Key ѡ��
             S8 *ps8Value  ��ֵ
 �������  : ��
 �� �� ֵ  : 0 �ɹ�
             -1 ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /* ���ҵ���Ҫ�ҵĵ�һ����ĸ��Ȼ����strncmp �Ա��ַ����Ƿ���ͬ */
    do
    {
        /* ��������\0 \0 Ϊ��־bootenv ���� */
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
    /* �����ݽ���crcУ�� */
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




