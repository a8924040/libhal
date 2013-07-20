/*
**  ************************************************************************
**
**  File    Name  : COMMON.h
**  Description   : ͨ�ð���ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

    /* $Function        :   RunSystemCommand
    ==   ===============================================================
    ==  Description     :   ģ��ϵͳsystem����
    ==  Argument        :   system ������
    ==  Return          :   0: �ɹ�  -1: ʧ��
    ==  Modification    :   2011/12/24   tango_zhu  ����
    ==   ===============================================================
    */
    int  RunSystemCommand(const char *command);

    /* $Function        :   wireless_hotplug_thd
    ==   ===============================================================
    ==  Description     :   �Ȳ������, �����Ȳ����Ϣ�����ϱ�3Gģ��������
    ==  Argument        :   ��
    ==  Return          :   ��
    ==  Modification    :   2011/7/19    huang_zhendi    ����
    ==   ===============================================================
    */
    void *wireless_hotplug_thd(void *arg);

    /* $Function        :   uint_sleep
    ==   ===============================================================
    ==  Description     :   �뼶˯��,���ᱻ�źŴ��
    ==  Argument        :   [IN] sec: ˯��ʱ�䣬��λ��
    ==  Return          :   ��
    ==  Modification    :   2011/7/19    huang_zhendi    ����
    ==   ===============================================================
    */
    void uint_sleep(int sec);

    /* $Function        :   uint_msleep
    ==   ===============================================================
    ==  Description     :   ���뼶˯��,���ᱻ�źŴ��
    ==  Argument        :   [IN] msec: ˯��ʱ�䣬��λ����
    ==  Return          :   ��
    ==  Modification    :   2011/7/19    huang_zhendi    ����
    ==   ===============================================================
    */
    void uint_msleep(int msec);

    /* $Function        :   set_bit_mask
    ==   ===============================================================
    ==  Description     :   ����һλ����
    ==  Argument        :   pval:����ֵ��ָ�룬index ���õ�λ��val ���õ�ֵ
    ==  Return          :   ��
    ==  Modification    :   2011/7/19    tango_zhu     ����
    ==   ===============================================================
    */
    void set_bit_mask(unsigned int *pval, unsigned char bit_index, unsigned char bit_val);
    /*****************************************************************************
    �� �� ��  : CallForkProcess
    ��������  : fork һ������
    �������  : char *pProcessPath:process ·��
             char **pArgv  ���� ��һ�����������ǳ�����
    �������  : ��
    �� �� ֵ  :
    ���ú���  :
    ��������  :

    �޸���ʷ      :
    1.��    ��   : 2012��11��17��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

    *****************************************************************************/
    int CallForkProcess(char *pProcessPath, char **pArgv);
#ifdef __cplusplus
}
#endif

#endif
