/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : Touchscreen.h
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : 触摸屏数据处理头文件
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月18日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/

#ifndef __TOUCHSCREEN_H__
#define __TOUCHSCREEN_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum TSC_ADJ_INDEX
    {
        INDEX_LU = 0,   //左上
        INDEX_LD,       //左下
        INDEX_RD,       //右下
        INDEX_M,        //中间
        INDEX_MAX
    }
    TSC_ADJ_INDEX_E;

    typedef struct TSC_ADJ_INFO
    {
        int x;          //绝对坐标x
        int y;          //绝对坐标y
        int x_value;    //坐标对应的ad芯片值x
        int y_value;    //坐标对应的ad芯片值y
        int valid;      // 1表示数据可用，其他不可用
    } TSC_ADJ_INFO_S;


    typedef struct TSC_ADJ_DOT_INFO
    {
        TSC_ADJ_INDEX_E adj_index;
        TSC_ADJ_INFO_S *adj_data;
    } TSC_ADJ_DOT_INDO_S;

    /*
    *        **********************************************
    *        *X(xl,yl)                                    *
    *        *                                            *
    *        *                                            *
    *        *                    X(xm,ym)                *
    *        *                                            *
    *        *                                            *
             *                                            *
             *                                    X(xr,yr)*
             **********************************************
                   xm-xl                   xm(代表应用传下来的对应的屏幕中间x坐标)
    *xl_divider =----------------            xl(代表应用传下来的对应的屏幕左上x坐标)
    *             xm(adj)-xl(adj)            xm(adj)对应实际对应的xm点的ad的值
    *                                        xl(adj)对应实际对应的xl点的ad的值
    *xl_weight = x - xl_divider*xl(adj)      x 代表应用需要的坐标值
    *                                        xl_divider 代表左半屏的x轴比例因子
    *                                        xl(adj) 代表点的ad值
    *
    *
    *
    *
    *
    */
    struct  adjust_coefficient
    {
        float xl_divider;/*校准分左右两个半屏校准，表示左半屏的x轴比例因子*/
        float xl_weight;/*对应的左半屏x轴的加权*/
        float yl_divider;
        float yl_weight;
        float xr_divider;
        float xr_weight;
        float yr_divider;
        float yr_weight;
        int   xl;
        int   xm;
        int   xr;
        int flags;
    };


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
    int TouchScreenSetAdjInfo(TSC_ADJ_INFO_S *AdjInfo,
                              TSC_ADJ_INDEX_E index);
    /*****************************************************************************
     函 数 名  : TouchScreenGetAdjInfo
     功能描述  : 应用层获取底层校准完成的触摸屏校准参数
     输入参数  : TSC_ADJ_INFO_S * AdjInfo_Ret:对应的坐标点坐标
                 TSC_ADJ_INDEX_E index:坐标点序号
     输出参数  : 无
     返 回 值  : success :0
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年6月26日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int TouchScreenGetAdjInfo(TSC_ADJ_INFO_S *AdjInfo, TSC_ADJ_INDEX_E index);
    /*****************************************************************************
     函 数 名  : TouchScreenAdjDot
     功能描述  : 触摸屏校准函数
     输入参数  : int x:应用设置下来的x坐标值
                 int y:应用设置下来的y坐标值
                 TSC_ADJ_INDEX_E index:对应x，y坐标值的序号
     输出参数  : 无
     返 回 值  :
     调用函数  :
     被调函数  :

     修改历史      :
      1.日    期   : 2012年6月28日
        作    者   : tango_zhu
        修改内容   : 新生成函数

    *****************************************************************************/
    int TouchScreenAdjDot(int x, int y, TSC_ADJ_INDEX_E index);
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
    int TouchScreenDestory(void);
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
    int TouchScreenCreate(void);
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
    void TouchScreenAdjEsc(void);


#ifdef __cplusplus
}
#endif

#endif

