/*
**  ************************************************************************
**
**  File    Name  : Ir.c
**
**  Description   : 红外相关文件
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
/*
**  ************************************************************************
**                              头文件
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

pthread_mutex_t irbuf_mutex;           //保护ir_buf
/* $Function        :   IrDeviceCreate(void)
==   ===============================================================
==  Description     :   获得串口设备的dev路径/dev/ir
==  Argument        :   无
==  Return          :   -2:设备已经打开：-1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int IrDeviceCreate(void)
{
    if(fd_ir > 0)
    {
        return -2;//避免重复打开
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
==  Description     :   接收红外数据线程
==  Argument        :   [IN] arg: 无
==  Return          :   0：成功  -1：失败
==  Modification    :   2011/7/19    huang_zhendi    创建
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
==  Description     :   初始化红外设备
==  Argument        :   无
==  Return          :   -1： 初始化失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
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
==  Description     :   从红外设备读数据
==  Argument        :   pbuf 从红外设备读到的数据的buf
==  Return          :   -1： 设备为不在设备列表内 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int IrDeviceRead(unsigned char *pbuf)
{
    memcpy(pbuf, ir_buf, sizeof(ir_buf));
    return 0;
}

/* $Function        :   IrDeviceWrite(unsigned char *pbuf)
==   ===============================================================
==  Description     :   从红外设备读数据
==  Argument        :   pbuf 往红外设备写数据的buf
==  Return          :   -1： 设备为不在设备列表内 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int IrDeviceWrite(unsigned char *pbuf)
{
    return 0;//红外不支持写
}

/* $Function        :   IrDeviceClose(void)
==   ===============================================================
==  Description     :   从红外设备读数据
==  Argument        :   pbuf 往红外设备写数据的buf
==  Return          :   -1： 失败 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int IrDeviceClose(void)
{
    flag_ir_thd_exit = 1;//退出红外读线程
    return 0;
}

/* $Function        :   IrDeviceDestory(void)
==   ===============================================================
==  Description     :   获得串口设备的dev路径/dev/ir
==  Argument        :   无
==  Return          :   -1： 设备为不在设备列表内 0： 成功
==  Modification  : 2011/11/24   tango_zhu   创建
==   ===============================================================
*/
int IrDeviceDestory(void)
{
    close(fd_ir);    //
    return 0;
}
