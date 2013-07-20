#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <mtd/mtd-user.h>

#include "CommonDef.h"
#include "CommonInclude.h"

#define MTD_INFO_PATH "/proc/mtd"

/*****************************************************************************
 函 数 名  : GetRegionCount
 功能描述  : 获取分区有多少block
 输入参数  : const char *region
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GetRegionCount(const char *region)
{
    FILE *fpRegion = NULL;
    S8 s8Buffer[128] = {0};
    U8 u8Mtd[32] = {0};
    U32 u32Size = 0;
    U32 u32Block = 0;
    if(NULL == region)
    {
        LIBHAL_ERROR("GetRegionCount param error!");
        return -1;
    }
    if((fpRegion = fopen(MTD_INFO_PATH, "r")) == NULL)
    {
        LIBHAL_ERROR("fopen %s failed", MTD_INFO_PATH);
        return -1;
    }
    while(NULL != fgets(s8Buffer, sizeof(s8Buffer), fpRegion))
    {
        if(NULL != (strstr(s8Buffer, region)))
        {
            LIBHAL_DEBUG("fined the region %s", s8Buffer);
            sscanf(s8Buffer, "%s %x %x", u8Mtd, &u32Size, &u32Block);
            return (u32Size / u32Block);
        }
    }
    LIBHAL_ERROR("%s not on the chip\n");
    return -1;
}

/*****************************************************************************
 函 数 名  : NonRegionErase
 功能描述  : 擦除flash分区
 输入参数  : int Fd
             int start
             int count
             int unlock
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int NonRegionErase(int Fd, int start, int count, int unlock)
{
    mtd_info_t meminfo;
    if(ioctl(Fd, MEMGETINFO, &meminfo) == 0)
    {
        erase_info_t erase;
        erase.start = start;
        erase.length = meminfo.erasesize;
        LIBHAL_DEBUG("Erase Unit Size 0x%x\n", meminfo.erasesize);
        for(; count > 0; count--)
        {
            LIBHAL_DEBUG("Performing Flash Erase of length 0x%x at offset 0x%x\n"
                         , erase.length, erase.start);
            if(unlock != 0)
            {
                //Unlock the sector first.
                LIBHAL_DEBUG("Performing Flash unlock at offset 0x%x", erase.start);
                if(ioctl(Fd, MEMUNLOCK, &erase) != 0)
                {
                    perror("\nMTD Unlock failure");
                    close(Fd);
                    return 8;
                }
            }
            if(ioctl(Fd, MEMERASE, &erase) != 0)
            {
                perror("\nMTD Erase failure");
                close(Fd);
                return 8;
            }
            erase.start += meminfo.erasesize;
        }
        LIBHAL_DEBUG("done\n");
    }
    return 0;
}

