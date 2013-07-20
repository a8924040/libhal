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

int CallForkProcess(char *pProcessPath, char **pArgv)
{
    int pid;
    if((pid = vfork()) < 0)
    {
        LIBHAL_ERROR("Fail to fork new process!");
        return -1;
    }
    else if(pid == 0)
    {
        LIBHAL_INFO("pid %d,ppid %d", getpid(), getppid());
        if(execvp(pProcessPath, pArgv) < 0)
        {
            LIBHAL_ALARM("Fail to exec the file");
            exit(127);
        }
    }
    LIBHAL_INFO("Child pid %d", pid);
    return 0;
}

int  main(int argc, char **argv)
{
    //int i;
    //char str[32][64];
    char *pArgv[30] =
    {
        "wpa_supplicant",
        "-B",
        "-iwlan0",
        "-c/etc/wpa_supplicant.conf",
        "-Dwext",
        NULL,
    };
    CallForkProcess("wpa_supplicant", pArgv);
    return 0;
}

