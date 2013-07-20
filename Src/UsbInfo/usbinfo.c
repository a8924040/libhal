/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : Usbinfo.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : usb信息获取
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
#include "usbinfo.h"

#define LINUX_KERNEL_VERSION    "/proc/version"
#define KERNEL_VERSION          "Linux version"
#define USB_DEVICE_FILE     "/proc/bus/usb/devices"
#define USB_SERIAL_DIR      "/sys/bus/usb-serial/devices"
#define USB_WIFI_DIR      "/sys/bus/usb/devices/"
#define FOPEN_RETRY_TIMES   5
#define USB_SUBDEV_COUNT    8               /*一个USB接口对应的设备个数  */
#define USB_DEV_MAX         16
#define HUB_PRODID_2514     0x2514
/* add by heliangbin  */
#define HUB_PRODID_6254 0x6254


static KERNERL_VERSION_STRU stVersion;
static USB_DRIVER_INFO_STRU gastUsbDriverInfoTable[] = { {USB_SERIAL, {"usbserial_generic"}}, {USB_WIFI, {"rt2870"}},};


/* 描述3g wifi 对应的usb 端口和对应的复位引脚 */
static USB_PORT_RES_STRU gastUsbDevInfo =
{
    .s32UsbNum = 9,
    .astUsbPort =
    {

        [0] =
        {
            .s32PortIndex = 0,
            .s32UsbBus = 1,
            .s32UsbLev = 1,
            .s32UsbPort = 0,
        },
        [1] =
        {
            .s32PortIndex = 0,
            .s32UsbBus = 2,
            .s32UsbLev = 1,
            .s32UsbPort = 0,
        },
        [2] =
        {
            .s32PortIndex = 1,
            .s32UsbBus = 3,
            .s32UsbLev = 1,
            .s32UsbPort = 0,
        },
        [3] =
        {
            .s32PortIndex = 1,
            .s32UsbBus = 4,
            .s32UsbLev = 1,
            .s32UsbPort = 0,
        },
        [4] =
        {
            .s32PortIndex = 2,
            .s32UsbBus = 5,
            .s32UsbLev = 1,
            .s32UsbPort = 0,
        },
        [5] =
        {
            .s32PortIndex = 2,
            .s32UsbBus = 6,
            .s32UsbLev = 1,
            .s32UsbPort = 0,
        },
        [6] =
        {
            .s32PortIndex = 2,
            .s32UsbBus = 5,
            .s32UsbLev = 2,
            .s32UsbPort = 0,
        },
        [7] =
        {
            .s32PortIndex = 3,
            .s32UsbBus = 5,
            .s32UsbLev = 2,
            .s32UsbPort = 1,
        },
        [8] =
        {
            .s32PortIndex = 3,
            .s32UsbBus = 3,
            .s32UsbLev = 2,
            .s32UsbPort = 1,
        },
    }

};



/* 但设备带有2514 hub芯片时，需要把2514的信息存储起来  */
static USB_DEV_INFO gstUsbHubDev =
{
    .bus = 255,
    .lev = 255,
    .prnt = 255,
    .port = 255,
};

/* add by heliangbin  */
int GetKernelVersion(void)
{
    FILE *fp = NULL;
    S8 as8Buf[128];
    S8 *pAddr = NULL;
    S32 Ver1 = 0;
    S32 Ver2 = 0;
    S32 Ver3 = 0;
    S32 s32Ret = -1;
    if((fp = fopen(LINUX_KERNEL_VERSION, "r")) == NULL)
    {
        LIBHAL_ERROR("open %s fail!\n", LINUX_KERNEL_VERSION);
        goto fail;
    }
    if(fgets(as8Buf, 128, fp) == NULL)
    {
        LIBHAL_ERROR("fgets value of kernel version fail!\n");
        fclose(fp);
        goto fail;
    }
    fclose(fp);
    pAddr = strstr(as8Buf, KERNEL_VERSION);
    if(pAddr != NULL)
    {
        pAddr += strlen(KERNEL_VERSION);
        s32Ret = sscanf(pAddr, " %d.%d.%d", &Ver1, &Ver2, &Ver3);
        if(s32Ret < 0)
        {
            LIBHAL_ERROR("sscanf fail!\n");
        }
        LIBHAL_ALARM("kernel version = %d.%d.%d\n", Ver1, Ver2, Ver3);
    }
    else
    {
        LIBHAL_ERROR("do't find kernel version!\n");
        goto fail;
    }
    stVersion.ver_first = Ver1;
    stVersion.ver_sec = Ver2;
    stVersion.ver_third = Ver3;
    return 0;
fail:
    return -1;
}

