/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  �� �� ��   : Mouse.h
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : ������ݴ���ͷ�ļ�
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��18��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __MOUSE_H__
#define __MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif


    /* �����豸�����ṹ�� */
    typedef struct INPUT_DEVICES_INFO
    {
        int id;
        int fd_event;
        char is_ts; // touch screen is 1, else is 0
        char device_node[15];
    } INPUT_DEVICES_INFO_STRU;

    /*----------------------------------------------*
     * ģ�鼶����                                   *
     *----------------------------------------------*/
#define MAX_INPUT_DEVICES 4
#define MAX_PATH 256
#define PRESS_SURE  1



#define DVR_ERR LIBHAL_ERROR
#define DVR_MSG LIBHAL_INFO
#define DVR_WARN LIBHARDWARE_WARNING


    /*
    MOUSE_LBUTTON   ���״̬���롣
    MOUSE_RBUTTON   �Ҽ�״̬���롣
    MOUSE_MBUTTON   �м�״̬���롣
    MOUSE_TOUCHSCREEN   �Ƿ�Ϊ���������ݡ�
    */
#define MOUSE_LBUTTON               0x1
#define MOUSE_RBUTTON               0x2
#define MOUSE_MBUTTON               0x4
#define MOUSE_TOUCHSCREEN           0x80



    /*
    key ���������3��������״̬����1��ʾ�������£���0��ʾ�����������±��е�ֵ����õ���Ӧ������״̬�����λ��Ӧ��������־����1��ʾΪ���������ݣ���0��ʾ������ݡ����������µ���������Ϊ�����
    x   x����ƫ�ƣ��������ң���������ȡֵ[-127,128]�����ʹ�á�
    y   y����ƫ�ƣ��������ϣ��������£�ȡֵ[-127,128]�����ʹ�á�
    z   ����ƫ�ƣ�������󣬸�����ǰ��ȡֵ[-127,128]�����ʹ�á�
    ax  ����x���꣬���������ҡ�������ʹ�á�
    ay  ����y���꣬���������¡�������ʹ�á�
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
     �� �� ��  : MouseGetData
     ��������  : ��ȡ������� �ȵ������������¼��Ժ󷵻��������
     �������  : MOUSE_DATA *data
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��26��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int MouseGetData(MOUSE_DATA *data);
    /*****************************************************************************
     �� �� ��  : MouseCreate
     ��������  : �������豸�ļ���ȡȫ�ֱ���DevicesFd
     �������  : void
     �������  : ��
     �� �� ֵ  : -1:���ļ�ʧ��
                 0: ���ļ��ɹ�
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��19��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int MouseCreate(void);
    /*****************************************************************************
     �� �� ��  : MouseDestroy
     ��������  : �ر����д򿪵�����豸�ļ�����������ʼ���豸����
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��26��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int MouseDestroy(void);


#ifdef __cplusplus
}
#endif

#endif




