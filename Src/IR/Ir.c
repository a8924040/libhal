/*
**  ************************************************************************
**
**  File    Name  : Ir.c
**
**  Description   : ��������ļ�
**  Modification  : 2011/11/24   tango_zhu   ����
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              ͷ�ļ�
**  ************************************************************************
*/
#include "CommonInclude.h"
#include "Common.h"
#include "IR.h"
#include "Log.h"

int fd_ir = -1;
#define IRDEVICES "/dev/ir"

unsigned char ir_buf[4] = { 0 };
unsigned char flag_ir_thd_exit;

pthread_mutex_t irbuf_mutex;           //����ir_buf
/* $Function        :   IrDeviceCreate(void)
==   ===============================================================
==  Description     :   ��ô����豸��dev·��/dev/ir
==  Argument        :   ��
==  Return          :   -2:�豸�Ѿ��򿪣�-1�� ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int IrDeviceCreate(void)
{
    if(fd_ir > 0)
    {
        return -2;//�����ظ���
    }
    if((fd_ir = open(IRDEVICES, O_RDWR)) <= 0)
    {
        LIBHAL_ERROR("open ir devices failed");
        return -1;
    }
    return 0;
}


/* $Function        :   wir_process_thd
==   ===============================================================
==  Description     :   ���պ��������߳�
==  Argument        :   [IN] arg: ��
==  Return          :   0���ɹ�  -1��ʧ��
==  Modification    :   2011/7/19    huang_zhendi    ����
==   ===============================================================
*/
void *ir_process_thd(void *arg)
{
    pthread_detach(pthread_self());
    while(1)
    {
        if(flag_ir_thd_exit == 0)
        {
            pthread_mutex_lock(&irbuf_mutex);
            if(read(fd_ir, ir_buf, 4) < 4)
            {
                pthread_mutex_unlock(&irbuf_mutex);
                uint_msleep(500);
                continue;
            }
            else
            {
                pthread_mutex_unlock(&irbuf_mutex);
                LIBHAL_DEBUG("read result is 0x%x,0x%x,0x%x,0x%x\n", ir_buf[0]
                             , ir_buf[1], ir_buf[2], ir_buf[3]);
                uint_msleep(500);
            }
        }
        else
        {
            break;
        }
    }
    pthread_exit(NULL);
}
/* $Function        :   IrDeviceInit(void)
==   ===============================================================
==  Description     :   ��ʼ�������豸
==  Argument        :   ��
==  Return          :   -1�� ��ʼ��ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int IrDeviceInit(void)
{
    pthread_t ir_process_tid;
    flag_ir_thd_exit = 0;
    pthread_mutex_init(&irbuf_mutex, NULL);
    if(pthread_create(&ir_process_tid, NULL, ir_process_thd, NULL) != 0)
    {
        LIBHAL_ERROR("Fail to create device automaton thread error: %d", errno);
        return -1;
    }
    else
    {
        return 0;
    }
}

/* $Function        :   IrDeviceRead(unsigned char *pbuf)
==   ===============================================================
==  Description     :   �Ӻ����豸������
==  Argument        :   pbuf �Ӻ����豸���������ݵ�buf
==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int IrDeviceRead(unsigned char *pbuf)
{
    memcpy(pbuf, ir_buf, sizeof(ir_buf));
    return 0;
}

/* $Function        :   IrDeviceWrite(unsigned char *pbuf)
==   ===============================================================
==  Description     :   �Ӻ����豸������
==  Argument        :   pbuf �������豸д���ݵ�buf
==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int IrDeviceWrite(unsigned char *pbuf)
{
    return 0;//���ⲻ֧��д
}

/* $Function        :   IrDeviceClose(void)
==   ===============================================================
==  Description     :   �Ӻ����豸������
==  Argument        :   pbuf �������豸д���ݵ�buf
==  Return          :   -1�� ʧ�� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int IrDeviceClose(void)
{
    flag_ir_thd_exit = 1;//�˳�������߳�
    return 0;
}

/* $Function        :   IrDeviceDestory(void)
==   ===============================================================
==  Description     :   ��ô����豸��dev·��/dev/ir
==  Argument        :   ��
==  Return          :   -1�� �豸Ϊ�����豸�б��� 0�� �ɹ�
==  Modification  : 2011/11/24   tango_zhu   ����
==   ===============================================================
*/
int IrDeviceDestory(void)
{
    close(fd_ir);    //
    return 0;
}