/*****************************************************************************
 函 数 名  : safe_fopen
 功能描述  : 安全打开 打开失败后重复尝试 FOPEN_RETRY_TIMES
 输入参数  : const char *path
             FILE **fp
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月13日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int SafeOpen(const S8 *ps8Path, FILE **fp)
{
    U32 u32Index = 0;
    for(u32Index = 0; u32Index < FOPEN_RETRY_TIMES; u32Index++)
    {
        if(NULL == (*fp = fopen(ps8Path, "r")))
        {
            usleep(500 * 1000);
        }
        else
        {
            break;
        }
    }
    if(FOPEN_RETRY_TIMES == u32Index)
    {
        LIBHAL_ERROR("fail to open %s!\n", ps8Path);
        return -1;
    }
    return 0;
}


/*****************************************************************************
 函 数 名  : MatchDriver
 功能描述  : 根据得到的驱动名字匹配驱动类型
 输入参数  : char *driver
             USB_DRIVER_TYPE *pusb_driver_type
 输出参数  : 无
 返 回 值  : -1:参数出错
             0: 找到匹配的
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月14日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int MatchDriver(S8 *ps8Driver, USB_DRIVER_TYPE *penUsbDriverType)
{
    U32 u8Index = 0;
    if(NULL == ps8Driver)
    {
        LIBHAL_ERROR("match driver param error!");
        goto fail;
    }
    for(u8Index = 0; u8Index < sizeof(gastUsbDriverInfoTable) / sizeof(USB_DRIVER_INFO_STRU);)
    {
        if(0 == strncmp(gastUsbDriverInfoTable[u8Index].driver_name, ps8Driver
                        , strlen(gastUsbDriverInfoTable[u8Index].driver_name)))
        {
            *penUsbDriverType = gastUsbDriverInfoTable[u8Index].driver_type;
            return 0;
        }
        u8Index ++;
    }
fail:
    return -1;
}

/*****************************************************************************
 函 数 名  : GetInterfaceIndex
 功能描述  : 根据busb,level,port,parent 获取接口号
 输入参数  : int bus
             int level
             int port
             int parent
             int count
             USB_DEV_DESC *pstDesc
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月16日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int GetInterfaceIndex(U32 u32Bus, U32 u32Level, U32 u32Port
                             , U32 *pu32UsbIndex)
{
    U32 u32Index = 0;
    if(NULL == pu32UsbIndex)
    {
        LIBHAL_ERROR("GetInterfaceIndex param error!\n");
    }
#ifdef DEBUG
    LIBHAL_ALARM("GetInterfaceIndex Bus = 0x%x,Level = 0x%x,Port = 0x%x!\n"
                 , u32Bus, u32Level, u32Port);
#endif
    for(u32Index = 0; u32Index < gastUsbDevInfo.s32UsbNum; u32Index++)
    {
        if(gastUsbDevInfo.astUsbPort[u32Index].s32UsbBus == u32Bus
                && gastUsbDevInfo.astUsbPort[u32Index].s32UsbLev == u32Level
                && gastUsbDevInfo.astUsbPort[u32Index].s32UsbPort == u32Port)
        {
            *pu32UsbIndex = gastUsbDevInfo.astUsbPort[u32Index].s32PortIndex;
            return 0;
        }
    }
    return -1;
}



/*****************************************************************************
 函 数 名  : GetWifiSubDevs
 功能描述  : 获取wifi设备的子设备，获取网卡名
 输入参数  : int bus
             int level
             int port
             int parent
             int count
             USB_DEV_DESC *pstDesc
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月16日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int GetWifiSubDevs(U32 u32Bus, U32 u32Level, U32 u32Port, U32 u32Parent
                          , U32 u32Count, USB_DEV_DESC *pstDesc)
{
    DIR *pstDir = NULL;
    struct dirent *pstDirent = NULL;
    S8 s8aWifiDir[128] = {0};
    S8 *ps8Str = NULL;
    U32 u32WlanIndex = 0;
    if(NULL == pstDesc)
    {
        LIBHAL_ERROR("GetWifiSubDevs param error!");
        goto fail;
    }
    /* fix me */
    /* usb 网卡的目录在由bus和count的组成的net目录下 */
    /* modfiy by heliangbin */
    if((0 == stVersion.ver_first) && (0 == stVersion.ver_first) && (0 == stVersion.ver_first))
    {
        LIBHAL_ERROR("invalid kernel version!\n");
        goto fail;
    }
    if(stVersion.ver_third < LINUX_2_6_23)
    {
        if(gstUsbHubDev.bus == u32Bus)
        {
            sprintf(s8aWifiDir, USB_WIFI_DIR"%d-%d.%d", u32Bus, gstUsbHubDev.lev, u32Port + 1);
        }
        else
        {
            sprintf(s8aWifiDir, USB_WIFI_DIR"%d-%d", u32Bus, u32Port + 1);
        }
    }
    else
    {
        if(gstUsbHubDev.bus == u32Bus)
        {
            sprintf(s8aWifiDir, USB_WIFI_DIR"%d-%d.%d/net", u32Bus, gstUsbHubDev.lev, u32Port + 1);
        }
        else
        {
            sprintf(s8aWifiDir, USB_WIFI_DIR"%d-%d/net", u32Bus, u32Port + 1);
        }
    }
    LIBHAL_DEBUG("the s8aWifiDir is %s\n", s8aWifiDir);
    pstDir = opendir(s8aWifiDir);
    if(NULL == pstDir)
    {
        LIBHAL_ERROR("open %s failed!", s8aWifiDir);
        goto fail;
    }
    while((pstDirent = readdir(pstDir)) != NULL)
    {
        if((strcmp(pstDirent->d_name, ".") == 0)
                || (strcmp(pstDirent->d_name, "..") == 0))
        {
            continue;
        }
        /* modify by heliangbin */
        if(stVersion.ver_third < LINUX_2_6_23)
        {
            ps8Str = strstr(pstDirent->d_name, "wlan");
            if(ps8Str != NULL)
            {
                /* 复制设备名称 */
                strncpy(pstDesc->subdev[u32WlanIndex].name, ps8Str, strlen("wlan0"));
                /* 设置设备类型 */
                pstDesc->subdev[u32WlanIndex].driver_type = USB_WIFI;
                u32WlanIndex ++;
                /* 填充wlan 子设备的个数 */
                pstDesc->subdev_count =  u32WlanIndex;
            }
        }
        else
        {
            /* 复制设备名称 */
            strncpy(pstDesc->subdev[u32WlanIndex].name, pstDirent->d_name
                    , strlen(pstDirent->d_name));
            /* 设置设备类型 */
            pstDesc->subdev[u32WlanIndex].driver_type = USB_WIFI;
            u32WlanIndex ++;
            /* 填充wlan 子设备的个数 */
            pstDesc->subdev_count =  u32WlanIndex;
        }
    }
    closedir(pstDir);
    return 0;
