/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : Mouse.h
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : 鼠标数据处理头文件
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月18日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/
#ifndef __MOUSE_H__
#define __MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif


    /* 输入设备描述结构体 */
    typedef struct INPUT_DEVICES_INFO
    {
        int id;
        int fd_event;
        char is_ts; // touch screen is 1, else is 0
        char device_node[15];
    } INPUT_DEVICES_INFO_STRU;

    /*----------------------------------------------*
     * 模块级变量                                   *
     *----------------------------------------------*/
#define MAX_INPUT_DEVICES 4
#define MAX_PATH 256
#define PRESS_SURE  1



#define DVR_ERR LIBHAL_ERROR
#define DVR_MSG LIBHAL_INFO
#define DVR_WARN LIBHARDWARE_WARNING


    /*
    MOUSE_LBUTTON   左键状态掩码。
    MOUSE_RBUTTON   右键状态掩码。
    MOUSE_MBUTTON   中键状态掩码。
    MOUSE_TOUCHSCREEN   是否为触摸屏数据。
    */
#define MOUSE_LBUTTON               0x1
#define MOUSE_RBUTTON               0x2
#define MOUSE_MBUTTON               0x4
#define MOUSE_TOUCHSCREEN           0x80



    /*
    key 鼠标左右中3个按键的状态，置1表示按键按下，置0表示按键弹起。与下表中的值相与得到对应按键的状态。最高位对应触摸屏标志，置1表示为触摸屏数据，置0表示鼠标数据。触摸屏按下弹起动作处理为左键。
    x   x坐标偏移，正数向右，负数向左，取值[-127,128]。鼠标使用。
    y   y坐标偏移，正数向上，负数向下，取值[-127,128]。鼠标使用。
    z   滚轮偏移，正数向后，负数向前，取值[-127,128]。鼠标使用。
    ax  绝对x坐标，正方向向右。触摸屏使用。
    ay  绝对y坐标，正方向向下。触摸屏使用。
    */

    typedef struct tagMOUSE_DATA
    {
        unsigned char   key;
        signed char     x;
        signed char     y;
        signed char     z;
        unsigned short  ax;
        unsigned short  ay;
    } MOUSE_DATA;

    typedef enum CALIBRATESTATE
    {
        CalibrateStart = 0,
        CalibrateDone = 1,

    } CALIBRATESTATE_ENUM;

    typedef enum MOUSEGETDATATYPE
    {
        MouseGetDataNormal = 0,
        MouseGetDataAdj,
    } MOUSEGETDATATYPE_ENUM;

    /*****************************************************************************
     函 数 名  : MouseGetData
     功能描述  : 获取鼠标数据 等到获得整个鼠标事件以后返回鼠标数据
     输入参数  : MOUSE_DATA *data
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年6月26日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int MouseGetData(MOUSE_DATA *data);
    /*****************************************************************************
     函 数 名  : MouseCreate
     功能描述  : 打开输入设备文件获取全局变量DevicesFd
     输入参数  : void
     输出参数  : 无
     返 回 值  : -1:打开文件失败
                 0: 打开文件成功
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年6月19日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int MouseCreate(void);
    /*****************************************************************************
     函 数 名  : MouseDestroy
     功能描述  : 关闭所有打开的鼠标设备文件描述符，初始化设备数组
     输入参数  : void
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年6月26日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int MouseDestroy(void);


#ifdef __cplusplus
}
#endif

#endif




