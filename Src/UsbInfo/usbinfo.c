/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  �� �� ��   : Usbinfo.c
  �� �� ��   : ����
  ��    ��   : tango_zhu
  ��������   : usb��Ϣ��ȡ
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
#include "usbinfo.h"

#define LINUX_KERNEL_VERSION    "/proc/version"
#define KERNEL_VERSION          "Linux version"
#define USB_DEVICE_FILE     "/proc/bus/usb/devices"
#define USB_SERIAL_DIR      "/sys/bus/usb-serial/devices"
#define USB_WIFI_DIR      "/sys/bus/usb/devices/"
#define FOPEN_RETRY_TIMES   5
#define USB_SUBDEV_COUNT    8               /*һ��USB�ӿڶ�Ӧ���豸����  */
#define USB_DEV_MAX         16
#define HUB_PRODID_2514     0x2514
/* add by heliangbin  */
#define HUB_PRODID_6254 0x6254


static KERNERL_VERSION_STRU stVersion;
static USB_DRIVER_INFO_STRU gastUsbDriverInfoTable[] = { {USB_SERIAL, {"usbserial_generic"}}, {USB_WIFI, {"rt2870"}},};


/* ����3g wifi ��Ӧ��usb �˿ںͶ�Ӧ�ĸ�λ���� */
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



