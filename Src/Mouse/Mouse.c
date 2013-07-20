/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : Mouse.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : 鼠标数据处理
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月18日
    作    者   : tango_zhu
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
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
 * 外部变量说明                             *
 *----------------------------------------------*/
extern  CALIBRATESTATE_ENUM     CalibrateFlag;
extern  int handle_touch_screen(MOUSE_DATA *data, int fd);
extern  int mouse_destory(void);

/*----------------------------------------------*
 * 外部函数说明                             *
 *----------------------------------------------*/

extern int handle_input_devices(MOUSE_DATA *data, int fd, char is_ts);
/*----------------------------------------------*
 * 常量定义                                     *
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
 * 宏定义                                       *
 *----------------------------------------------*/
#define INPUT_DEVICE "/proc/bus/input/devices"  /*input device node info*/
#define INPUT_DEV   "/dev/input/"          /* dev/input path*/
#define BUFF_MAX                2048

/*****************************************************************************
 函 数 名  : MouseCreate
 功能描述  : 打开输入设备文件获取全局变量DevicesFd
 输入参数  : void
 输出参数  : 无
 返 回 值  :-2:文件已打开
            -1:打开文件失败
             0:打开文件成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月19日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : open_input_devices
 功能描述  : 获得鼠标设备的设备描述符
 输入参数  : int index
             char *pevent
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int open_input_devices(int index, char *pevent)
{
    char name_dev[MAX_PATH];
    memset(name_dev, 0, MAX_PATH);
    /*if mouse_node not NULL,open mouse_node*/
    if(NULL != pevent)
    {
        /* 打开event 设备 */
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
 函 数 名  : parse_input_devices
 功能描述  : 处理输入设备，填充InputDevices结构体
 输入参数  : char *pstr
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月25日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int parse_input_devices(char *pstr)
{
    char *pevent = NULL;
    int i;
    int j_index = 0;
    char flag_fd = 0;
    if(NULL != (pevent = strstr(pstr, "event"))
            && NULL == strstr(pstr, "kbd"))  /* 去掉在无线鼠标中带kbd
                                                和even 的节点 */
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
                flag_fd = 1;/* 找到第一个fd_event 为小于0 的存储位置 */
            }
        }
        /* 只有在节点没打开时才打开相应的设备节点 */
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
 函 数 名  : detect_input_devices
 功能描述  : 探测输入设备
 输入参数  : void
 输出参数  : 无
 返 回 值  : -1 探测失败
              0 探测成功
              1 鼠标不存在
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
    /* 恢复文件指针到文件头 */
    lseek(DevicesFd, 0, SEEK_SET);
    if((read_sum = read(DevicesFd, buffer_read, BUFF_MAX)) <= 0)
    {
        /* 获得读取文件的大小如果为0 则表示没有接鼠标，小于0 则表示出错 */
        if(read_sum == 0)
        {
            ret = 1;
        }
        goto fail;
    }
    strtmp = strtok(buffer_read, "\n");
    while(NULL != (strtmp = strtok(NULL, "\n")))
    {
        /*过滤掉有些内核一个默认的mb705-fpbutton，保留扫描到得其他鼠标*/
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
            /* 每找到一个鼠标设备就立即处理 */
            /* 在此处加锁是因为多线程获取数据时会导致重复打开fd 导致fd 泄漏 */
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
 函 数 名  : mouse_destory
 功能描述  : 销毁鼠标设备，关闭打开的输入设备的文件描述符
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : handle_mouse
 功能描述  : 处理鼠标数据
 输入参数  : MOUSE_DATA *data
             int fd
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月3日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
 函 数 名  : mouse_get_data
 功能描述  : 内部函数鼠标获取数据的具体实现
 输入参数  : MOUSE_DATA *data
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
                /* 在触摸屏校准的时候避免触摸屏fd进入select,但可获取鼠标数据 */
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
                /* 在触摸屏校准时只允许触摸屏fd 进入select */
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
        return 0;/* 在触摸屏校准时在读超时，返回0  使校准函数阻塞，循环读取 */
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
 函 数 名  : MouseGetData
 功能描述  : 获取鼠标数据 等到获得整个鼠标事件以后返回鼠标数据
 输入参数  : MOUSE_DATA *data
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

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
        /* 如果上次是鼠标事件清零，以保证再select返回的时候，鼠标坐标不变*/
        /* 如果是触摸屏则保留上次数据，以保证箭头绘制的时候位置不变*/
        if(data->key == 0 || data->key == 1)
        {
            memset((char *) data + 1, 0, sizeof(MOUSE_DATA) - 1);
        }
        /* 如果鼠标不存在那么返回0  */
        if(ret == 1)
        {
            return 0;
        }
    }
    return mouse_get_data(data, MouseGetDataNormal);
}
/*****************************************************************************
 函 数 名  : MouseDestroy
 功能描述  : 关闭所有打开的鼠标设备文件描述符，初始化设备数组
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月26日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int MouseDestroy(void)
{
    /* 关闭输入设备文件 */
    if(DevicesFd > 0)
    {
        close(DevicesFd);
        DevicesFd = -1;
    }
    return mouse_destory();
}



