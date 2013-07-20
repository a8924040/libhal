/*
**  ************************************************************************
**                                  dvr
**                      Digital Video Recoder xp
**
**          (c) Copyright 1992-2004, ZheJiang Dahua Technology Stock Co.Ltd.
**                          All Rights Reserved
**
**  File    Name  : debug.h
**  Description   : ��־ͷ�ļ�
**  Modification  : 2011/9/6    huang_zhendi    ����
**  ************************************************************************
*/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

    /*
    **  ************************************************************************
    **                             �������Ͷ���
    **  ************************************************************************
    */
    typedef int (*DEBUG_CMD_FUNC)(int count, char *input[]);      //��������ִ�еĲ���

    /*
    **  ************************************************************************
    **                             ��������
    **  ************************************************************************
    */
    /* $Function        :   int init_system_debug(char *mapfile, char *debugfifo)
    ==   ===============================================================
    ==  Description     :   ��ʼ��������Դ
    ==  Argument        :   [IN] mapfile: ����MAP�ļ�·��
                            [IN] debugfifo: ��������fifo�ļ�·��
    ==  Return          :   0: �ɹ�  -1: ʧ��
    ==  Modification    :   2011/9/6    huang_zhendi    ����
    ==   ===============================================================
    */
    int init_system_debug(char *mapfile, char *debugfifo);

    /* $Function        :   register_system_debug_cmd
    ==   ===============================================================
    ==  Description     :   ע��һ����������
    ==  Argument        :   [IN] cmd: �����ַ���
                            [IN] func: ����ִ�к���
    ==  Return          :   0: �ɹ� -1: ʧ��
    ==  Modification    :   2011/9/25    huang_zhendi    ����
    ==   ===============================================================
    */
    int register_system_debug_cmd(char *cmd, DEBUG_CMD_FUNC func);

#ifdef __cplusplus
}
#endif

#endif //__DEBUG_H__

