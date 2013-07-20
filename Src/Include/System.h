#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

    typedef struct SYSTEM_TIME /* ϵͳʱ��ṹ�� */
    {
        int second;     /* seconds (1 - 60) */
        int minute;     /* minutes (0 - 59) */
        int hour;           /* hours (0 - 23)   */
        int wday;           /* day of week      */
        int day;            /* day of the month */
        int month;      /* month (0 - 11)   */
        int year;      /* year          */
    } SYSTEM_TIME_STRU;

    typedef struct SYSTEM_CAPS /* ϵͳ���ܽṹ�� */
    {

        unsigned long MemoryTotal;    //Total usable main memory size
        unsigned long MemoryLeft;     //Available memory size

    } SYSTEM_CAPS_STRU;

    typedef struct THREAD_INFO /* �������߳������߳�id */
    {
        char thread_name[32];
        unsigned int thread_id;
        struct THREAD_INFO *pthread_info_next;
    } THREAD_INFO_STRU;


    /*****************************************************************************
    �� �� ��  : SystemGetCurrentTime
    ��������  : ����ϵͳʱ��
    �������  : SYSTEM_TIME *pTime
    �������  : ��
    �� �� ֵ  : -1:ʧ��
                 0:�ɹ�

    �޸���ʷ      :
     1.��    ��   : 2012��5��20��
       ��    ��   : tango_zhu
       �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemGetCurrentTime(SYSTEM_TIME_STRU *pTime);

    /*****************************************************************************
     �� �� ��  : SystemSetCurrentTime
     ��������  : ����ϵͳʱ��
     �������  : SYSTEM_TIME *pTime
     �������  : ��
     �� �� ֵ  : -1:ʧ��
                  0:�ɹ�

     �޸���ʷ      :
      1.��    ��   : 2012��5��20��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/

    int SystemSetCurrentTime(SYSTEM_TIME_STRU *pTime);
    /*****************************************************************************
     �� �� ��  : SystemGetCPUUsage
     ��������  : ���ϵͳcpuռ����
     �������  : void
     �������  : ��
     �� �� ֵ  : -1 ʧ��
                  0 �ɹ�

     �޸���ʷ      :
      1.��    ��   : 2012��5��20��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemGetCPUUsage(void);
    /*****************************************************************************
     �� �� ��  : SystemDeviceInit
     ��������  : ��ʼ��ϵͳ����ϵͳ��־��ϵͳ��������
     �������  : THREAD_INFO_STRU *thread_info
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��5��21��
        ��    ��   : thread_info
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemDeviceInit(THREAD_INFO_STRU **thread_info);
    /*****************************************************************************
     �� �� ��  : SystemDeleteThreadInfo
     ��������  : ɾ�����̴�
     �������  : THREAD_INFO_STRU *thread_info
                 char *p_thread_name
                 unsigned int thread_id
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��5��22��
        ��    ��   : thread_info
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemDeleteThreadInfo(THREAD_INFO_STRU *thread_info, char *p_thread_name
                               , unsigned int thread_id);
    /*****************************************************************************
     �� �� ��  : SystemAddThreadInfo
     ��������  : �洢���̴洢��Ϣ��������Ϣ������
     �������  : THREAD_INFO_STRU *thread_info
                 char *p_thread_name
                 unsigned int thread_id
     �������  : ��
     �� �� ֵ  :  -1:fail  0:success
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��5��21��
        ��    ��   : thread_info
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemAddThreadInfo(THREAD_INFO_STRU *thread_info, char *p_thread_name
                            , unsigned int thread_id);

    /*****************************************************************************
     �� �� ��  : SystemCheckPartionMounted
     ��������  : ���Ҫ���ص�Ŀ¼�Ƿ��Ѿ�����
     �������  : char *pmount_partion
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��6��1��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemCheckPartionMounted(char *pmount_partion);

    /*****************************************************************************
     �� �� ��  : SystemSetBlankInterVal
     ��������  : �����������̨��ʾʱ��
     �������  : void
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2013��1��1��
        ��    ��   : tanago_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int SystemSetBlankInterVal(int timeval);


    /*****************************************************************************
     �� �� ��  : SystemInitPlatform
     ��������  : ϵͳ��ʼ������
     �������  : void
     �������  : ��
     �� �� ֵ  : void
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��19��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    void SystemInitPlatform(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

