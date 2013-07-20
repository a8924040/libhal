/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  �� �� ��   : Mouse.c
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : ������ݴ���
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��18��
    ��    ��   : tango_zhu
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <linux/input.h>
#include "Log.h"
#include "Mouse.h"
#include "Touchscreen.h"

int     x_y_reverse; /* flag for x & y reverse status */
static pthread_mutex_t OPENINPUTDEVICESMUTEX = PTHREAD_MUTEX_INITIALIZER;

/*----------------------------------------------*
 * �ⲿ����˵��                             *
 *----------------------------------------------*/
extern  CALIBRATESTATE_ENUM     CalibrateFlag;
extern  int handle_touch_screen(MOUSE_DATA *data, int fd);
extern  int mouse_destory(void);

/*----------------------------------------------*
 * �ⲿ����˵��                             *
 *----------------------------------------------*/

extern int handle_input_devices(MOUSE_DATA *data, int fd, char is_ts);
/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
int DevicesFd = -1;
INPUT_DEVICES_INFO_STRU InputDevices[MAX_INPUT_DEVICES] =
{
    { -1, -1, -1},               \
    { -1, -1, -1},                \
    { -1, -1, -1},                 \
    { -1, -1, -1},                    \
};
char *ExcludeMouseDevices[32] =
{
    "mb705-fpbutton",
    NULL
};

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define INPUT_DEVICE "/proc/bus/input/devices"  /*input device node info*/
#define INPUT_DEV   "/dev/input/"          /* dev/input path*/
#define BUFF_MAX                2048

/*****************************************************************************
 �� �� ��  : MouseCreate
 ��������  : �������豸�ļ���ȡȫ�ֱ���DevicesFd
 �������  : void
 �������  : ��
 �� �� ֵ  :-2:�ļ��Ѵ�
            -1:���ļ�ʧ��
             0:���ļ��ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��19��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int MouseCreate(void)
{
    if(DevicesFd > 0)
    {
        DVR_ERR(INPUT_DEVICE"already opened");
        return -2;
    }
    if(access(INPUT_DEVICE, R_OK) < 0)
    {
        DVR_ERR("access input devices failed!");
        return -1;
    }
    if((DevicesFd = open(INPUT_DEVICE, O_RDONLY)) < 0)
    {
        DVR_ERR("open input devices failed!");
    }
    else
    {
        return 0;
    }
    return -1;
}


/*****************************************************************************
 �� �� ��  : open_input_devices
 ��������  : �������豸���豸������
 �������  : int index
             char *pevent
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��26��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
static int open_input_devices(int index, char *pevent)
{
    char name_dev[MAX_PATH];
    memset(name_dev, 0, MAX_PATH);
    /*if mouse_node not NULL,open mouse_node*/
    if(NULL != pevent)
    {
        /* ��event �豸 */
        sprintf(name_dev, "%s%s", INPUT_DEV, pevent);
        InputDevices[index].fd_event = open(name_dev, O_RDWR | O_NONBLOCK);
        if(InputDevices[index].fd_event < 0)
        {
            DVR_ERR("Open dev %s fail.\n", name_dev);
            goto fail;
        }
        sprintf(InputDevices[index].device_node, "%s", pevent);
    }
    /* get device name */
    if(ioctl(InputDevices[index].fd_event, EVIOCGNAME(MAX_PATH - 1),
             name_dev) < 0)
    {
        DVR_MSG("Search input device type fail.\n");
        goto fail;
    }
    if(strstr(name_dev, "Touch")
            || strstr(name_dev, "TOUCH")
            || strstr(name_dev, "touch"))
    {
        /*touch device*/
        InputDevices[index].is_ts = 1;
    }
    else
    {
        /*mouse device*/
        InputDevices[index].is_ts = 0;
    }
    return 0;
fail:
    if(InputDevices[index].fd_event > 0)
    {
        close(InputDevices[index].fd_event);
        InputDevices[index].fd_event = -1;
    }
    return -1;
}

