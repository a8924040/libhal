#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

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
    int NonRegionErase(int Fd, int start, int count, int unlock);
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
    int GetRegionCount(const char *region);



#ifdef __cplusplus
}
#endif

#endif

