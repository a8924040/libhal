/*
**  ************************************************************************
**
**  File    Name  : IR.h
**  Description   : �������ͷ�ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
#ifndef __IR_H__
#define __IR_H__

#ifdef __cplusplus
extern "C" {
#endif

    /* $Function        :   IrDeviceCreate(void)
    ==   ===============================================================
    ==  Description     :   ��ô����豸��dev·��/dev/ir
    ==  Argument        :   ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int IrDeviceCreate(void);

    /* $Function        :   IrDeviceInit(void)
    ==   ===============================================================
    ==  Description     :   ��ʼ�������豸
    ==  Argument        :   ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int IrDeviceInit(void);

    /* $Function        :   IrDeviceRead(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   �Ӻ����豸������
    ==  Argument        :   pbuf �Ӻ����豸���������ݵ�buf
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int IrDeviceRead(unsigned char *pbuf);

    /* $Function        :   IrDeviceWrite(unsigned char *pbuf)
    ==   ===============================================================
    ==  Description     :   �Ӻ����豸������
    ==  Argument        :   pbuf �������豸д���ݵ�buf
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int IrDeviceWrite(unsigned char *pbuf);

    /* $Function        :   IrDeviceClose(void)
    ==   ===============================================================
    ==  Description     :   �Ӻ����豸������
    ==  Argument        :   pbuf �������豸д���ݵ�buf
    ==  Return          :   -1�� ʧ�� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int IrDeviceClose(void);

    /* $Function        :   IrDeviceDestory(void)
    ==   ===============================================================
    ==  Description     :   ��ô����豸��dev·��/dev/ir
    ==  Argument        :   ��
    ==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
    ==  Modification  : 2011/11/24   tango_zhu   ����
    ==   ===============================================================
    */
    int IrDeviceDestory(void);

#ifdef __cplusplus
}
#endif

#endif