/*****************************************************************************
 �� �� ��  : parse_input_devices
 ��������  : ���������豸�����InputDevices�ṹ��
 �������  : char *pstr
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��25��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
static int parse_input_devices(char *pstr)
{
    char *pevent = NULL;
    int i;
    int j_index = 0;
    char flag_fd = 0;
    if(NULL != (pevent = strstr(pstr, "event"))
            && NULL == strstr(pstr, "kbd"))  /* ȥ������������д�kbd
                                                ��even �Ľڵ� */
    {
        * (pevent + 6) = '\0';
        for(i  = 0; i < MAX_INPUT_DEVICES; i++)
        {
            if(strstr(InputDevices[i].device_node, pevent))
            {
                break;
            }
            if((flag_fd == 0 && InputDevices[i].fd_event < 0))
            {
                j_index = i;
                flag_fd = 1;/* �ҵ���һ��fd_event ΪС��0 �Ĵ洢λ�� */
            }
        }
        /* ֻ���ڽڵ�û��ʱ�Ŵ���Ӧ���豸�ڵ� */
        if(MAX_INPUT_DEVICES == i)
        {
            if(open_input_devices(j_index, pevent) < 0)
            {
                DVR_ERR("open input devices failed\n");
                return -1;
            }
        }
    }
    return 0;
}


/*****************************************************************************
 �� �� ��  : detect_input_devices
 ��������  : ̽�������豸
 �������  : void
 �������  : ��
 �� �� ֵ  : -1 ̽��ʧ��
              0 ̽��ɹ�
              1 ��겻����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��26��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int detect_input_devices(void)
{
    char buffer_read[BUFF_MAX] = { 0 };
    char *strtmp = NULL;
    char *pstr = NULL;
    char *pdevices = NULL;
    char flag_mb705 = 0;
    int i;
    int read_sum = 0;
    int ret = -1;
    if(DevicesFd < 0)
    {
        DVR_ERR("input devices file  description error!");
        goto fail;
    }
    /* �ָ��ļ�ָ�뵽�ļ�ͷ */
    lseek(DevicesFd, 0, SEEK_SET);
    if((read_sum = read(DevicesFd, buffer_read, BUFF_MAX)) <= 0)
    {
        /* ��ö�ȡ�ļ��Ĵ�С���Ϊ0 ���ʾû�н���꣬С��0 ���ʾ���� */
        if(read_sum == 0)
        {
            ret = 1;
        }
        goto fail;
    }
    strtmp = strtok(buffer_read, "\n");
    while(NULL != (strtmp = strtok(NULL, "\n")))
    {
        /*���˵���Щ�ں�һ��Ĭ�ϵ�mb705-fpbutton������ɨ�赽���������*/
        i = 0;
        if(strstr(strtmp, "Name="))
        {
            while((pdevices = ExcludeMouseDevices[i]) != NULL)
            {
                if(NULL != strstr(strtmp, pdevices))
                {
                    flag_mb705 = 1;
                    break;
                }
                i ++;
            }
        }
        if((pstr = strstr(strtmp, "Handlers")))
        {
            if(flag_mb705)
            {
                flag_mb705 = 0;
                continue;
            }
            /* ÿ�ҵ�һ������豸���������� */
            /* �ڴ˴���������Ϊ���̻߳�ȡ����ʱ�ᵼ���ظ���fd ����fd й© */
            pthread_mutex_lock(&OPENINPUTDEVICESMUTEX);
            parse_input_devices(pstr);
            pthread_mutex_unlock(&OPENINPUTDEVICESMUTEX);
        }
    }
    return 0;
fail:
    return  ret;
}

