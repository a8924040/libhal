/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : Touchscreen.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : 鼠标数据处理
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月18日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <linux/input.h>

#include "Log.h"
#include "Mouse.h"
#include "Touchscreen.h"


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define AVERAGECOUNT    10
#define XY_REVERSE_FLAGS  0x8000
/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
int handle_input_devices(MOUSE_DATA *data, int fd, char is_ts);


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TSC_ADJ_INFO_S AdjInfo[4];
int     x_y_reverse; /* flag for x & y reverse status */
struct  adjust_coefficient touch_factor;
CALIBRATESTATE_ENUM     CalibrateFlag = CalibrateDone;
static int EscTouchScreenAdj = 0;
/*----------------------------------------------*
 * 外部函数说明                             *
 *----------------------------------------------*/
extern int DevicesFd ;
extern INPUT_DEVICES_INFO_STRU InputDevices[MAX_INPUT_DEVICES];
extern int mouse_get_data(MOUSE_DATA *data, MOUSEGETDATATYPE_ENUM type);
extern int detect_input_devices(void);
extern int mouse_destory(void);

/*****************************************************************************
 函 数 名  : TouchScreenOpen
 功能描述  : 打开触摸屏设备，因为输入设备统一在mouse文件的打开直接返回0
 输入参数  : void
 输出参数  : 无
 返 回 值  : success :0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月2日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenOpen(void)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : TouchScreenClose
 功能描述  : 关闭触摸屏设备
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  : success :0
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月2日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenClose(void)
{
    return mouse_destory();
}

/*****************************************************************************
 函 数 名  : TouchScreenCreate
 功能描述  : 创建触摸屏设备
 输入参数  : void
 输出参数  : 无
 返 回 值  : success :0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月2日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenCreate(void)
{
    return TouchScreenOpen();
}

/*****************************************************************************
 函 数 名  : TouchScreenDestory
 功能描述  : 销毁触摸屏设备
 输入参数  : void
 输出参数  : 无
 返 回 值  : success: 0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月2日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenDestory(void)
{
    return TouchScreenClose();
}

/*****************************************************************************
 函 数 名  : TouchScreenGetAdjInfo
 功能描述  : 应用层获取底层校准完成的触摸屏校准参数
 输入参数  : TSC_ADJ_INFO_S * AdjInfo_Ret
             TSC_ADJ_INDEX_E index
 输出参数  : 无
 返 回 值  : success :0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenGetAdjInfo(TSC_ADJ_INFO_S *AdjInfo_Ret, TSC_ADJ_INDEX_E index)
{
    if((index >= INDEX_MAX) || (index < 0))
    {
        DVR_ERR("index is out of range\n");
        return -1;
    }
    AdjInfo_Ret->x      = AdjInfo[index].x;
    AdjInfo_Ret->y      = AdjInfo[index].y;
    AdjInfo_Ret->x_value = AdjInfo[index].x_value;
    AdjInfo_Ret->y_value = AdjInfo[index].y_value;
    AdjInfo_Ret->valid   = 1;
    if(index == INDEX_M && x_y_reverse == 1)
    {
        /* 记录翻转标志 */
        AdjInfo_Ret->x_value = AdjInfo_Ret->x_value | XY_REVERSE_FLAGS;
    }
    return 0;
}


