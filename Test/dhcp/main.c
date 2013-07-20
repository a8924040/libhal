#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include "client.h"
#include "handler.h"
#include "udpipgen.h"

struct in_addr inform_ipaddr, default_router;
char *ProgramName = NULL;
char **ProgramEnviron = NULL;
char *IfNameExt = DEFAULT_IFNAME;
int IfNameExt_len = DEFAULT_IFNAME_LEN;
char *HostName = NULL;
int HostName_len = 0;
char *ClassID = NULL;
int ClassID_len = 0;
unsigned char *ClientID = NULL;
int ClientID_len = 0;
void *(*currState)() = &Reboot;
int DebugFlag = 1;
int BeRFC1541 = 0;
unsigned LeaseTime = DEFAULT_LEASETIME;
int SetHostName = 0;
int BroadcastResp = 0;
time_t TimeOut = DEFAULT_TIMEOUT;
int magic_cookie = 0;
unsigned short dhcpMsgSize = 0;
unsigned nleaseTime = 0;
int DoCheckSum = 0;
int SendSecondDiscover = 0;
int Persistent = 1;
int DownIfaceOnStop = 1;
int autoip_flag = 0;
int autoip_sleep = 300;
int SetDHCPDefaultRoutes = 1;
int RouteMetric = 1;
int Window = 0;


int daemon_init(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        return(-1);
    }

    else if (pid != 0)
    {
        exit(0);
    }

    setsid();
    chdir("/");
    umask(0);
    return(0);
}

int main(int argn, char *argc[], char *argv[])
{
    int s         =   1;
    int k         =   1;
    int i         =   1;
    int j;
    /*
     * Ensure that fds 0, 1, 2 are open, to /dev/null if nowhere else.
     * This way we can close 0, 1, 2 after forking the daemon without clobbering
     * a fd that we are using (such as our sockets). This is necessary if
     * this program is run from init scripts where 0, 1, and/or 2 may be closed.
     */
    j = open("/dev/null", O_RDWR);

    while (j < 2 && j >= 0)
    {
        j = dup(j);
    }

    if (j > 2)
    {
        close(j);
    }

    if (geteuid())
    {
        fprintf(stderr, "****  %s: not a superuser\n", argc[0]);
        exit(1);
    }

    while (argc[i])
        if (argc[i][0] == '-')
            switch (argc[i][s])
            {
                case 'd':
                    i++;
                    DebugFlag = 1;
                    continue;      //to see if there exists another parameter

                case 'g':
                    if (argc[i][s + 1])
                    {
                        goto usage;
                    }

                    SetDHCPDefaultRoutes = 0;
                    i++;

                    if (argc[i] && inet_aton(argc[i], &default_router))
                    {
                        i++;
                    }

                    else
                    {
                        memset(&default_router, 0, sizeof(default_router));
                    }

                    s = 1;
                    break;

                case 'i':
                    if (argc[i][s + 1])
                    {
                        goto usage;
                    }

                    i++;
                    IfNameExt = argc[i++];

                    if (IfNameExt == NULL || IfNameExt[0] == '-')
                    {
                        goto usage;
                    }

                    s = 1;

                    if ((IfNameExt_len = strlen(IfNameExt) + 1) <= 5)
                    {
                        printf("the interface name is %s\n", IfNameExt);
                        break;
                    }

                    printf("the interface name is %s\n", IfNameExt);
                    fprintf(stderr, "%s: too long Interface Name string: strlen=%d\n", argc[0], IfNameExt_len);
                    break;

                case 't':
                    if (argc[i][s + 1])
                    {
                        goto usage;
                    }

                    i++;

                    if (argc[i])
                    {
                        TimeOut = atol(argc[i++]);
                    }

                    if (DebugFlag == 1)
                    {
                        printf("Set Timeout = %ds\n", TimeOut);
                    }

                    else
                    {
                        goto usage;
                    }

                    s = 1;

                    if (TimeOut >= 0)
                    {
                        break;
                    }

                    goto usage;

                case 'T':
                    if (argc[i][s + 1])
                    {
                        goto usage;
                    }

                    i++;

                    if (argc[i])
                    {
                        autoip_sleep = atol(argc[i++]);
                    }

                    if (DebugFlag == 1)
                    {
                        printf("Set autoip_sleep = %ds\n", autoip_sleep);
                    }

                    else
                    {
                        goto usage;
                    }

                    s = 1;

                    if (autoip_sleep >= 0)
                    {
                        break;
                    }

                    goto usage;

                default:
usage:
                    fprintf(stderr,
                            "Usage: dhcpclient [-d(debug mode)] [-i interface name] [-t dhcp timeout] [-T AutoIP sleep time] [-g Gateway]\n");
                    exit(1);
            }

        else
        {
            goto usage;
        }

    ProgramName = argc[0];
    ProgramEnviron = argv;
    umask(022);
    signalSetup();
    magic_cookie = htonl(MAGIC_COOKIE);
    dhcpMsgSize = htons(sizeof(dhcpMessage) + sizeof(udpiphdr));
    nleaseTime = htonl(LeaseTime);

    if (TimeOut != 0)
    {
        alarm(TimeOut);
    }

    while ((currState != &Bound) && (autoip_flag == 0))
    {
        if ((currState = (*currState)()) == NULL)
        {
            exit(1);
        }
    }

    if (DebugFlag == 0)
    {
        if ((i = open("/dev/null", O_RDWR, 0)) >= 0)
        {
            (void)dup2(i, STDIN_FILENO);
            (void)dup2(i, STDOUT_FILENO);
            (void)dup2(i, STDERR_FILENO);

            if (i > 2)
            {
                (void)close(i);
            }
        }
    }

    daemon_init();

    if (autoip_flag == 1)
    {
        printf("autoip_flag == 1\n");
        currState = &Init;

        while (currState != &Bound)
            if ((currState = (*currState)()) == NULL)
            {
                exit(1);
            }
    }

    autoip_flag = 0;
    alarm(0);     //cancel the alarm set previously

    while (currState)
    {
        currState = (*currState)();
    }

    exit(1);
}