fail:
    if(pstDir > 0)
    {
        closedir(pstDir);
    }
    return -1;
}
/*****************************************************************************
 函 数 名  : GetUsbSerialSubDevs
 功能描述  : 根据usb devices 文件中的level bus,parent port 获取子设备信息
 输入参数  : int bus
             int level
             int port
             int parent
             USB_DEV_DESC pstDesc
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月14日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int GetUsbSerialSubDevs(U32 u32Bus, U32 u32Level, U32 u32Port
                               , U32 u32Parent, U32 u32Count
                               , USB_DEV_DESC *pstDesc)
{
    DIR *pstDir = NULL;
    struct dirent *pstDirent;
    struct stat stFileState;
    S8 s8aTtyusbDir[128] = {0};
    S8 s8aTtyusbLink[128] = {0};
    S8 s8aBusName[32] = {0};
    S8 *s8pTtyusb =  NULL;
    S8 *ps8BusName =  NULL;
    U8 u8TtyIndex = 0;
    if(NULL == pstDesc)
    {
        LIBHAL_ERROR("get usb serial subdevs param error!");
        goto fail;
    }
    pstDir = opendir(USB_SERIAL_DIR);
    if(NULL == pstDir)
    {
        LIBHAL_ERROR("open %s failed!\n", USB_SERIAL_DIR);
        goto fail;
    }
    /* fix me */
    /* 在和hub芯片的bus 相同时使用%d-%d.%d 目录 */
    if(gstUsbHubDev.bus == u32Bus)
    {
        sprintf(s8aBusName, "%d-%d.%d", u32Bus, gstUsbHubDev.lev, u32Port + 1);
    }
    else
    {
        sprintf(s8aBusName, "%d-%d", u32Bus, u32Port + 1);
    }
    LIBHAL_ERROR("the bus name is %s\n", s8aBusName);
    while((pstDirent = readdir(pstDir)) != NULL)
    {
        if((strcmp(pstDirent->d_name, ".") == 0)
                || (strcmp(pstDirent->d_name, "..") == 0))
        {
            continue;
        }
        /* 获得ttyUSB* 的路径 */
        sprintf(s8aTtyusbDir, "%s/%s", USB_SERIAL_DIR, pstDirent->d_name);
        if(lstat(s8aTtyusbDir, &stFileState) < 0)
        {
            LIBHAL_ERROR("lstat %s failed", s8aTtyusbDir);
            goto fail;
        }
        /* ttyUSB* 是链接文件，读取链接文件所指向的文件 */
        if(S_ISLNK(stFileState.st_mode))
        {
            readlink(s8aTtyusbDir, s8aTtyusbLink, 128);
            s8pTtyusb = strrchr(s8aTtyusbLink, '/');
            *s8pTtyusb = '\0';
            ps8BusName  = strrchr(s8aTtyusbLink, '/');
            /* 总线编号 和参数相符合 后进行相关的操作 */
            if(!strncmp(ps8BusName + 1, s8aBusName, strlen(s8aBusName)))
            {
                /* 复制设备名称 */
                strncpy(pstDesc->subdev[u8TtyIndex].name, s8pTtyusb + 1
                        , strlen(s8pTtyusb + 1));
                /* 设置设备类型 */
                pstDesc->subdev[u8TtyIndex].driver_type = USB_SERIAL;
                u8TtyIndex ++;
                /* 填充ttyusb 子设备的个数 */
                pstDesc->subdev_count =  u8TtyIndex;
            }
        }
    }
    closedir(pstDir);
    return 0;
