#include <CommonDef.h>
#include <CommonInclude.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int  bbtest_main(int argc, char *argv[])
{
    int i;
    for(i = 0; i < 100 ; i ++)
    {
        if(i == 0)
        {
            printf("%3d", i);
            continue;
        }
        if(i % 10 == 0)
        {
            printf("\n");
        }
        printf("%3d", i);
    }
    return 0;
}

