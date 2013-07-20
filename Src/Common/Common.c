/*
**  ************************************************************************
**
**  File    Name  : common.c
**  Description   : ͨ�ú���
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/

/*
**  ************************************************************************
**                              ͷ�ļ�
**  ************************************************************************
*/
#include "CommonInclude.h"
#include "Common.h"
#include "Log.h"
/* �����ȡlinux �汾·��  */
#define LINUX_KERNEL_VERSION    "/proc/version"
#define KERNEL_VERSION          "Linux version"

/*
**  ************************************************************************
**                             ��������
**  ************************************************************************
*/
/* $Function        :    RunSystemCommand(const char *command)
==   ===============================================================
==  Description     :   ģ��system���
==  Argument        :   ���������
==  Return          :   1�� Ϊ����Ϊ��  -1�� ����ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu     ����
==   ===============================================================
*/
int  RunSystemCommand(const char *command)
{
    pid_t pid;
    int status = 0;
    struct sigaction ignore, saveintr, savequit;
    sigset_t childmask, savemask;
    if(command == NULL)
    { return (1); }
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;
    if(sigaction(SIGINT, &ignore, &saveintr) < 0)
    { return (-1); }
    if(sigaction(SIGQUIT, &ignore, &savequit) < 0)
    { return (-1); }
    sigemptyset(&childmask);
    sigaddset(&childmask, SIGCHLD);
    if(sigprocmask(SIG_BLOCK, &childmask, &savemask) < 0)
    { return (-1); }
    if((pid = fork()) < 0)
    {
        status = -1;
    }
    else if(pid == 0)           /* child */
    {
        sigaction(SIGINT, &saveintr, NULL);
        sigaction(SIGQUIT, &savequit, NULL);
        sigprocmask(SIG_SETMASK, &savemask, NULL);
        execl("/bin/sh", "sh", "-c", command, (char *) 0);
        _exit(127);
    }
    else
    {
        while(waitpid(pid, &status, 0) < 0)
            if(errno != EINTR)
            {
                status = -1;
                break;
            }
    }
    if(sigaction(SIGINT, &saveintr, NULL) < 0)
    { return (-1); }
    if(sigaction(SIGQUIT, &savequit, NULL) < 0)
    { return (-1); }
    if(sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
    { return (-1); }
    return (status);
}



/*
**  ************************************************************************
**                             ��������
**  ************************************************************************
*/
/* $Function        :    GetFileSize(const   char   *file_name)
==   ===============================================================
==  Description     :   ����ļ��Ĵ�С ���ֽ�Ϊ��λ
==  Argument        :   ������ļ���
==  Return          :    -1�� ����ʧ��    >=0�� ���صĴ�С
==  Modification  : 2011/11/24   tango_zhu    ����
==   ===============================================================
*/

long int GetFileSize(const   char   *file_name)
{
    struct   stat   buf;
    if(stat(file_name,   &buf)   !=   0)
    {
        return (-1);
    }
    return (buf.st_size);
}



int GetKernelVersion(void)
{
    FILE *fp = NULL;
    S8 as8Buf[128];
    S8 *pAddr = NULL;
    S32 Ver1 = 0;
    S32 Ver2 = 0;
    S32 Ver3 = 0;
    S32 s32Ret = -1;
    if((fp = fopen(LINUX_KERNEL_VERSION, "r")) == NULL)
    {
        LIBHAL_ERROR("open %s fail!\n", LINUX_KERNEL_VERSION);
        goto fail;
    }
    if(fgets(as8Buf, 128, fp) == NULL)
    {
        LIBHAL_ERROR("fgets value of kernel version fail!\n");
        fclose(fp);
        goto fail;
    }
    fclose(fp);
    pAddr = strstr(as8Buf, KERNEL_VERSION);
    if(pAddr != NULL)
    {
        pAddr += strlen(KERNEL_VERSION);
        s32Ret = sscanf(pAddr, " %d.%d.%d", &Ver1, &Ver2, &Ver3);
        if(s32Ret < 0)
        {
            LIBHAL_ERROR("sscanf fail!\n");
        }
        LIBHAL_DEBUG("kernel version = %d.%d.%d\n", Ver1, Ver2, Ver3);
    }
    else
    {
        LIBHAL_ERROR("do't find kernel version!\n");
        goto fail;
    }
    return 0;
fail:
    return -1;
}



/* $Function        :   wireless_hotplug_thd
==   ===============================================================
==  Description     :   �Ȳ������, �����Ȳ����Ϣ�����ϱ�3Gģ��������
==  Argument        :   ��
==  Return          :   ��
==  Modification    :   2011/7/19    tango_zhu     ����
==   ===============================================================
*/
#define UEVENT_BUFFER_SIZE 2048
#include <linux/netlink.h>

void *system_hotplug_thd(void *arg)
{
    struct sockaddr_nl snl;
    char buf[UEVENT_BUFFER_SIZE] = {0};
    const int buffersize = 32 * UEVENT_BUFFER_SIZE;
    int retval;
    pthread_detach(pthread_self());
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
    int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if(hotplug_sock == -1)
    {
        LIBHAL_ERROR("Getting socket: %s", strerror(errno));
        return (void *)(-1);
    }
    /* ���ý��ջ�������С */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
    retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));
    if(retval < 0)
    {
        LIBHAL_ERROR("Bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return (void *)(-1);
    }
    while(1)
    {
        if(recv(hotplug_sock, &buf, sizeof(buf), 0) > 0)
        {
            LIBHAL_ERROR("Hotplug receive failed: %s", buf);
        }
        else
        {
            LIBHAL_ERROR("Hotplug receive failed: %s", strerror(errno));
        }
    }
    return 0;
}