/*****************************************************************************
 �� �� ��  : mouse_destory
 ��������  : ��������豸���رմ򿪵������豸���ļ�������
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int mouse_destory(void)
{
    int i;
    for(i = 0; i < MAX_INPUT_DEVICES; i++)
    {
        if(InputDevices[i].fd_event > 0)
        {
            close(InputDevices[i].fd_event);
            InputDevices[i].fd_event = -1;
            InputDevices[i].is_ts = -1;
            InputDevices[i].device_node[0] = '\0';
        }
    }
    DVR_MSG("MouseDestroy:\n");
    return 0;
}

#if 0
/*****************************************************************************
 �� �� ��  : handle_mouse
 ��������  : �����������
 �������  : MOUSE_DATA *data
             int fd
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��3��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
static int handle_mouse(MOUSE_DATA *data, int fd)
{
    char buf[12];
    int ret = -1;
    memset(buf, 0, sizeof(buf));
    if(fd < 0)
    {
        DVR_MSG("Device mouse not exist\n");
        return -1;
    }
    if((ret = read(fd, buf, sizeof(buf))) < 0)
    {
        DVR_MSG("Mouse not exist now.\n");
        return ret;
    }
    data->key   = (buf[0] & 0x07);
    data->x     = buf[1];
    data->y     = buf[2];
    data->z     = buf[3];
    data->ax    = (buf[4] & 0xFF) | (buf[5] << 8);
    data->ay    = (buf[6] & 0xFF) | (buf[7] << 8);
    return 0;
}
#endif
/*****************************************************************************
 �� �� ��  : mouse_get_data
 ��������  : �ڲ���������ȡ���ݵľ���ʵ��
 �������  : MOUSE_DATA *data
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��26��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int mouse_get_data(MOUSE_DATA *data, MOUSEGETDATATYPE_ENUM type)
{
    int i;
    int mouse_valid = 0;
    fd_set readset;
    struct timeval tv;
    int max_fd = -1;
    int ret = -1;
    FD_ZERO(&readset);
    if(type == MouseGetDataNormal)
    {
        for(i = 0; i < MAX_INPUT_DEVICES; i++)
        {
            if(InputDevices[i].fd_event > 0)
            {
                /* �ڴ�����У׼��ʱ����ⴥ����fd����select,���ɻ�ȡ������� */
                if(CalibrateStart == CalibrateFlag && InputDevices[i].is_ts)
                {
                    continue;
                }
                mouse_valid = 1;
                FD_SET(InputDevices[i].fd_event, &readset);
                max_fd = MAX(InputDevices[i].fd_event, max_fd);
            }
        }
    }
    else
    {
        for(i = 0; i < MAX_INPUT_DEVICES; i++)
        {
            if(InputDevices[i].fd_event > 0)
            {
                /* �ڴ�����У׼ʱֻ��������fd ����select */
                if(InputDevices[i].is_ts)
                {
                    mouse_valid = 1;
                    FD_SET(InputDevices[i].fd_event, &readset);
                    max_fd = MAX(InputDevices[i].fd_event, max_fd);
                }
            }
        }
    }
    if(!mouse_valid)
    {
        return -1;
    }
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if(max_fd > 0)
    {
        ret = select(max_fd + 1, &readset, NULL, NULL, &tv);
    }
    if(ret == 0)
    {
        return 0;/* �ڴ�����У׼ʱ�ڶ���ʱ������0  ʹУ׼����������ѭ����ȡ */
    }
    if(ret < 0)
    {
        goto fail1;
    }
    for(i = 0; i < MAX_INPUT_DEVICES; i++)
    {
        if(InputDevices[i].fd_event > 0
                && FD_ISSET(InputDevices[i].fd_event, &readset))
        {
            ret = handle_input_devices(data, InputDevices[i].fd_event
                                       , InputDevices[i].is_ts);
            if(ret < 0)
            {
                goto fail;
            }
            return ret;
        }
    }
    return 0;
fail1:
    DVR_ERR("input device Select:%s!\n", strerror(errno));
fail:
    mouse_destory();
    return -1;
}


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
int MouseGetData(MOUSE_DATA *data)
{
    int ret = 0;
    if(NULL ==  data)
    {
        DVR_ERR("mouse get data invalided parameter!");
        return -1;
    }
    if((ret = detect_input_devices()) < 0)
    {
        DVR_ERR("detect input devices failed!");
        return -1;
    }
    else
    {
        /* ����ϴ�������¼����㣬�Ա�֤��select���ص�ʱ��������겻��*/
        /* ����Ǵ����������ϴ����ݣ��Ա�֤��ͷ���Ƶ�ʱ��λ�ò���*/
        if(data->key == 0 || data->key == 1)
        {
            memset((char *) data + 1, 0, sizeof(MOUSE_DATA) - 1);
        }
        /* �����겻������ô����0  */
        if(ret == 1)
        {
            return 0;
        }
    }
    return mouse_get_data(data, MouseGetDataNormal);
}
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
int MouseDestroy(void)
{
    /* �ر������豸�ļ� */
    if(DevicesFd > 0)
    {
        close(DevicesFd);
        DevicesFd = -1;
    }
    return mouse_destory();
}



