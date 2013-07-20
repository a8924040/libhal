#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>

 
#define DNS_RESOLV_FILE_PATH "/etc/resolv.conf" 
#define DNS_TIMEOUT_MAX 5
#define DNS_HOST  0x01
#define DNS_CNAME 0x05
#define DNS_SERVER_PORT 53 


static int is_pointer(int in)
{
	return ((in & 0xc0) == 0xc0);
}

S32 GetDnsServerIP(char (*pDnsIP)[32])
{
    FILE *fp = NULL;
    char *pSpace = NULL;
    S8 as8DnsBuf[256] = {0};
    S32 s32DnsServerIPCnt = 0;
    
    if (NULL == (fp = fopen(DNS_RESOLV_FILE_PATH,"r")))
    {
        printf("open %s failed\n",DNS_RESOLV_FILE_PATH);
        return -1;
    }
    
    while (NULL != fgets(as8DnsBuf,sizeof(as8DnsBuf),fp))
    {
    
        pSpace = as8DnsBuf;
        if (*pSpace == '\n')
        {
            continue;
        }
        
        while(1)
        {
           if (isdigit(*pSpace) || ispunct(*pSpace))
           {
             sprintf(*pDnsIP,"%s",pSpace);
             /* 去掉行未的回车符 */
             *(*pDnsIP+(strlen(*pDnsIP)-1)) = '\0';
             s32DnsServerIPCnt++;
             pDnsIP++;
             break; 
           }
           pSpace++;
        }
    }
    return  s32DnsServerIPCnt;
}

void ParaseDnsName(unsigned char *chunk, unsigned char *ptr , char *out , int *len)
{
	int n ,  flag;
	char *pos = out + (*len);
 
	for(;;)
	{
		flag = (int)ptr[0];
		if(flag == 0)
			break;
		if(is_pointer(flag))
		{
			n = (int)ptr[1];
			ptr = chunk + n;
			ParaseDnsName(chunk , ptr , out , len);
			break;
		}
		else
		{
			ptr ++;
			memcpy(pos , ptr , flag);	
			pos += flag;
			ptr += flag;
			*len += flag;
			if((int)ptr[0] != 0)
			{
				memcpy(pos , "." , 1);
				pos += 1;
				(*len) += 1;
			}
		}
	}
 
}


S32 ParseDnsResponse(S32 s32SocketFd,S8 *ps8UrlIP)
{
 
	unsigned char buf[1024];
	unsigned char *ptr = buf;
	struct sockaddr_in addr;
	int n , i , flag , querys , answers;
	int type , ttl , datalen , len;
	char cname[128] , aname[128] , ip[20] ;
	unsigned char netip[4];
	size_t addr_len = sizeof(struct sockaddr_in);

	/* set recvfrom from server timeout */
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = DNS_TIMEOUT_MAX;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(s32SocketFd, &readfds);
    if (select(s32SocketFd+1,&readfds,NULL, NULL, &tv) > 0)
    {
        n = recvfrom(s32SocketFd , buf , sizeof(buf) , 0, (struct sockaddr*)&addr , &addr_len);
        if ( n < 0)
        {
            perror("dns recv");
            return -1;
        }

    }
    else
    {
        LIBHAL_WARN("Timeout!there is no data arrived!\n");
        return -1;
    }
 

	ptr += 4; /* move ptr to Questions */
	querys = ntohs(*((unsigned short*)ptr));
	ptr += 2; /* move ptr to Answer RRs */
	answers = ntohs(*((unsigned short*)ptr));
	ptr += 6; /* move ptr to Querys */
	/* move over Querys */
	for(i= 0 ; i < querys ; i ++)
	{
		for(;;)
		{
			flag = (int)ptr[0];
			ptr += (flag + 1);
			if(flag == 0)
			{
				break;
			}
		}
		ptr += 4;
	}
	/* now ptr points to Answers */
	for(i = 0 ; i < answers ; i ++)
	{
		bzero(aname , sizeof(aname));
		len = 0;
		ParaseDnsName(buf , ptr , aname , &len);
		ptr += 2; /* move ptr to Type*/
		type = htons(*((unsigned short*)ptr));
		ptr += 4; /* move ptr to Time to live */
		ttl = htonl(*((unsigned int*)ptr));
		ptr += 4; /* move ptr to Data lenth */
		datalen = ntohs(*((unsigned short*)ptr));
		ptr += 2; /* move ptr to Data*/
		if(type == DNS_CNAME)
		{
			bzero(cname , sizeof(cname));
			len = 0;
			ParaseDnsName(buf , ptr , cname , &len);
#ifdef DNS_DEEBUG
            printf("-------------------------------\n");
			printf("%s is an alias for %s\n" , aname , cname);
#endif
			ptr += datalen;
		}
		
		if(type == DNS_HOST)
		{
			bzero(ip , sizeof(ip));
			if(datalen == 4)
			{
				memcpy(netip , ptr , datalen);
				inet_ntop(AF_INET , netip , ip , sizeof(struct sockaddr));
				sprintf(ps8UrlIP,"%s",ip);
				
#ifdef DNS_DEEBUG		
                printf("%s has address %s\n" , aname , ip);
				printf("\tTime to live: %d minutes , %d seconds\n", ttl / 60 , ttl % 60);
#endif			
			}
			ptr += datalen;
		}
 
	}
	ptr += 2;
    return 0;
}
 


