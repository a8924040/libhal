#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include "Log.h"
int main(int argc, char *argv[])
{
    char *s = NULL;
    s = getenv("USER");
    putenv("TERM=linux");
    s = getenv("TERM");
    LIBHAL_DEBUG("the get env is %s", s);
    return 0;
}

