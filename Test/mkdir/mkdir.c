#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <Network.h>


#include <sys/stat.h>

int main(char argc, char *argv[])
{
	int ok = 0;
	if((ok=mkdir("/var/杭州",0755)) != 0)
	{
		perror("mkdir:");
	}
	return 0;	
}
