/**
* @file
* @brief test ini file api
* @author Deng Yangjun
* @date 2007-1-9
* @version 0.2
*/
#include <stdio.h>
#include "IniFile.h"
#include "NetWork.h"
#include <CommonDef.h>
#include <CommonInclude.h>
#define NETDEVPATH                      "/sys/class/net/"
#define BUF_SIZE 256

int main()
{
    char file[32] = {"network12.conf"};
    const char *section = "NETWORK";
    const char *name_key = "IPADDR";
    const char *age_key = "MAC";
    char name[BUF_SIZE] = {0};
    int age;
    if(!write_profile_string(section, name_key, "192.168.2.19", file))
    {
        printf("write name pair to ini file fail\n");
        return -1;
    }
    if(!write_profile_string(section, age_key, "12:34:56:78", file))
    {
        printf("write age pair to ini file fail\n");
        return -1;
    }
    printf("[%s]\n", section);
    if(!read_profile_string(section, name_key, name, BUF_SIZE, "", file))
    {
        printf("read ini file fail\n");
        return -1;
    }
    else
    {
        printf("%s=%s\n", name_key, name);
    }
    //read age pair, test read int value.
    //if read fail, return default value
    age = read_profile_int(section, age_key, 0, file);
    printf("%s=%d\n", age_key, age);
    UPSTRING(file);
    printf("%s\n", file);
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(NETDEVPATH);
    while((ptr = readdir(dir)) != NULL)
    {
        if((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0) || (strcmp(ptr->d_name, "lo") == 0))                   //ÅÅ³ýloÍø¿¨
        {
            continue;
        }
        printf("the netdev name is %s\n", ptr->d_name);
    }
    printf("begin to set ip\n");
    NetWorkSetIp("eth0", "192.168.2.19");
    NetWorkSetNetMask("eth0", "255.255.255.0") ;
    NetWorkSetGateWay("eth0", "192.168.2.1");
    return 0;
}

