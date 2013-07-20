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
#include <NetWork.h>
 
#define DNS_RESOLV_FILE_PATH "/etc/resolv.conf" 
#define DNS_TIMEOUT_MAX 5
#define DNS_HOST  0x01
#define DNS_CNAME 0x05
#define DNS_SERVER_PORT 53 


int main(int argc, char *argv[])
{
    S8 s8UrlIP[32] = { 0 };
    int i ;
    printf("%s,%s\n",__DATE__,__TIME__);

    NetWorkGetIpFromUrl("ztg328.oicp.net", s8UrlIP);
    LIBHAL_INFO("Url IP:%s\n", s8UrlIP);        
    memset(s8UrlIP,0,sizeof(s8UrlIP));
    sleep(1);
    NetWorkSetIp("wlan0", "0.0.0.0");
    //NetWorkSetDefault("wlan0");
    return 0;
}
