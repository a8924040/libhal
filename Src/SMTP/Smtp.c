/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved                              

 ******************************************************************************
  文 件 名   : smtp.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : 使用smtp 发邮件 可包含附件
  最近修改   :
              CreatSocket
              GetSmtpServerIp
              SendEmail
              smtprev
  功能描述   : 
  函数列表   :
  修改历史   :
  1.日    期   : 2013年3月1日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <Wifi.h>
#include <System.h>
#include "NetWork.h"
#include "Smtp.h"

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern char *base64_encode(const  char *value, int vlen);

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
int open_socket(struct sockaddr *addr);

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
    
#define MAX_PATH 128
#define ATTA_PATH_MAX   128
#define ATTA_NAME_MAX   128

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define BOUNDARY "#BOUNDARY#"

/*
*	使用规则：
*	可以发送带附件的邮件，附件大小限制由发送方服务器而定，暂未测试具体为多少MB
*	附件可以是多个
*	密码验证均为base64加密
*	邮件正文和附件的数据传送方式，均为base64
*/


/*SMTP返回码分析，正确不操作，错误直接返回-1，程序即退出*/
int smtprev(char *str)
{
	int revcode=0;
	if(strlen(str)<3)
    {
		return -1;
    }
    LIBHAL_ALARM("the str is %s",str);
    revcode = atoi(str);
    LIBHAL_ALARM("The revcode is %d", revcode);
	switch(revcode)
	{
		case 221:printf("错误码--%d，服务关闭了传输通道！\n",revcode);return -1;break;
		case 251:printf("错误码--%d，收件人非本地用户，将转发到 <forward-path>！\n",revcode);return -1;break;
		case 421:printf("错误码--%d，服务器遇到问题，无法提供正常服务，必须关闭传输管道！\n",revcode);return -1;break;
		case 450:printf("错误码--%d，邮箱不可用，或邮箱忙被拒绝，动作无法执行！\n",revcode);return -1;break;
		case 451:printf("错误码--%d，本地端发生错误，导致系统投递时遇到意外，动作中断！\n",revcode);return -1;break;
		case 452:printf("错误码--%d，邮箱限额满，或系统空间不足，动作无法执行！\n",revcode);return -1;break;
		case 500:printf("错误码--%d，命令行太长，或命令格式错误，不可识别！\n",revcode);return -1;break;
		case 501:printf("错误码--%d，命令参数错误！\n",revcode);return -1;break;
		case 502:printf("错误码--%d，命令尚未实现！\n",revcode);return -1;break;
		case 503:printf("错误码--%d，错误的命令顺序！\n",revcode);return -1;break;
		case 504:printf("错误码--%d，命令的参数尚未实现！\n",revcode);return -1;break;
		case 535:printf("错误码--%d，邮箱登陆验证失败，可能是密码错！\n",revcode);return -1;break;
		case 550:printf("错误码--%d，用户不存在，或用户被锁定，所要求动作无法执行！\n",revcode);return -1;break;
		case 551:printf("错误码--%d，收件人不属于本地用户，转发到<forward-path>！\n",revcode);return -1;break;
		case 552:printf("错误码--%d，超出所分配的储存空间，动作中断！\n",revcode);return -1;break;
		case 553:printf("错误码--%d，不接受该信箱，动作未执行，原因可能是邮件地址错误、被作为垃圾邮件拒收！\n",revcode);return -1;break;
		case 554:printf("错误码--%d，传输失败！\n",revcode);return -1;break;
		default:break;
	};
	return 0;
}

