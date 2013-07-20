/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved                              

 ******************************************************************************
  �� �� ��   : smtp.c
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : ʹ��smtp ���ʼ� �ɰ�������
  ����޸�   :
              CreatSocket
              GetSmtpServerIp
              SendEmail
              smtprev
  ��������   : 
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��3��1��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <Wifi.h>
#include <System.h>
#include "NetWork.h"
#include "Smtp.h"

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/
extern char *base64_encode(const  char *value, int vlen);

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
int open_socket(struct sockaddr *addr);

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
    
#define MAX_PATH 128
#define ATTA_PATH_MAX   128
#define ATTA_NAME_MAX   128

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define BOUNDARY "#BOUNDARY#"

/*
*	ʹ�ù���
*	���Է��ʹ��������ʼ���������С�����ɷ��ͷ���������������δ���Ծ���Ϊ����MB
*	���������Ƕ��
*	������֤��Ϊbase64����
*	�ʼ����ĺ͸��������ݴ��ͷ�ʽ����Ϊbase64
*/


/*SMTP�������������ȷ������������ֱ�ӷ���-1�������˳�*/
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
		case 221:printf("������--%d������ر��˴���ͨ����\n",revcode);return -1;break;
		case 251:printf("������--%d���ռ��˷Ǳ����û�����ת���� <forward-path>��\n",revcode);return -1;break;
		case 421:printf("������--%d���������������⣬�޷��ṩ�������񣬱���رմ���ܵ���\n",revcode);return -1;break;
		case 450:printf("������--%d�����䲻���ã�������æ���ܾ��������޷�ִ�У�\n",revcode);return -1;break;
		case 451:printf("������--%d�����ض˷������󣬵���ϵͳͶ��ʱ�������⣬�����жϣ�\n",revcode);return -1;break;
		case 452:printf("������--%d�������޶�������ϵͳ�ռ䲻�㣬�����޷�ִ�У�\n",revcode);return -1;break;
		case 500:printf("������--%d��������̫�����������ʽ���󣬲���ʶ��\n",revcode);return -1;break;
		case 501:printf("������--%d�������������\n",revcode);return -1;break;
		case 502:printf("������--%d��������δʵ�֣�\n",revcode);return -1;break;
		case 503:printf("������--%d�����������˳��\n",revcode);return -1;break;
		case 504:printf("������--%d������Ĳ�����δʵ�֣�\n",revcode);return -1;break;
		case 535:printf("������--%d�������½��֤ʧ�ܣ������������\n",revcode);return -1;break;
		case 550:printf("������--%d���û������ڣ����û�����������Ҫ�����޷�ִ�У�\n",revcode);return -1;break;
		case 551:printf("������--%d���ռ��˲����ڱ����û���ת����<forward-path>��\n",revcode);return -1;break;
		case 552:printf("������--%d������������Ĵ���ռ䣬�����жϣ�\n",revcode);return -1;break;
		case 553:printf("������--%d�������ܸ����䣬����δִ�У�ԭ��������ʼ���ַ���󡢱���Ϊ�����ʼ����գ�\n",revcode);return -1;break;
		case 554:printf("������--%d������ʧ�ܣ�\n",revcode);return -1;break;
		default:break;
	};
	return 0;
}

/*****************************************************************************
 �� �� ��  : GetSmtpServerIp
 ��������  : ��ȡsmtp ��������ip
 �������  : S8 *ps8SmtpAccount   
             S8 *ps8SmtpServerIp  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��3��1��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : CreatSocket
 ��������  : ����socket
 �������  : struct sockaddr *addr  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��3��1��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
		/* �ʼ��ռ���Ϊ��*/
		LIBHAL_ALARM("SMTP EmailTo IS NULL!");
		return -1;
	}
	if( NULL == pstEmailInfo->ps8EmailFrom )
	{
		/* �ʼ�������Ϊ�� */
		LIBHAL_ALARM("SMTP EmailFrom IS NULL!");
		return -1;
	}
	if(NULL == pstEmailInfo->ps8EmailPwd)
	{
		/* ����Ϊ�� */
		LIBHAL_ALARM("SMTP EmailPwd IS NULL!");
		return -1;
	}
	
    GetSmtpServerIp(pstEmailInfo->ps8EmailFrom, s8SmtpServerIp);

	memset(&stSmtpServer, 0, sizeof(stSmtpServer));
	stSmtpServer.sin_family = AF_INET;
	stSmtpServer.sin_port = htons(25);
	stSmtpServer.sin_addr.s_addr = inet_addr(s8SmtpServerIp);
	/* �����ʼ�������*/
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

	/* RCPT TO �ռ��� */
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
	/* DATA ׼����ʼ�����ʼ����� */
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
	
	/* �����ʼ�������Ϣͷ */
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
	
	
	/* �����ʼ�����*/
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
	
	
	/* ����\r\n*/
	send(sockfd, "\r\n\r\n--", strlen("\r\n\r\n--"), 0);
	/* ���͵�һ���ָ���*/
	send(sockfd,BOUNDARY,strlen(BOUNDARY), 0);
	
	/*�ж��Ƿ��и���*/
	if(  NULL == pstAttach || pstAttach->u32AttachSum == 0)
	{
		send(sockfd, "--", strlen("--"), 0);
	}
	else
	{
		/*˳����ļ������ֶη��ͳ�ȥ*/
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
			/*�����ļ���������*/
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
			
			/*����ÿ����������Ϣ�ļ���β*/
			/* ����\r\n*/
			send(sockfd, "\r\n\r\n--", strlen("\r\n\r\n--"), 0);
			/* ���͵�һ���ָ���*/
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

	/* �������ݷ�����ϣ�����\r\n.\r\n������� */
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

