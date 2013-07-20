#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <Network.h>


int main(int argc, char *argv[])
{
    S8 s8UrlIP[32] = { 0 };

    char url[32] = "ztg328.oicp.net";
    NetWorkGetIpFromUrl(url, s8UrlIP);
    LIBHAL_DEBUG("%s url ip is %s\n", url, s8UrlIP);

   // if(CheckIpConflic("wlan0",  "192.168.2.21") < 0)
   // {
  ///      LIBHAL_ERROR("this ip is already be used someone!");
 //   }



    //NetWorkSetGateWay("wlan0", "0.0.0.0");
    //NetWorkSetDefault("wlan0");
    return 0;
}
