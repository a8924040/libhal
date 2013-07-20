/* socksrv.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* for struct sockaddr_in*/

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#define BACKLOG 10
#define MYPORT  4000

int main()
{
    int sockfd;
    int new_fd;
    time_t time_begin = 0;
    struct sockaddr_in my_addr, their_addr;
    int res;
    int sin_size;
    char *buf;
    int on = 1;
    unsigned long long receive_sum = 0;
    unsigned long long i_receive = 0;
    /* 取得套接字描述符*/
    sockfd = socket(AF_INET,     /* domain*/
                    SOCK_STREAM, /* type*/
                    0);          /* protocol*/
    if(sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    FILE *fp_record = NULL;
    if((fp_record = fopen("/mnt/record.txt", "w+")) == NULL)
    {
        printf("opem record.txt failed \n");
    }
    rewind(fp_record);
    /* Init sockaddr_in */
    my_addr.sin_family = AF_INET;                /* 注意: 应使用主机字节顺序*/
    my_addr.sin_port = htons(MYPORT);            /* 注意: 应使用网络字节顺序*/
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* 使用自己的 IP 地址 */
    bzero(& (my_addr.sin_zero), 8);              /* 结构的其余的部分须置 0*/
    /* 指定一个套接字使用的地址及端口*/
    res = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));
    if(res == -1)
    {
        perror("bind");
        exit(1);
    }
    /* 监听请求, 等待连接*/
    res = listen(sockfd,
                 BACKLOG);  /* 未经处理的连接请求队列可容纳的最大数目*/
    if(res == -1)
    {
        perror("listen");
        exit(1);
    }
    /* 接受对方的连接请求, 建立连接，返回一个新的连接描述符.
     * 而第一个套接字描述符仍在你的机器上原来的端口 listen()
     */
    sin_size = sizeof(struct sockaddr_in);
    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, (socklen_t *) &sin_size);
    buf = (char *) malloc(1024);
    if(buf == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }
    char fprintf_buf[128] = {0  };
    /* 接受对方发来的数据*/
    while(1)
    {
        res = recv(new_fd, buf, 1024, 0);
        if(res == -1)
        {
            perror("recv()");
            exit(1);
        }
        if(res == 0)
        {
            continue;
        }
        i_receive ++;
        if(i_receive == (unsigned long long) 1)
        {
            time_begin = time(NULL);
            printf("begint receive time is %u\n", (unsigned int) time_begin);
        }
        //printf("recv dataxxxxxxxx:%d,%x,i_receive = %d\n", res,buf[0],i_receive);
        receive_sum += res;
        //receive_sum = (unsigned long long)((unsigned long long)1024*1024*60*60*6/2);
        if(strstr(buf, "exit") != NULL)
        {
            printf("end receive time is %u\n", (unsigned int) time(NULL));
            sprintf(fprintf_buf, "receive exit cmd exit,i_receive_sum=%lldm,spend time is %u\n", (receive_sum / 1024 / 1024), (unsigned int)(time(NULL) - time_begin));
            fwrite(fprintf_buf, strlen(fprintf_buf), 1, fp_record);
            fflush(fp_record);
            fclose(fp_record);
            /* 关闭本次连接*/
            close(new_fd);
            /* 关闭系统监听*/
            close(sockfd);
            exit(1);
        }
        memset(buf, 0, sizeof(buf));
    }
    free(buf);
    return 0;
}