fail:
    if(pstDir > 0)
    {
        closedir(pstDir);
    }
    return -1;
}


/*****************************************************************************
 函 数 名  : process_usb_info
 功能描述  : 分析/proc/bus/usb/devices 文件获取相应的参数
 输入参数  : FILE *fp
             USB_DEV_DESC *pstDesc
             unsigned int *pu32Count
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月18日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
static int ProcessUsbInfo(FILE *fp, USB_DEV_DESC *pstDesc
                          , U32 *pu32Count)
{
    S8  as8Buf[256] = {0};
    U32 u32Bus = 0;
    U32 u32Level = 0;
    U32 u32Parent = 0;
    U32 u32Port = 0;
    U32 u32Count = 0;
    U32 u32Vendor = 0;
    U32 u32Prodid = 0;
    U32 u32Subdev = 0;
    S32 s32ReturnValue = 0;
    S8  as8Manufacture[32] = {0};
    S8 *ps8Manufacture =  NULL;
    S8 as8Product[32] = {0};
    S8 *ps8Product =  NULL;
    S8 as8Driver[32] = {0};
    USB_DRIVER_TYPE  enUsbDriverType;
    S8 *ps8Driver =  NULL;
    S32 s32RetVal = 0;
    U32 u32UsbIndex = 0;
    S8 *ps8Str = NULL;
    if(NULL == fp || NULL == pstDesc || NULL == pu32Count)
    {
        LIBHAL_ERROR(" process usb info param error!");
        goto fail;
    }
    *pu32Count = 0;
    while(NULL != fgets(as8Buf, sizeof(as8Buf), fp))
    {
        if(NULL != strstr(as8Buf, "T:"))    /* 找到拓扑分支后开始处理 */
        {
            memset(as8Manufacture, 0, sizeof(as8Manufacture));
            memset(as8Product, 0, sizeof(as8Product));
            memset(as8Driver, 0, sizeof(as8Driver));
            sscanf(as8Buf, "T:	Bus=%d Lev=%d Prnt=%d Port=%d Cnt=%d", &u32Bus
                   , &u32Level, &u32Parent, &u32Port, &u32Count);
            do
            {
                ps8Str = fgets(as8Buf, sizeof(as8Buf), fp);
                if(NULL != strstr(as8Buf, "P:"))    /* 处理P分支 PRODUCT ID信息 */
                {
                    sscanf(as8Buf, "P:  Vendor=%x ProdID=%x", &u32Vendor
                           , &u32Prodid);
                    /* 如果vendor为0x1d6b，则不处理 */
                    if(u32Vendor == 0x1d6b || (u32Vendor == 0 && u32Prodid == 0))
                    {
                        break;
                    }
                }
                else if(NULL != strstr(as8Buf, "S:"))       /* 处理S分支 描述信息 */
                {
                    if(NULL != strstr(as8Buf, "Manufacturer"))
                    {
                        ps8Manufacture =  strstr(as8Buf, "=");
                        strncpy(as8Manufacture, ps8Manufacture + 1
                                , sizeof(as8Manufacture) - 1);
                        as8Manufacture[strlen(as8Manufacture) - 1] = '\0';
                    }
                    if(NULL != strstr(as8Buf, "Product"))
                    {
                        ps8Product =  strstr(as8Buf, "=");
                        strncpy(as8Product, ps8Product + 1, sizeof(as8Product) - 1);
                        as8Product[strlen(as8Product) - 1] = '\0';
                    }
                }
                else if(NULL != strstr(as8Buf, "C:"))       /* 分析C分支 配置信息 */
                {
                    sscanf(as8Buf, "C:* #Ifs=%d", &u32Subdev);
                }
                else if(NULL != strstr(as8Buf, "I:"))       /* 分析I分支 接口信息 */
                {
                    ps8Driver =  strstr(as8Buf, "Driver");
                    ps8Driver =  strstr(ps8Driver, "=");
                    strncpy(as8Driver, ps8Driver + 1, sizeof(as8Driver) - 1);
                    as8Driver[strlen(as8Driver) - 1] = '\0';
                    pstDesc->usb_dev_info.bus = u32Bus;
                    pstDesc->usb_dev_info.lev = u32Level;
                    pstDesc->usb_dev_info.port = u32Port;
                    pstDesc->usb_dev_info.prnt = u32Parent;
                    pstDesc->vendor = u32Vendor;
                    pstDesc->product_id = u32Prodid;
                    /* pstDesc->subdev_count = subdev;*/
                    strncpy(pstDesc->manufacturer, as8Manufacture
                            , strlen(as8Manufacture));
                    strncpy(pstDesc->product, as8Product, strlen(as8Product));
                    if(0 == MatchDriver(as8Driver, &enUsbDriverType))
                    {
                        if(USB_SERIAL == enUsbDriverType)
                        {
                            s32ReturnValue = GetUsbSerialSubDevs(u32Bus
                                                                 , u32Level, u32Port, u32Parent, u32Count, pstDesc);
                            if(0 == s32ReturnValue)
                            {
                                /* 获取接口编号 */
                                if((s32RetVal = GetInterfaceIndex(u32Bus
                                                                  , u32Level, u32Port, &u32UsbIndex)) < 0)
                                {
                                    LIBHAL_ERROR("get 3g  index failed\n");
                                    goto fail;
                                }
                                pstDesc->inerface_index = u32UsbIndex;
                            }
                            else
                            {
                                LIBHAL_ERROR("get 3g  index failed\n");
                                goto fail;
                            }
                            (*pu32Count) ++;
                            pstDesc ++;
                        }
                        if(USB_WIFI == enUsbDriverType)
                        {
                            pstDesc->subdev_count = 1;
                            pstDesc->subdev[0].driver_type = USB_WIFI;
                            sprintf(pstDesc->subdev[0].name, "%s", "wlan0");
                            s32ReturnValue = GetWifiSubDevs(u32Bus, u32Level
                                                            , u32Port, u32Parent, u32Count, pstDesc);
                            if(0 == s32ReturnValue)
                            {
                                /* 获取接口编号 */
                                if((s32RetVal = GetInterfaceIndex(u32Bus
                                                                  , u32Level, u32Port, &u32UsbIndex)) < 0)
                                {
                                    LIBHAL_ERROR("get wifi index failed\n");
                                    goto fail;
                                }
                                pstDesc->inerface_index = u32UsbIndex;
                            }
                            else
                            {
                                LIBHAL_ERROR("get wifi index failed\n");
                                goto fail;
                            }
                            (*pu32Count) ++;
                            pstDesc ++;
                        }
                        /* 只要匹配返回为0 则结束这一个usb 模块信息的读取 */
                        break;
                    }
                    else
                    {
                        /* 对于未知设备直接填充0设备为未知设备，sub_count为1 */
                        pstDesc->subdev[0].driver_type = USB_UNKNOWN;
                        pstDesc->subdev_count = 1;
                    }
                } /* 判断一个usb模块结束或者文件结束 */
                else if((as8Buf[0] == '\n') || (0 != feof(fp)))
                {
                    /* modify by heliangbin, add 0x6254 */
                    /* 如果prodid 为0x2514 或0x6254, 则保存hub信息 */
                    if((u32Prodid == HUB_PRODID_2514)
                            || (u32Prodid == HUB_PRODID_6254))
                    {
                        gstUsbHubDev.bus = u32Bus;
                        gstUsbHubDev.lev = u32Level;
                        gstUsbHubDev.prnt = u32Parent;
                        gstUsbHubDev.port = u32Port;
                        /* 如果是hub芯片则直接跳出本次循环,不上报给应用层hub
                           2514 设备 */
                        break;
                    }
                    (*pu32Count)++;
                    pstDesc ++;
                    break;
                }
            }
            while(NULL != ps8Str);
        }
    }
    return 0;