/*****************************************************************************
 函 数 名  : Do_calibration_factor
 功能描述  : 触摸屏校准参数计算
 输入参数  : void
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月28日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int Do_calibration_factor(void)
{
    /*
     * 把屏幕分成左右两部分校准以提高精度
     * 触摸屏两点校准法则
     */
    touch_factor.xl = AdjInfo[INDEX_LU].x_value;
    touch_factor.xr = AdjInfo[INDEX_RD].x_value;
    touch_factor.xm = AdjInfo[INDEX_M].x_value;
    touch_factor.xl_divider = (AdjInfo[INDEX_LU].x - AdjInfo[INDEX_M].x)
                              / (float)(AdjInfo[INDEX_LU].x_value - AdjInfo[INDEX_M].x_value);
    touch_factor.xl_weight = AdjInfo[INDEX_LU].x - touch_factor.xl_divider * AdjInfo[INDEX_LU].x_value;
    touch_factor.yl_divider = (AdjInfo[INDEX_LU].y - AdjInfo[INDEX_M].y)
                              / (float)(AdjInfo[INDEX_LU].y_value - AdjInfo[INDEX_M].y_value);
    touch_factor.yl_weight = AdjInfo[INDEX_LU].y - touch_factor.yl_divider * AdjInfo[INDEX_LU].y_value;
    touch_factor.xr_divider = (AdjInfo[INDEX_RD].x - AdjInfo[INDEX_M].x)
                              / (float)(AdjInfo[INDEX_RD].x_value - AdjInfo[INDEX_M].x_value);
    touch_factor.xr_weight = AdjInfo[INDEX_RD].x - touch_factor.xr_divider * AdjInfo[INDEX_RD].x_value;
    touch_factor.yr_divider = (AdjInfo[INDEX_RD].y - AdjInfo[INDEX_M].y)
                              / (float)(AdjInfo[INDEX_RD].y_value - AdjInfo[INDEX_M].y_value);
    touch_factor.yr_weight = AdjInfo[INDEX_RD].y - touch_factor.yr_divider * AdjInfo[INDEX_RD].y_value;
    /* 标志获取校准参数后可以进行校准 */
    touch_factor.flags = 1;
    CalibrateFlag = CalibrateDone;
    return 0;
}

/*****************************************************************************
 函 数 名  : TouchScreenSetAdjInfo
 功能描述  : 应用层设置已校准的参数到底层避免上电后二次校准
 输入参数  : TSC_ADJ_INFO_S * AdjInfo_Afferent
             TSC_ADJ_INDEX_E index
 输出参数  : 无
 返 回 值  : success :０
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenSetAdjInfo(TSC_ADJ_INFO_S *AdjInfo_Afferent,
                          TSC_ADJ_INDEX_E index)
{
    int i;
    if((index > INDEX_MAX) || (index < 0))
    {
        DVR_ERR("index out range\n");
        return -1;
    }
    touch_factor.flags = 0;
    /*一次设置所有数据*/
    if(index == INDEX_MAX)
    {
        for(i = 0 ; i < INDEX_MAX; i++)
        {
            AdjInfo[i].x            = (AdjInfo_Afferent + i)->x;
            AdjInfo[i].y            = (AdjInfo_Afferent + i)->y;
            AdjInfo[i].x_value      = (AdjInfo_Afferent + i)->x_value;
            AdjInfo[i].y_value      = (AdjInfo_Afferent + i)->y_value;
            AdjInfo[i].valid        = (AdjInfo_Afferent + i)->valid;
        }
    } /* 应用层分次设置校准数据 */
    else
    {
        AdjInfo[index].x            =   AdjInfo_Afferent->x;
        AdjInfo[index].y            =   AdjInfo_Afferent->y;
        AdjInfo[index].x_value      =   AdjInfo_Afferent->x_value;
        AdjInfo[index].y_value      =   AdjInfo_Afferent->y_value;
        AdjInfo[index].valid        =   AdjInfo_Afferent->valid;
    }
    /*if had got the end point ,begin calculate adjust coefficient */
    if(INDEX_M == index)
    {
        if(AdjInfo[index].x_value & XY_REVERSE_FLAGS)
        {
            x_y_reverse = 1;
            AdjInfo[index].x_value = AdjInfo[index].x_value & 0x7FFF;
        }
        Do_calibration_factor();
    }
    return 0;
}