static void GenerateDnsRequest(const char *dns_name , unsigned char *buf , int *len)
{
	char *pos;
	unsigned char *ptr;
	int n;
 
	*len = 0;
	ptr = buf;	
	pos = (char*)dns_name; 
	for(;;)
	{
		n = strlen(pos) - (strstr(pos , ".") ? strlen(strstr(pos , ".")) : 0);
		*ptr ++ = (unsigned char)n;
		memcpy(ptr , pos , n);
		*len += n + 1;
		ptr += n;
		if(!strstr(pos , "."))
		{
			*ptr = (unsigned char)0;
			ptr ++;
			*len += 1;
			break;
		}
		pos += n + 1;
	}
	*((unsigned short*)ptr) = htons(1);
	*len += 2;
	ptr += 2;
	*((unsigned short*)ptr) = htons(1);
	*len += 2;
}
 
int SendDnsRequest(const char *dns_name,S32 s32SocketFd,struct sockaddr_in stDest)
{
 
	unsigned char request[256];
	unsigned char *ptr = request;
	unsigned char question[128];
	int question_len;
 
 
	GenerateDnsRequest(dns_name , question , &question_len);
 
	*((unsigned short*)ptr) = htons(0xff00);
	ptr += 2;
	*((unsigned short*)ptr) = htons(0x0100);
	ptr += 2;
	*((unsigned short*)ptr) = htons(1);
	ptr += 2;
	*((unsigned short*)ptr) = 0;
	ptr += 2;
	*((unsigned short*)ptr) = 0;
	ptr += 2;
	*((unsigned short*)ptr) = 0;
	ptr += 2;
	memcpy(ptr , question , question_len);
	ptr += question_len;
 
	if ( sendto(s32SocketFd , request , question_len + 12 , 0
	            , (struct sockaddr*)&stDest , sizeof(struct sockaddr)) < 0)
	{
	    LIBHAL_ERROR("send data error\n");
	    perror("dns");
	    return -1;
	}
	return 0;
}

S32 GetUrlIPFromDns(S8 *ps8DomainName, S8 *ps8UrlIP)
{
    S32 s32SocketFd = 0;
    struct sockaddr_in stDest;
    S32 s32Ret = -1;
    S8 au8DnsIPAary[10][32];
    S32 s32DnsIPSum = 0;
    S32 s32Index = 0;
    
    if(NULL == ps8DomainName || NULL == ps8UrlIP) 
    {
       LIBHAL_ERROR("GetUrlIPFromDns param error!");
    }

    
	s32DnsIPSum = GetDnsServerIP(au8DnsIPAary);

	LIBHAL_INFO("Get DNS server ip from %s is %d\n",DNS_RESOLV_FILE_PATH, s32DnsIPSum);

    s32SocketFd = socket(AF_INET , SOCK_DGRAM , 0);
    if(s32SocketFd < 0)
    {
        perror("create socket failed");
        return -1;
    }
    /* 采用非阻塞模式 */
    if ( fcntl(s32SocketFd, F_SETFL, O_NONBLOCK) < 0)
    {
        printf("fcntl socket error!\n");
        return -1;
    }

  	for (s32Index = 0; s32Index < s32DnsIPSum; s32Index++)
	{
        LIBHAL_INFO("%s\n",au8DnsIPAary[s32Index]);      
        bzero(&stDest , sizeof(stDest));
        
        stDest.sin_family = AF_INET;
        stDest.sin_port = htons(DNS_SERVER_PORT);
        stDest.sin_addr.s_addr = inet_addr(au8DnsIPAary[s32Index]);
     
        s32Ret = SendDnsRequest(ps8DomainName,s32SocketFd,stDest);
        s32Ret = ParseDnsResponse(s32SocketFd, ps8UrlIP);
        if (s32Ret < 0)
        {
            if (s32Index+1 == s32DnsIPSum)
            {
                goto fail;
            }
            else
            {
                continue;
            }
        }
        else
        {
            break;
        }
	}
	
	close(s32SocketFd);
 
	return 0;
fail:
	close(s32SocketFd);
	return -1;
}

int main(int argc, char *argv[])
{
    S8 s8UrlIP[32] = { 0 };
    int i ;
    printf("%s,%s\n",__DATE__,__TIME__);
    for ( i = 0; i < 100; i++)
    {
        GetUrlIPFromDns("ztg328.oicp.net", s8UrlIP);
        LIBHAL_INFO("Url IP:%s\n", s8UrlIP);        
        memset(s8UrlIP,0,sizeof(s8UrlIP));
        sleep(1);
    }
#if 0
    char url[32] = "ztg328.oicp.net";
    NetWorkGetIpFromUrl(url, urlip);
    LIBHAL_DEBUG("%s url ip is %s\n", url, urlip);

    if(CheckIpConflic("wlan0",  "192.168.2.21") < 0)
    {
        LIBHAL_ERROR("this ip is already be used someone!");
    }

#endif

    //NetWorkSetGateWay("wlan0", "0.0.0.0");
    //NetWorkSetDefault("wlan0");
    return 0;
}
