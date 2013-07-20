#include <System.h>
#include <CommonDef.h>
#include <CommonInclude.h>
#include "BootEnv.h"

int main(void)
{
    printf("%s,compile ta %s,%s\n", __FILE__, __DATE__, __TIME__);
    BootEnvInit();
    BootSetPara("appauto", "0");
    return 0;
}