/*****************************************************************************
 函 数 名  : TouchScreenGetAdjDotData
 功能描述  : 应用层获得触摸屏校准数据
 输入参数  : MOUSE_DATA *data
 输出参数  : 无
 返 回 值  : faile : -1
             success : 0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月28日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenGetAdjDotData(MOUSE_DATA *data)
{
    int i;
    if(NULL ==  data)
    {
        DVR_ERR("mouse get data invalided parameter!");
        return -1;
    }
    if(detect_input_devices() < 0)
    {
        DVR_ERR("detect input devices failed!");
        goto fail;
    }
    for(i = 0 ; i < MAX_INPUT_DEVICES; i++)
    {
        if(1 == InputDevices[i].is_ts)
        {
            break;
        }
    }
    /* 如果没有触摸屏设备，返回-1 */
    if(MAX_INPUT_DEVICES == i)
    {
        goto fail;
    }
    return mouse_get_data(data, MouseGetDataAdj);
fail:
    return -1;
}


/*****************************************************************************
 函 数 名  : TouchScreenAdjEsc
 功能描述  : 应用层退出触摸屏校准
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月28日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
void TouchScreenAdjEsc(void)
{
    EscTouchScreenAdj = 1;
}

/*****************************************************************************
 函 数 名  : deal_xy_reverse
 功能描述  : 内联函数 处理触摸屏翻转
 输入参数  : void
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月2日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static inline void deal_xy_reverse(void)
{
    if(abs(AdjInfo[0].x_value - AdjInfo[1].x_value)         \
            > abs(AdjInfo[0].y_value - AdjInfo[1].y_value))
    {
        AdjInfo[0].x_value = AdjInfo[0].x_value ^  AdjInfo[0].y_value;
        AdjInfo[0].y_value = AdjInfo[0].y_value ^  AdjInfo[0].x_value;
        AdjInfo[0].x_value = AdjInfo[0].y_value ^  AdjInfo[0].x_value;
        AdjInfo[1].x_value = AdjInfo[1].x_value ^  AdjInfo[1].y_value;
        AdjInfo[1].y_value = AdjInfo[1].y_value ^  AdjInfo[1].x_value;
        AdjInfo[1].x_value = AdjInfo[1].y_value ^  AdjInfo[1].x_value;
        /* 需要x,y 倒置 */
        x_y_reverse = 1;
    }
}

/*****************************************************************************
 函 数 名  : TouchScreenAdjDot
 功能描述  : 触摸屏校准函数
 输入参数  : int x
             int y
             TSC_ADJ_INDEX_E index
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月28日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int TouchScreenAdjDot(int x, int y, TSC_ADJ_INDEX_E index)
{
    MOUSE_DATA touchdata[AVERAGECOUNT];
    int i = 0;
    int count = 0;
    double ax_tmp = 0;
    double ay_tmp = 0;
    if((index >= INDEX_MAX) || (index < 0))
    {
        DVR_ERR("index is out of range");
        goto fail;
    }
    /* do not convert the data while MouseGetDataAdjDot */
    touch_factor.flags = 0;
    if(index == 0)
    {
        x_y_reverse = 0;
    }
    CalibrateFlag = CalibrateStart;
    do
    {
        if(TouchScreenGetAdjDotData(&touchdata[0]) < 0)
        {
            goto fail;
        }
    }
    while((touchdata[0].key != (MOUSE_TOUCHSCREEN | MOUSE_LBUTTON))
            && !EscTouchScreenAdj);
    do
    {
        if(i < AVERAGECOUNT - 1)
        {
            if((TouchScreenGetAdjDotData(&touchdata[++i]) < 0))
            {
                goto fail;
            }
        }
        else
        {
            i = 0;
        }
    }
    while(touchdata[i].key != MOUSE_TOUCHSCREEN && !EscTouchScreenAdj);
    /* get the average data */
    for(; i > 0;)
    {
        if(touchdata[--i].key == (MOUSE_TOUCHSCREEN | MOUSE_LBUTTON))
        {
            ax_tmp += touchdata[i].ax;
            ay_tmp += touchdata[i].ay;
            count++;
        }
    }
    AdjInfo[index].x = x;
    AdjInfo[index].y = y;
    AdjInfo[index].x_value = ax_tmp / count;
    AdjInfo[index].y_value = ay_tmp / count;
