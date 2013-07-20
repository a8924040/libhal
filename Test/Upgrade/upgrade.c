#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "NetWork.h"
#include "Flash.h"
#include "Log.h"

typedef enum PACKAGE_CMD
{
    PACKAGE_START,
    PACKAGE_END,
    PACKAGE_RESPONED,
    PACKAGE_DATA

} PACKAGE_CMD_ENUM;

typedef struct UPGRADE_PACKAGE_INFO
{
    U32 u32PackageCrc32;
    PACKAGE_CMD_ENUM enPackageCmd;
    S8 s8Data[0];
} UPGRADE_PACKAGE_INFO_STRU;


void UpgradeServerProcess(void)
{
}

void UpgradeClientProcess(const S8 *s8Path)
{
}


int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        LIBHAL_PRINTF("***-s:start upgrade as servert\n"
                      "***-c:start upgrade as clien,argv[2] is the upgrade image!\n");
        return 0;
    }
    if(strstr(argv[1], "-s") != NULL)
    {
        LIBHAL_INFO("Upgrade start as server!\n");
        UpgradeServerProcess();
    }
    if(strstr(argv[1], "-c") != NULL)
    {
        LIBHAL_INFO("Upgrade start as client!\n");
        UpgradeClientProcess(argv[2]);
    }
    return -1;
}
