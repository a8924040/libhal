/*
**  ************************************************************************
**
**  File    Name  : UART.h
**  Description   : �������ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
#ifndef __SMTP_H__
#define __SMTP_H__

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_ATTACHMENT_SUM  32

typedef struct EMAIL_INFO
{
    S8 *ps8EmailTo;/* �ʼ����յ�ַ */
    S8 *ps8EmailFrom;/* �ʼ����͵�ַ */
    S8 *ps8EmailAccount;/* �ʼ������˻� */
    S8 *ps8EmailPwd; /* �ʼ������˻����� */
    S8 *ps8EmailTitle;/* �ʼ����� */
    S8 *ps8EmailBody;/* �ʼ����� */
   
}EMAIL_INFO_STRU;

typedef struct ATTACHMENT_INFO
{
    S8 *ps8FilePath;
    S8 *ps8FileName;    
}ATTACHMENT_INFO_STRU;

typedef struct ATTACHMENT_ARRAY
{
  ATTACHMENT_INFO_STRU stAttachment[MAX_ATTACHMENT_SUM ];
  U32 u32AttachSum;
}ATTACHMENT_ARRAY_STRU;

/*****************************************************************************
 �� �� ��  : SendEmail
 ��������  : ʹ��smtp ����Email
 �������  : EMAIL_INFO_STRU *pstEmailInfo     
             ATTACHMENT_ARRAY_STRU *pstAttach  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��3��1��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/

int SendEmail(EMAIL_INFO_STRU *pstEmailInfo, ATTACHMENT_ARRAY_STRU *pstAttach);


#ifdef __cplusplus
}
#endif

#endif
