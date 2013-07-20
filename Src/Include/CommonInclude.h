/*
**  ************************************************************************
**
**  File    Name  : COMMON_INCLUDE.h
**  Description   : 通用包含头文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __COMMON_INCLUDE_H__
#define __COMMON_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linux/unistd.h>


#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include <termios.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/times.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <sys/mman.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "Type.h"
#include "Log.h"
#ifdef __cplusplus
}
#endif

#endif
