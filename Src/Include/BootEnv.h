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

    /* ���ڵ�������������󳤶�Ϊ20���ֽ� */
#define MAX_ENV_LEN 64

    /* ͨ��resource  ������ȡ�Ĳ��� */
#define ENV_NODE_SIZE_MAX    32
    typedef struct BOOT_ENV_RES
    {
        U32     u32EnvAddr;         /* uboot����������ʼ��ַ */
        U32     u32EnvSize;         /* uboot����������С */
        S8      as8EnvNode[ENV_NODE_SIZE_MAX];     /* uboot ���������Ľڵ�·�� */
    } BOOT_ENV_RES_STRU;

    typedef struct BOOT_ENV_INFO
    {
        U32  u32Crc;          /* uboot����������crc У��ֵ */
        S8   s8Data[0];       /* uboot ������������ָ�� */
    } BOOT_ENV_INFO_STRU;

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
    int BootEnvInit(void);
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
    void BootDeviceDestroy(void);
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
    int BootSetPara(S8 *ps8Key, const S8 *ps8Value);
    /*****************************************************************************
     �� �� ��  : BootEnvGetValue
     ��������  : ��bootenv ����ֵ
     �������  : char *ps8Key:  ��Ҫ�õ���ѡ��?
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
    int BootEnvGetValue(char *ps8Key, char *ps8Data, U32 u32Len);


#ifdef __cplusplus
}
#endif

#endif
