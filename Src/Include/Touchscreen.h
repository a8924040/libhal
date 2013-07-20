/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  �� �� ��   : Touchscreen.h
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : ���������ݴ���ͷ�ļ�
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��18��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __TOUCHSCREEN_H__
#define __TOUCHSCREEN_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum TSC_ADJ_INDEX
    {
        INDEX_LU = 0,   //����
        INDEX_LD,       //����
        INDEX_RD,       //����
        INDEX_M,        //�м�
        INDEX_MAX
    }
    TSC_ADJ_INDEX_E;

    typedef struct TSC_ADJ_INFO
    {
        int x;          //��������x
        int y;          //��������y
        int x_value;    //�����Ӧ��adоƬֵx
        int y_value;    //�����Ӧ��adоƬֵy
        int valid;      // 1��ʾ���ݿ��ã�����������
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
                   xm-xl                   xm(����Ӧ�ô������Ķ�Ӧ����Ļ�м�x����)
    *xl_divider =----------------            xl(����Ӧ�ô������Ķ�Ӧ����Ļ����x����)
    *             xm(adj)-xl(adj)            xm(adj)��Ӧʵ�ʶ�Ӧ��xm���ad��ֵ
    *                                        xl(adj)��Ӧʵ�ʶ�Ӧ��xl���ad��ֵ
    *xl_weight = x - xl_divider*xl(adj)      x ����Ӧ����Ҫ������ֵ
    *                                        xl_divider �����������x���������
    *                                        xl(adj) ������adֵ
    *
    *
    *
    *
    *
    */
    struct  adjust_coefficient
    {
        float xl_divider;/*У׼��������������У׼����ʾ�������x���������*/
        float xl_weight;/*��Ӧ�������x��ļ�Ȩ*/
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
     �� �� ��  : TouchScreenSetAdjInfo
     ��������  : Ӧ�ò�������У׼�Ĳ������ײ�����ϵ�����У׼
     �������  : TSC_ADJ_INFO_S * AdjInfo_Afferent
                 TSC_ADJ_INDEX_E index
     �������  : ��
     �� �� ֵ  : success :��
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��26��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int TouchScreenSetAdjInfo(TSC_ADJ_INFO_S *AdjInfo,
                              TSC_ADJ_INDEX_E index);
    /*****************************************************************************
     �� �� ��  : TouchScreenGetAdjInfo
     ��������  : Ӧ�ò��ȡ�ײ�У׼��ɵĴ�����У׼����
     �������  : TSC_ADJ_INFO_S * AdjInfo_Ret:��Ӧ�����������
                 TSC_ADJ_INDEX_E index:��������
     �������  : ��
     �� �� ֵ  : success :0
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��26��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int TouchScreenGetAdjInfo(TSC_ADJ_INFO_S *AdjInfo, TSC_ADJ_INDEX_E index);
    /*****************************************************************************
     �� �� ��  : TouchScreenAdjDot
     ��������  : ������У׼����
     �������  : int x:Ӧ������������x����ֵ
                 int y:Ӧ������������y����ֵ
                 TSC_ADJ_INDEX_E index:��Ӧx��y����ֵ�����
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��28��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int TouchScreenAdjDot(int x, int y, TSC_ADJ_INDEX_E index);
    /*****************************************************************************
     �� �� ��  : TouchScreenDestory
     ��������  : ���ٴ������豸
     �������  : void
     �������  : ��
     �� �� ֵ  : success: 0
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��2��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int TouchScreenDestory(void);
    /*****************************************************************************
     �� �� ��  : TouchScreenCreate
     ��������  : �����������豸
     �������  : void
     �������  : ��
     �� �� ֵ  : success :0
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��7��2��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int TouchScreenCreate(void);
    /*****************************************************************************
     �� �� ��  : TouchScreenAdjEsc
     ��������  : Ӧ�ò��˳�������У׼
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��28��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    void TouchScreenAdjEsc(void);


#ifdef __cplusplus
}
#endif

#endif

