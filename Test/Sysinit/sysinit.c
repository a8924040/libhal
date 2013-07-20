#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "Log.h"
#include "Debug.h"
#include <signal.h>
#include "System.h"

THREAD_INFO_STRU *pThreadInfo;

static void *testthread_thd(void *arg)
{
    int thread_id = 0;
    int i = 0;
    int i_thread = * (int *) arg;
    char thread_name[32] = {0};
    pthread_detach(pthread_self());
    thread_id = (int) syscall(__NR_gettid);
    printf("the thread id is %d,the pid is %d,the i_thread = %d,cpu is %d\n"
           , thread_id, getpid(), i_thread, SystemGetCPUUsage());
    sprintf(thread_name, "the test process is %d", thread_id);
    SystemAddThreadInfo(pThreadInfo, thread_name, thread_id);
    while(1)
    {
        if(i_thread != 10)
        {
            i++;
            if(i > 100)
            {
                printf("thread %s,%d eixt\n", thread_name, thread_id);
                SystemDeleteThreadInfo(pThreadInfo, thread_name, thread_id);
                return 0;
            }
            usleep(510000);
        }
    }
    return 0;
}



int main(int argc, char *argv[])
{
    pthread_t testhread_tid;
    int i_thread = 0;
    SystemDeviceInit(&pThreadInfo);
    for(i_thread = 0; i_thread < 100; i_thread++)
    {
        if(pthread_create(&testhread_tid, NULL, testthread_thd, (void *) &i_thread) != 0)
        {
            LIBHAL_ERROR("Fail to start wireless watch thread! error: %d", errno);
            return -1;
        }
        usleep(500000);
    }
    while(1)
    {
        sleep(1);
    }
    return 0;
}