/*****************************************************************************
 函 数 名  : GetSmtpServerIp
 功能描述  : 获取smtp 服务器的ip
 输入参数  : S8 *ps8SmtpAccount   
             S8 *ps8SmtpServerIp  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月1日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GetSmtpServerIp(S8 *ps8SmtpAccount,S8 *ps8SmtpServerIp)
{
    S8 as8Temp[32]={0};
    S8 as8Url[30]= {"smtp."};
    S8 as8UrlIp[30]= {0};
    S8 *ps8Str = NULL;
    
    if ( NULL == ps8SmtpAccount || NULL == ps8SmtpServerIp)
    {
        LIBHAL_ERROR("GetSmtpServerIp PARAM ERROR!");
        return -1;
    }
    
	memset(as8Temp,0x00,sizeof(as8Temp));
    ps8Str = strstr(ps8SmtpAccount, "@");
    ps8Str ++;
	strcpy(as8Temp, ps8Str);
    strcat(as8Url, as8Temp);
    LIBHAL_ALARM("the smtp server url is  %s", as8Url);
    NetWorkGetIpFromUrl(as8Url, as8UrlIp);
    LIBHAL_ALARM("the smtp server ip is  %s", as8UrlIp);
    sprintf(ps8SmtpServerIp, "%s", as8UrlIp);
    return 0;
}


/*****************************************************************************
 函 数 名  : CreatSocket
 功能描述  : 创建socket
 输入参数  : struct sockaddr *addr  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月1日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int CreatSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd=socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		LIBHAL_ERROR("creat socket <tcp> failed");
        perror("");
        return -1;
	}
	if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
	{
		LIBHAL_ERROR("connect socket <tcp> failed\n");
        perror("");
		return -1;
	}
	
	return sockfd;
}

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
int SendEmail(EMAIL_INFO_STRU *pstEmailInfo, ATTACHMENT_ARRAY_STRU *pstAttach)
{
    if (NULL == pstEmailInfo)
    {
        LIBHAL_ERROR("SendEmail Param error!");
        return -1;
    }

    int sockfd = 0,i=0,filelen=0,nowlen=0,readlen=0,rec=0,sizesum = 0;
    U32 u32Count = 0;
    U32 u32BoundSum = 0;
    double percent = 0.0;
    
	double dp=0;
	struct sockaddr_in stSmtpServer;	
    S8 s8SmtpServerIp[32]={0};

	char buf[1500] = {0};
	char filebuf[240+1];
	char rbuf[1500] = {0};
	char login[128] = {0};
	char pass[128] = {0};
	char sendbuf[1500]={0};
	char filebase64[400];
	char tmpstr[50];
	FILE *fp = NULL;

	if(NULL == pstEmailInfo->ps8EmailTo)
	{
		/* 邮件收件人为空*/
		LIBHAL_ALARM("SMTP EmailTo IS NULL!");
		return -1;
	}
	if( NULL == pstEmailInfo->ps8EmailFrom )
	{
		/* 邮件发件人为空 */
		LIBHAL_ALARM("SMTP EmailFrom IS NULL!");
		return -1;
	}
	if(NULL == pstEmailInfo->ps8EmailPwd)
	{
		/* 密码为空 */
		LIBHAL_ALARM("SMTP EmailPwd IS NULL!");
		return -1;
	}
	
    GetSmtpServerIp(pstEmailInfo->ps8EmailFrom, s8SmtpServerIp);

	memset(&stSmtpServer, 0, sizeof(stSmtpServer));
	stSmtpServer.sin_family = AF_INET;
	stSmtpServer.sin_port = htons(25);
	stSmtpServer.sin_addr.s_addr = inet_addr(s8SmtpServerIp);
	/* 连接邮件服务器*/
	sockfd = CreatSocket((struct sockaddr *)&stSmtpServer);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	
	LIBHAL_ALARM("\nSMTP SERVER RESPONSE:\n\t%s", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}

	/* EHLO */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "EHLO %s\r\n", pstEmailInfo->ps8EmailAccount);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);	
	LIBHAL_ALARM("EHLO Response:\n%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}

	/* AUTH LOGIN */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	LIBHAL_ALARM("AUTH LOGIN Response:\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}


	/* USER */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf,"%s",pstEmailInfo->ps8EmailFrom);
	memset(login,0x00,sizeof(login));
	strcpy(login,base64_encode(buf,strlen(buf)));
	sprintf(buf, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("USER Response:\t\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}


	/* PASSWORD */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "%s",pstEmailInfo->ps8EmailPwd);
	memset(pass,0x00,sizeof(pass));
	strcpy(pass,base64_encode(buf,strlen(buf)));
	sprintf(buf, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("PASSWORD Response:\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}


	/* MAIL FROM */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "MAIL FROM:<%s>\r\n",pstEmailInfo->ps8EmailFrom);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("MAIL FROM Response:\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}

	/* RCPT TO 收件人 */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "RCPT TO:<%s>\r\n",pstEmailInfo->ps8EmailTo);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("RCPT TO Response:\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}
	/* DATA 准备开始发送邮件内容 */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("Pre-DATA Response:\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}
	
	/* 发送邮件公共信息头 */
	memset(sendbuf,0x00,sizeof(sendbuf));
	sprintf(sendbuf,"From: <%s>\r\nTo: %s\r\nSubject: %s\r\nMIMI-Version:1.0\r\n"
	        "Content-Type: multipart/mixed;boundary=\"%s\"\r\n"
	        "Content-Transfer-Encoding: base64\r\n\r\n--%s\r\n"
	        "Content-Type: text/plain;charset=\"gb18030\"\r\n"
	        "Content-Transfer-Encoding: base64\r\n\r\n"
	        ,pstEmailInfo->ps8EmailFrom,pstEmailInfo->ps8EmailTo
	        ,pstEmailInfo->ps8EmailTitle,BOUNDARY,BOUNDARY);
	
	/*printf("sendbuf:\t%s\n", sendbuf);  */
	send(sockfd, sendbuf, strlen(sendbuf), 0);
	
	
	/* 发送邮件内容*/
	nowlen=0;
	filelen=strlen(pstEmailInfo->ps8EmailBody);
	while(1)
	{
		if((filelen-nowlen)>=240)
		{
			memset(filebase64,0x00,sizeof(filebase64));
			strcpy(filebase64,base64_encode(pstEmailInfo->ps8EmailBody,240));
			nowlen+=240;
			send(sockfd, filebase64, strlen(filebase64), 0);
		}
		else
		{
			memset(filebase64,0x00,sizeof(filebase64));
			strcpy(filebase64,base64_encode(pstEmailInfo->ps8EmailBody,(filelen-nowlen)));
			send(sockfd, filebase64, strlen(filebase64), 0);
			break;
		}
	}
	
	
	/* 发送\r\n*/
	send(sockfd, "\r\n\r\n--", strlen("\r\n\r\n--"), 0);
	/* 发送第一个分隔符*/
	send(sockfd,BOUNDARY,strlen(BOUNDARY), 0);
	
	/*判断是否有附件*/
	if(  NULL == pstAttach || pstAttach->u32AttachSum == 0)
	{
		send(sockfd, "--", strlen("--"), 0);
	}
	else
	{
		/*顺序读文件，并分段发送出去*/
		for(i=0; i < pstAttach->u32AttachSum; i++)
		{
			memset(sendbuf,0x00,sizeof(sendbuf));
			sprintf(sendbuf, "\r\nContent-Type: application/octet-stream;"
			        "charset=\"gb18030\";name=\"%s\"\r\n"
			        "Content-Disposition: attachment; filename=\"%s\"\r\n"
			        "Content-Transfer-Encoding: base64\r\n\r\n"
			        ,pstAttach->stAttachment[i].ps8FileName
			        ,pstAttach->stAttachment[i].ps8FileName);
			        
			send(sockfd, sendbuf, strlen(sendbuf), 0);
			/*发送文件主体内容*/
			/*Coding begin*/
			fp=NULL;
			fp=fopen(pstAttach->stAttachment[i].ps8FilePath,"rb");
			if(fp==NULL)
			{
				LIBHAL_ERROR("Attach File [%s] open Err!\n"
				             ,pstAttach->stAttachment[i].ps8FilePath);
				continue;
			}
			fseek(fp,0,SEEK_END);
			sizesum=ftell(fp);
			//nowread=0;
			memset(sendbuf,0x00,sizeof(sendbuf));
			memset(tmpstr,0x00,sizeof(tmpstr));
			dp=(double)sizesum/1024;
			sprintf(tmpstr,"The Attachmen is %5.2f KB",dp);
			LIBHAL_WARN("%s",tmpstr);
			
			fseek(fp,0,SEEK_SET);
			
			u32BoundSum = sizesum/57;
            u32Count = 1;

			while(!feof(fp))
			{
				memset(filebuf,0x00,sizeof(filebuf));
				readlen=fread(filebuf,1,57,fp);
				if(readlen<0)
				{
					LIBHAL_ERROR("Attach File [%s] read Err!\n"
					              ,pstAttach->stAttachment[i].ps8FileName);
					fclose(fp);
					break;
				}		
				memset(filebase64,0x00,sizeof(filebase64));
				strcpy(filebase64,base64_encode(filebuf,readlen));
				strcat(filebase64,"\r\n");
				send(sockfd,filebase64,strlen(filebase64),0);
				percent = (double)((double)u32Count/(double)u32BoundSum)*100.0;
				if ( percent > 100.00)
				{
				    percent = 100.00;
				}
				if ( (u32Count % (u32BoundSum/10)) == 0)
                LIBHAL_WARN("sending %d of %d attachment%0.1f%%"
                        ,i+1,pstAttach->u32AttachSum, percent );
                u32Count++;
				//nowread+=readlen;
			}
			fclose(fp);
            LIBHAL_WARN("sending %d of %d attachment100.0%%",i+1,pstAttach->u32AttachSum );
			/*Coding End*/
			
			/*发送每个附件的信息文件结尾*/
			/* 发送\r\n*/
			send(sockfd, "\r\n\r\n--", strlen("\r\n\r\n--"), 0);
			/* 发送第一个分隔符*/
			send(sockfd,BOUNDARY,strlen(BOUNDARY), 0);
			if( (i+1) == pstAttach->u32AttachSum )
			{
				send(sockfd, "--", strlen("--"), 0);
			}
			sleep(2);
            LIBHAL_ALARM("END Send %d of %d ATTACHMENT",i+1,pstAttach->u32AttachSum );
		}
	}

    send(sockfd, "\r\n\r\n--", strlen("\r\n\r\n--"), 0);

	/* 所有内容发送完毕，发送\r\n.\r\n结束标记 */
	memset(sendbuf,0x00,sizeof(sendbuf));
	strcpy(sendbuf, "\r\n.\r\n");
	send(sockfd, sendbuf, strlen(sendbuf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("Rel-DATA Response:\t%s\n", rbuf);
	rec=smtprev(rbuf);
	if(rec!=0)
	{
		return -1;
	}

	/* QUIT */
	memset(buf,0x00,sizeof(buf));
	sprintf(buf, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf,0x00,sizeof(rbuf));
	recv(sockfd, rbuf, sizeof(rbuf), 0);
	printf("QUIT Response:\t\t%s\n", rbuf);
	close(sockfd);

	return 0;
}