#if 0
    DVR_ERR("AdjInfo[index].x = %d,AdjInfo[index].y =%d, AdjInfo[index].x_value =%d"
            "AdjInfo[index].y_value =%d", AdjInfo[index].x, AdjInfo[index].y, AdjInfo[index].x_value, AdjInfo[index].y_value);
#endif
    /* if x & y is reverse or not */
    if(INDEX_LD == index)
    {
        deal_xy_reverse();
    }
    /* begin calculate adjust coefficient */
    if(INDEX_M == index)
    {
        Do_calibration_factor();
    }
    return 0;
fail:
    CalibrateFlag = CalibrateDone;
    DVR_ERR("MouseGetData error\n");
    return -1;
}


/*****************************************************************************
 函 数 名  : conver_coor
 功能描述  : 在触摸屏没有校准前，将从触摸屏读到的数据限制在显示屏范围内
 输入参数  : unsigned short coor
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static unsigned short conver_coor(unsigned short coor)
{
    int offset = 156;
    int width = 0x3ff;
    double tmp = (double) coor;
    //DVR_MSG("coor:%d\n", coor);
    if(coor > width)
    {
        tmp = (tmp - width) / width;
        coor = coor + (unsigned short)(tmp * offset);
        if(coor > 0x7ff)
        { coor = 0x7ff; }
    }
    else
    {
        tmp = (width - tmp) / width;
        coor = coor - (unsigned short)(tmp * offset);
        if(coor > 0x7ff)
        { coor = 0; }
    }
    return coor;
}

/*****************************************************************************
 函 数 名  : coord_adjust
 功能描述  : 对从触摸屏获得的数据进行转换
 输入参数  : MOUSE_DATA *tmpdata
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月2日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static inline void coord_adjust(MOUSE_DATA *tmpdata)
{
    float tmp = 0;
    if(1 == touch_factor.flags)
    {
        if(tmpdata->ax < touch_factor.xm)
        {
            tmp = touch_factor.xl_divider * tmpdata->ax + touch_factor.xl_weight;
            tmpdata->ax = tmp > 0 ? tmp : 0;
            tmp = touch_factor.yl_divider * tmpdata->ay + touch_factor.yl_weight;
            tmpdata->ay = tmp > 0 ? tmp : 0;
        }
        else if(tmpdata->ax >= touch_factor.xm)
        {
            tmp = touch_factor.xr_divider * tmpdata->ax + touch_factor.xr_weight;
            tmpdata->ax = tmp > 0 ? tmp : 0;
            tmp = touch_factor.yl_divider * tmpdata->ay + touch_factor.yl_weight;
            tmpdata->ay = tmp > 0 ? tmp : 0;
        }
    }
    else if(CalibrateDone == CalibrateFlag)
    {
        tmpdata->ax = ~tmpdata->ax;
        tmpdata->ax &= 0x7ff;
        tmpdata->ax = conver_coor(tmpdata->ax);
        tmpdata->ay = conver_coor(tmpdata->ay);
    }
}

/*****************************************************************************
 函 数 名  : handle_input_devices
 功能描述  : 处理输入设备数据
 输入参数  : MOUSE_DATA *data
             int fd
             char is_ts
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月10日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int handle_input_devices(MOUSE_DATA *data, int fd, char is_ts)
{
    MOUSE_DATA tmpdata;
    static MOUSE_DATA tmpdata_ts;
    fd_set readset;
    struct timeval tv;
    int flag = -1;
    int max_fd = -1;
    unsigned char key_press = 0;
    struct input_event event;
    unsigned int key_status = 0;
    unsigned short ts_x = 0xf000, ts_y = 0xf000;
    int ret = -1;
    memset(&tmpdata, 0, sizeof(MOUSE_DATA));
    if(fd < 0)
    {
        DVR_ERR("Open TS error.\n");
        goto fail;
    }
    max_fd  = MAX(fd, max_fd);
    FD_ZERO(&readset);
    FD_SET(fd, &readset);
    while(1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        /* 使用select 实现避免陷入死循环 */
        ret = select(max_fd + 1, &readset, NULL, NULL, &tv);
        if(ret == 0)
        {
            break;
        }
        if(ret < 0)
        {
            DVR_ERR("mouse select error\n");
            goto fail;
        }
        ret = read(fd, &event, sizeof(struct input_event));
        if(ret < 0)
        {
            DVR_ERR("Input Devices read error.\n");
            ret = -1;
            goto fail;
        }
        if(ret != sizeof(struct input_event))
        {
            DVR_MSG("read event data wrong!\n");
            ret = -1;
            goto fail;
        }
        /* receive syn event, send data if not wrong */
        if(event.type == EV_SYN)
        {
            if(is_ts)
            {
                if((ts_x != 0xf000 && ts_y != 0xf000)     /* 触摸屏按下 */
                        || (flag == -1 && key_press))    /* 触摸屏弹起 */
                {
                    flag = 1;
                }
            }
            else
            {
                flag = 1;
            }
            break;
        }
        if(is_ts)    /*handle touch screen*/
        {
            if(event.type == EV_ABS)
            {
                if(event.code == ABS_X)
                {
                    ts_x = event.value;
                    if(!x_y_reverse)
                    {
                        tmpdata_ts.ax = event.value;
                    }
                    else
                    {
                        tmpdata_ts.ay = event.value;
                    }
                }
                if(event.code == ABS_Y)
                {
                    ts_y = event.value;
                    if(!x_y_reverse)
                    {
                        tmpdata_ts.ay = event.value;
                    }
                    else
                    {
                        tmpdata_ts.ax = event.value;
                    }
                }
            }
            if(event.type == EV_KEY)
            {
                key_status = event.value;
                tmpdata_ts.key = key_status | MOUSE_TOUCHSCREEN;
                key_press = 1;
            }
        }
        else/*handle mouse*/
        {
            if(event.type == EV_REL)
            {
                if(event.code == REL_X)
                {
                    tmpdata.x = event.value;
                }
                if(event.code == REL_Y)
                {
                    tmpdata.y = ~event.value;
                }
                if(event.code == REL_WHEEL)
                {
                    tmpdata.z = event.value;
                }
            }
            if(event.type == EV_KEY)
            {
                key_status = event.value;
                if(key_status == 0)
                {
                    tmpdata.key = key_status;
                }
                else
                {
                    /* 通过event 节点获取的鼠标数据为0x110~0x112 而设备使用
                       的是1,2,4 在这里做转化 */
                    key_status = event.code;
                    tmpdata.key = (1 << (key_status & 0x7));
                }
            }
        }
    }
