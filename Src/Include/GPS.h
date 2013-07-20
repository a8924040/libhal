/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : log.h
**  Description   : ���ڶ���
**  Modification  : 2011/7/19    tango_zhu  ����
**  ************************************************************************
*/
#ifndef __GPS_H__
#define __GPS_H__

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct
    {
        char latitude[32];              /*!<  γ��*/
        char longitude[32];             /*!<  ����*/
        char pos_latitude[4];           /*�ϱ�γN or S*/
        char pos_longitude[4];          /*������W or E*/
        char data_vail[4];              /*!<  ��λ��Ч��*/
        char satellite_vail_sum[4];                 /*��Ч����*/
        char altitude[32];          /*!<  �߶�*/
        char speed[32];                 /*!<  �ٶ�*/
        char direction[32];
        char time_utc[32];        /*utc ʱ��*/
        char date_utc[32];        /*utc ����*/
    } GpsOrientData, *pGpsOrientData;


    /* $Function        :   int GpsDeviceCreate(void)
    ==   ===============================================================
    ==  Description     :   ��ô����豸��dev·��/dev/s3c2410_serial2
    ==  Argument        :   ��
    ==  Return          :   -2:�豸�Ѿ��򿪣�-1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpsDeviceCreate(void);

    /* $Function        :   IrDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   ��ʼ�������豸
    ==  Argument        :   ��
    ==  Return          :   -1�� ��ʼ��ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpsDeviceInit(void);

    /* $Function        :   GpsDeviceWrite(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :
    ==  Argument        :
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpsDeviceWrite(unsigned char *pbuf);

    /* $Function        :   int GpsDeviceRead(GpsOrientData *pgps_orient_data)
    ==   ===============================================================
    ==  Description     :   ��gps�豸����������
    ==  Argument        :   pgps_orient_data GpsOrientData�ṹ��ָ��
    ==  Return          :    0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpsDeviceRead(GpsOrientData *pgps_orient_data);

    /* $Function        :   GpsDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   �ر�gps�豸
    ==  Argument        :   pbuf �������豸д���ݵ�buf
    ==  Return          :   -1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int GpsDeviceClose(void);


#ifdef __cplusplus
}
#endif

#endif