/*****************************************************************************
 �� �� ��  : CallForkProcess
 ��������  : fork һ������
 �������  : char *pProcessPath:process ·��
             char **pArgv  ���� ��һ�����������ǳ�����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��17��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int CallForkProcess(char *pProcessPath, char **pArgv)
{
    int pid;
    if((pid = vfork()) < 0)
    {
        LIBHAL_ALARM("Fail to fork new process!");
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

/* $Function        :   uint_sleep
==   ===============================================================
==  Description     :   �뼶˯��,���ᱻ�źŴ��
==  Argument        :   [IN] sec: ˯��ʱ�䣬��λ��
==  Return          :   ��
==  Modification    :   2011/7/19    tango_zhu     ����
==   ===============================================================
*/
void uint_sleep(int sec)
{
    while((sec = sleep(sec)));
}


/* $Function        :   uint_msleep
==   ===============================================================
==  Description     :   ���뼶˯��,���ᱻ�źŴ��
==  Argument        :   [IN] msec: ˯��ʱ�䣬��λ����
==  Return          :   ��
==  Modification    :   2011/7/19   tango_zhu     ����
==   ===============================================================
*/
void uint_msleep(int msec)
{
    struct timespec req;
    req.tv_sec = msec / 1000;
    req.tv_nsec = (msec % 1000) * 1000000;
    while((nanosleep(&req, &req) == -1) && (errno == EINTR))
    {
        continue;
    }
}
/* $Function        :   set_bit_mask
==   ===============================================================
==  Description     :   ����һλ����
==  Argument        :   pval:����ֵ��ָ�룬index ���õ�λ��val ���õ�ֵ
==  Return          :   ��
==  Modification    :   2011/7/19    tango_zhu     ����
==   ===============================================================
*/
void set_bit_mask(unsigned int *pval, unsigned char bit_index, unsigned char bit_val)
{
    if(0x1 == bit_val)
    {
        *pval |= (1 << bit_index);
    }
    else
    {
        *pval &= ~(1 << bit_index);
    }
}