#if 0
    if(!is_ts)
    {
        DVR_MSG("is mouse tmpdata.key:%d mpdata.x:%d tmpdata.y:%d tmpdata.z:%d tmpdata.ax:%d"
                "tmpdata.ay:%d\n",
                tmpdata.key, tmpdata.x, tmpdata.y, tmpdata.z,
                tmpdata.ax, tmpdata.ay);
    }
    else
    {
        DVR_MSG(" is touch screen tmpdata.key:%d mpdata.x:%d tmpdata.y:%d tmpdata.z:%d tmpdata.ax:%d"
                "tmpdata.ay:%d\n",
                tmpdata_ts.key, tmpdata_ts.x, tmpdata_ts.y, tmpdata_ts.z,
                tmpdata_ts.ax, tmpdata_ts.ay);
    }
#endif
    /* 在触摸屏校准开始时不转化数据直接拷贝ad值 */
    if((CalibrateFlag == CalibrateStart) && is_ts)
    {
        memcpy(data, &tmpdata_ts, sizeof(tmpdata_ts));
    }/* 在校准完成后需要对从触摸屏获得ad值进行转换 */
    else if(flag > 0 && is_ts)
    {
        /* 触摸屏数据转换 */
        coord_adjust(&tmpdata_ts);
        memcpy(data, &tmpdata_ts, sizeof(tmpdata_ts));
    }
    else if(flag > 0)
    {
        /* 鼠标直接数据拷贝 */
        memcpy(data, &tmpdata, sizeof(tmpdata));
    }
    return 0;
fail:
    return ret;
}



