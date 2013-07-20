/*
**  ************************************************************************
**
**  File    Name  : UART.h
**  Description   : 串口相关头文件
**  Modification  : 2011/11/24   tango_zhu   创建
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
    S8 *ps8EmailTo;/* 邮件接收地址 */
    S8 *ps8EmailFrom;/* 邮件发送地址 */
    S8 *ps8EmailAccount;/* 邮件发送账户 */
    S8 *ps8EmailPwd; /* 邮件发送账户密码 */
    S8 *ps8EmailTitle;/* 邮件标题 */
    S8 *ps8EmailBody;/* 邮件内容 */
   
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
 函 数 名  : SendEmail
 功能描述  : 使用smtp 发送Email
 输入参数  : EMAIL_INFO_STRU *pstEmailInfo     
             ATTACHMENT_ARRAY_STRU *pstAttach  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月1日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/

int SendEmail(EMAIL_INFO_STRU *pstEmailInfo, ATTACHMENT_ARRAY_STRU *pstAttach);


#ifdef __cplusplus
}
#endif

#endif