/* ���豸����2514 hubоƬʱ����Ҫ��2514����Ϣ�洢����  */
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
 �� �� ��  : safe_fopen
 ��������  : ��ȫ�� ��ʧ�ܺ��ظ����� FOPEN_RETRY_TIMES
 �������  : const char *path
             FILE **fp
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��13��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : MatchDriver
 ��������  : ���ݵõ�����������ƥ����������
 �������  : char *driver
             USB_DRIVER_TYPE *pusb_driver_type
 �������  : ��
 �� �� ֵ  : -1:��������
             0: �ҵ�ƥ���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��14��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : GetInterfaceIndex
 ��������  : ����busb,level,port,parent ��ȡ�ӿں�
 �������  : int bus
             int level
             int port
             int parent
             int count
             USB_DEV_DESC *pstDesc
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��16��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : GetWifiSubDevs
 ��������  : ��ȡwifi�豸�����豸����ȡ������
 �������  : int bus
             int level
             int port
             int parent
             int count
             USB_DEV_DESC *pstDesc
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��16��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /* usb ������Ŀ¼����bus��count����ɵ�netĿ¼�� */
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
                /* �����豸���� */
                strncpy(pstDesc->subdev[u32WlanIndex].name, ps8Str, strlen("wlan0"));
                /* �����豸���� */
                pstDesc->subdev[u32WlanIndex].driver_type = USB_WIFI;
                u32WlanIndex ++;
                /* ���wlan ���豸�ĸ��� */
                pstDesc->subdev_count =  u32WlanIndex;
            }
        }
        else
        {
            /* �����豸���� */
            strncpy(pstDesc->subdev[u32WlanIndex].name, pstDirent->d_name
                    , strlen(pstDirent->d_name));
            /* �����豸���� */
            pstDesc->subdev[u32WlanIndex].driver_type = USB_WIFI;
            u32WlanIndex ++;
            /* ���wlan ���豸�ĸ��� */
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
 �� �� ��  : GetUsbSerialSubDevs
 ��������  : ����usb devices �ļ��е�level bus,parent port ��ȡ���豸��Ϣ
 �������  : int bus
             int level
             int port
             int parent
             USB_DEV_DESC pstDesc
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��14��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
    /* �ں�hubоƬ��bus ��ͬʱʹ��%d-%d.%d Ŀ¼ */
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
        /* ���ttyUSB* ��·�� */
        sprintf(s8aTtyusbDir, "%s/%s", USB_SERIAL_DIR, pstDirent->d_name);
        if(lstat(s8aTtyusbDir, &stFileState) < 0)
        {
            LIBHAL_ERROR("lstat %s failed", s8aTtyusbDir);
            goto fail;
        }
        /* ttyUSB* �������ļ�����ȡ�����ļ���ָ����ļ� */
        if(S_ISLNK(stFileState.st_mode))
        {
            readlink(s8aTtyusbDir, s8aTtyusbLink, 128);
            s8pTtyusb = strrchr(s8aTtyusbLink, '/');
            *s8pTtyusb = '\0';
            ps8BusName  = strrchr(s8aTtyusbLink, '/');
            /* ���߱�� �Ͳ�������� �������صĲ��� */
            if(!strncmp(ps8BusName + 1, s8aBusName, strlen(s8aBusName)))
            {
                /* �����豸���� */
                strncpy(pstDesc->subdev[u8TtyIndex].name, s8pTtyusb + 1
                        , strlen(s8pTtyusb + 1));
                /* �����豸���� */
                pstDesc->subdev[u8TtyIndex].driver_type = USB_SERIAL;
                u8TtyIndex ++;
                /* ���ttyusb ���豸�ĸ��� */
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
 �� �� ��  : process_usb_info
 ��������  : ����/proc/bus/usb/devices �ļ���ȡ��Ӧ�Ĳ���
 �������  : FILE *fp
             USB_DEV_DESC *pstDesc
             unsigned int *pu32Count
 �������  : ��
 �� �� ֵ  : static
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��18��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
        if(NULL != strstr(as8Buf, "T:"))    /* �ҵ����˷�֧��ʼ���� */
        {
            memset(as8Manufacture, 0, sizeof(as8Manufacture));
            memset(as8Product, 0, sizeof(as8Product));
            memset(as8Driver, 0, sizeof(as8Driver));
            sscanf(as8Buf, "T:	Bus=%d Lev=%d Prnt=%d Port=%d Cnt=%d", &u32Bus
                   , &u32Level, &u32Parent, &u32Port, &u32Count);
            do
            {
                ps8Str = fgets(as8Buf, sizeof(as8Buf), fp);
                if(NULL != strstr(as8Buf, "P:"))    /* ����P��֧ PRODUCT ID��Ϣ */
                {
                    sscanf(as8Buf, "P:  Vendor=%x ProdID=%x", &u32Vendor
                           , &u32Prodid);
                    /* ���vendorΪ0x1d6b���򲻴��� */
                    if(u32Vendor == 0x1d6b || (u32Vendor == 0 && u32Prodid == 0))
                    {
                        break;
                    }
                }
                else if(NULL != strstr(as8Buf, "S:"))       /* ����S��֧ ������Ϣ */
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
                else if(NULL != strstr(as8Buf, "C:"))       /* ����C��֧ ������Ϣ */
                {
                    sscanf(as8Buf, "C:* #Ifs=%d", &u32Subdev);
                }
                else if(NULL != strstr(as8Buf, "I:"))       /* ����I��֧ �ӿ���Ϣ */
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
                                /* ��ȡ�ӿڱ�� */
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
                                /* ��ȡ�ӿڱ�� */
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
                        /* ֻҪƥ�䷵��Ϊ0 �������һ��usb ģ����Ϣ�Ķ�ȡ */
                        break;
                    }
                    else
                    {
                        /* ����δ֪�豸ֱ�����0�豸Ϊδ֪�豸��sub_countΪ1 */
                        pstDesc->subdev[0].driver_type = USB_UNKNOWN;
                        pstDesc->subdev_count = 1;
                    }
                } /* �ж�һ��usbģ����������ļ����� */
                else if((as8Buf[0] == '\n') || (0 != feof(fp)))
                {
                    /* modify by heliangbin, add 0x6254 */
                    /* ���prodid Ϊ0x2514 ��0x6254, �򱣴�hub��Ϣ */
                    if((u32Prodid == HUB_PRODID_2514)
                            || (u32Prodid == HUB_PRODID_6254))
                    {
                        gstUsbHubDev.bus = u32Bus;
                        gstUsbHubDev.lev = u32Level;
                        gstUsbHubDev.prnt = u32Parent;
                        gstUsbHubDev.port = u32Port;
                        /* �����hubоƬ��ֱ����������ѭ��,���ϱ���Ӧ�ò�hub
                           2514 �豸 */
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
 �� �� ��  : GetUsbDevInfo
 ��������  : ��ȡUSB �豸��Ϣ
 �������  : USB_DEV_DESC *desc   ����ģ��������Ϣ
             unsigned int *count  ����ģ���ģ�����
 �������  : ��
 �� �� ֵ  : =0    �ɹ�
             <0    ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��13��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : InitUsbInfo
 ��������  : ��ʼ��usb�ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : 0:�ɹ�
             -1: ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��23��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

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
 �� �� ��  : ReleaseUsbInfo
 ��������  : ע��USB�豸�ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : 0:�ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��23��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int ReleaseUsbInfo(void)
{
    return 0;
}

/*****************************************************************************
 �� �� ��  : power_on_multusb
 ��������  : ����USBģ���Դ ��Զ�ģ��
 �������  : int index ģ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��23��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int PowerOnMultUsbDev(int index)
{
    return 0;
}


/*****************************************************************************
 �� �� ��  : PowerOffMultUsbDev
 ��������  : �ر�USBģ���Դ ����Զ�ģ�飩
 �������  : int index ģ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��23��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int PowerOffMultUsbDev(int index)
{
    return 0;
}

/*****************************************************************************
 �� �� ��  : ResetMultUsbDev
 ��������  : ����USBģ���������汾����Զ�ģ�飩
 �������  : int index ģ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��23��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int ResetMultUsbDev(int index)
{
    return 0;
}

/*****************************************************************************
 �� �� ��  : ResetUsbHubDev
 ��������  : ��������usb hub
 �������  : int index: hub ���
 �������  : ��
 �� �� ֵ  : 0: �ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��7��23��
    ��    ��   : tango_zhu
    �޸�����   : �����ɺ���

*****************************************************************************/
int ResetUsbHubDev(int index)
{
    return 0;
}