fail:
    return -1;
}

/*****************************************************************************
 函 数 名  : GetUsbDevInfo
 功能描述  : 获取USB 设备信息
 输入参数  : USB_DEV_DESC *desc   无线模块数组信息
             unsigned int *count  无线模块的模块个数
 输出参数  : 无
 返 回 值  : =0    成功
             <0    失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月13日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int GetUsbDevInfo(USB_DEV_DESC *pstDesc, unsigned int *pu32Count)
{
    FILE *fp = NULL;
    if(NULL == pstDesc || NULL == pu32Count)
    {
        LIBHAL_ERROR("arguments	error\n");
        return -1;
    }
    memset(pstDesc, 0, sizeof(USB_DEV_DESC) * (*pu32Count));
    if(SafeOpen(USB_DEVICE_FILE, &fp) < 0)
    {
        goto fail;
    }
    if(ProcessUsbInfo(fp, pstDesc, pu32Count) < 0)
    {
        LIBHAL_ERROR("process usb devices file failed!");
        fclose(fp);
        goto fail;
    }
    fclose(fp);
    return 0;
fail:
    return -1;
}



/*****************************************************************************
 函 数 名  : InitUsbInfo
 功能描述  : 初始化usb接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : 0:成功
             -1: 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int InitUsbInfo(void)
{
    S32 s32Ret = 0;
    s32Ret = GetKernelVersion();
    if(s32Ret < 0)
    {
        LIBHAL_ERROR("get kernel version failed\n");
        stVersion.ver_first = 0;
        stVersion.ver_sec = 0;
        stVersion.ver_third = 0;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : ReleaseUsbInfo
 功能描述  : 注销USB设备接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : 0:成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int ReleaseUsbInfo(void)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : power_on_multusb
 功能描述  : 开启USB模块电源 针对多模块
 输入参数  : int index 模块编号
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int PowerOnMultUsbDev(int index)
{
    return 0;
}


/*****************************************************************************
 函 数 名  : PowerOffMultUsbDev
 功能描述  : 关闭USB模块电源 （针对多模块）
 输入参数  : int index 模块编号
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int PowerOffMultUsbDev(int index)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : ResetMultUsbDev
 功能描述  : 重启USB模块无阻塞版本（针对多模块）
 输入参数  : int index 模块编号
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int ResetMultUsbDev(int index)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : ResetUsbHubDev
 功能描述  : 重启外置usb hub
 输入参数  : int index: hub 编号
 输出参数  : 无
 返 回 值  : 0: 成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int ResetUsbHubDev(int index)
{
    return 0;
}


