#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /*for struct sockaddr_in*/

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>

#define DEST_IP   "192.168.2.22"
#define DEST_PORT 4000
#define TEST_LENG  ((60*60*14)/2)
int main()
{
    int res;
    int sockfd;
    int i;
    struct sockaddr_in dest_addr;
    char msg[1024];
    int len, bytes_sent;
    memset(msg, 0x5a, 1024);
    /* ȡ��һ���׽���*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket()");
        exit(1);
    }
    /* ����Զ�����ӵ���Ϣ*/
    dest_addr.sin_family = AF_INET;                 /* ע�������ֽ�˳��*/
    dest_addr.sin_port = htons(DEST_PORT);          /* Զ�����Ӷ˿�, ע�������ֽ�˳��*/
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);    /* Զ�� IP ��ַ, inet_addr() �᷵�������ֽ�˳��*/
    bzero(& (dest_addr.sin_zero), 8);               /* ����ṹ���� 0*/
    /* ����Զ�������������� -1*/
    res = connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr_in));
    if(res == -1)
    {
        perror("connect()");
        exit(1);
    }
    for(i = 0; i <= 1024 * TEST_LENG; i++)
    {
        len = 1024;
        bytes_sent = send(sockfd,   /* ����������*/
                          msg,    /* ��������*/
                          len,    /* �������ݳ���*/
                          0);     /* ���ͱ��, һ���� 0*/
        // printf("send byte is %d i=%d\n",bytes_sent,i);
        if(i == (1024 * TEST_LENG - 1))
        {
            memset(msg, 0, sizeof(msg));
            strncpy(msg, "exit", 10);
        }
    }
    /* �ر�����*/
    close(sockfd);
    return 0;
}

