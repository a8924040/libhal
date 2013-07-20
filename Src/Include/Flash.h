#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

    /*****************************************************************************
     �� �� ��  : NonRegionErase
     ��������  : ����flash����
     �������  : int Fd
                 int start
                 int count
                 int unlock
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��3��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int NonRegionErase(int Fd, int start, int count, int unlock);
    /*****************************************************************************
     �� �� ��  : GetRegionCount
     ��������  : ��ȡ�����ж���block
     �������  : const char *region
     �������  : ��
     �� �� ֵ  :
     ���ú���  :
     ��������  :

     �޸���ʷ      :
      1.��    ��   : 2012��11��3��
        ��    ��   : tango_zhu
        �޸�����   : �����ɺ���

    *****************************************************************************/
    int GetRegionCount(const char *region);



#ifdef __cplusplus
}
#endif

#endif

