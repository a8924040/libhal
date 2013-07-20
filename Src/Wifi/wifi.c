/******************************************************************************

(c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.

                             All Rights Reserved

 ******************************************************************************
  文 件 名   : wifi.c
  版 本 号   : 初稿
  作    者   : tango_zhu
  功能描述   : wlan相关函数
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
#include "Wifi.h"
#include "wpa_ctrl.h"

#define WLAN_DEV_PATH                     "/sys/class/net/"
#define WLAN_DEV_NAME                     "wlan0"
#define WPA_SUPPLICANT_PATH               "/var/wpa_supplicant"
#define WPA_SUPPLICANT_SOCK               "/var/run/wpa_supplicant/wlan0"
#define CHECK_SOCK_MAX                    20

WPA_CONN_CTRL gstWpaCtrl;
int  GNetworkID = 0;

char *paEncryptType[4] =
{
    "key_mgmt NONE",
    "key_mgmt NONE",
    "psk",
    "psk",
};


int wpa_ctrl_open(const char *ctrl_path, struct wpa_ctrl *ctrl)
{
    static int counter = 0;
    memset(ctrl, 0, sizeof(*ctrl));
    ctrl->s = socket(PF_UNIX, SOCK_DGRAM, 0);
    if(ctrl->s < 0)
    {
        return -1;
    }
    ctrl->local.sun_family = AF_UNIX;
    sprintf(ctrl->local.sun_path, "/var/wpa_ctrl_%d-%d", getpid(), counter++);
    if(bind(ctrl->s, (struct sockaddr *) &ctrl->local,
            sizeof(ctrl->local)) < 0)
    {
        close(ctrl->s);
        return -1;
    }
    ctrl->dest.sun_family = AF_UNIX;
    sprintf(ctrl->dest.sun_path, "%s", ctrl_path);
    if(connect(ctrl->s, (struct sockaddr *) &ctrl->dest,
               sizeof(ctrl->dest)) < 0)
    {
        close(ctrl->s);
        unlink(ctrl->local.sun_path);
        return -1;
    }
    return 0;
}


void wpa_ctrl_close(struct wpa_ctrl *ctrl)
{
    unlink(ctrl->local.sun_path);
    close(ctrl->s);
}

int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len,
                     char *reply, size_t *reply_len,
                     void (*msg_cb)(char *msg, size_t len))
{
    struct timeval tv;
    int res;
    fd_set rfds;
    const char *_cmd;
    size_t _cmd_len;
    _cmd = cmd;
    _cmd_len = cmd_len;
    if(send(ctrl->s, _cmd, _cmd_len, 0) < 0)
    {
        return -1;
    }
    for(;;)
    {
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(ctrl->s, &rfds);
        res = select(ctrl->s + 1, &rfds, NULL, NULL, &tv);
        if(FD_ISSET(ctrl->s, &rfds))
        {
            res = recv(ctrl->s, reply, *reply_len, 0);
            if(res < 0)
            { return res; }
            if(res > 0 && reply[0] == '<')
            {
                /* This is an unsolicited message from
                 * wpa_supplicant, not the reply to the
                 * request. Use msg_cb to report this to the
                 * caller. */
                if(msg_cb)
                {
                    /* Make sure the message is nul
                     * terminated. */
                    if((size_t) res == *reply_len)
                    { res = (*reply_len) - 1; }
                    reply[res] = '\0';
                    msg_cb(reply, res);
                }
                continue;
            }
            *reply_len = res;
            break;
        }
        else
        {
            return -2;
        }
    }
    return 0;
}

int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply, size_t *reply_len)
{
    int res;
    res = recv(ctrl->s, reply, *reply_len, 0);
    if(res < 0)
    { return res; }
    *reply_len = res;
    return 0;
}


int wpa_ctrl_pending(struct wpa_ctrl *ctrl)
{
    struct timeval tv;
    fd_set rfds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(ctrl->s, &rfds);
    select(ctrl->s + 1, &rfds, NULL, NULL, &tv);
    return FD_ISSET(ctrl->s, &rfds);
}


int wpa_ctrl_get_fd(struct wpa_ctrl *ctrl)
{
    return ctrl->s;
}

/*****************************************************************************
 函 数 名  : CheckWlanDev
 功能描述  : 检查wlan 设备是否存在
 输入参数  : void
 输出参数  : 无
 返 回 值  : 1:存在 0:不存在
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月17日
    作    者   : 检查网线连接状态
    修改内容   : 新生成函数

*****************************************************************************/
static int CheckWlanDev(void)
{
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(WLAN_DEV_PATH);
    while((ptr = readdir(dir)) != NULL)
    {
        if((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)
                || (strcmp(ptr->d_name, "lo") == 0))       //排除lo网卡
        {
            continue;
        }
        if(strcmp(ptr->d_name, WLAN_DEV_NAME) == 0)
        {
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);
    return 0;
}




static void wpa_cli_msg_cb(char *msg, size_t len)
{
    LIBHAL_PRINTF("%s\n", msg);
}


static int _wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, int print)
{
    WPA_CONN_CTRL *wpa_ctrl;
    char *buf;
    size_t len;
    int ret;
    wpa_ctrl = container_of(ctrl, WPA_CONN_CTRL, ctrl_conn);
    buf = wpa_ctrl->buffer;
    len = WPA_BUF_SIZE - 1 ;
    ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len,
                           wpa_cli_msg_cb);
    if(ret == -2)
    {
        LIBHAL_WARN("'%s' command timed out.\n", cmd);
        return -2;
    }
    else if(ret < 0)
    {
        LIBHAL_WARN("'%s' command failed.\n", cmd);
        return -1;
    }
    if(print)
    {
        buf[len] = '\0';
    }
    //LIBHAL_INFO("Command %s, response:%s", cmd, wpa_ctrl->buffer);
    return 0;
}


int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd)
{
    return _wpa_ctrl_command(ctrl, cmd, 1);
}

/*****************************************************************************
 函 数 名  : WlanCreat
 功能描述  : 创建waln 接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : -3 timeout;-2 wlan 设备不存在
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月17日
    作    者   : 检查网线连接状态
    修改内容   : 新生成函数

*****************************************************************************/
int WlanCreat(void)
{
    int i;
    char *pArgv[30] =
    {
        "wpa_supplicant",
        "-B",
        "-iwlan0",
        "-c/etc/wpa_supplicant.conf",
        "-Dwext",
        NULL,
    };
    if(0 == CheckWlanDev())
    {
        LIBHAL_ERROR("There is no wlan dev!");
        return -2;
    }
    if(CallForkProcess(WPA_SUPPLICANT_PATH, pArgv) < 0)
    {
        LIBHAL_ERROR("CallForkProcess failed!");
        return -1;
    }
    for(i = 0; i < CHECK_SOCK_MAX; i++)
    {
        if(access(WPA_SUPPLICANT_SOCK, F_OK) != 0)
        {
            uint_sleep(1);
            continue;
        }
        break;
    }
    if(i == CHECK_SOCK_MAX)
    {
        LIBHAL_ERROR("Check wpa_supplicant socket timeout!");
        return -3;
    }
    if(-1 == wpa_ctrl_open(WPA_SUPPLICANT_SOCK, &gstWpaCtrl.ctrl_conn))
    {
        return -1;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : WlanScanAP
 功能描述  : 扫描wlan 热点
 输入参数  : WLAN_AP_STRU *pWlanAP
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/

int WlanScanAP(WLAN_AP_STRU *pWlanAP)
{
    char *ptok = NULL;
    char *pstr = NULL;
    char buf_ssid[128] = {0};
    char buf_mac[32] = {0};
    char buf_frq[8] = {0};
    char buf_signal[8] = {0};
    char buf_encrypt[32] = {0};
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, "SCAN_RESULTS") < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    //LIBHAL_INFO("The scan result is %s", gstWpaCtrl.buffer);
    strtok(gstWpaCtrl.buffer, "\n");
    while((ptok = strtok(NULL, "\n")) != NULL)
    {
        sscanf(ptok, "%s %s %s %s %s", buf_mac, buf_frq
               , buf_signal, buf_encrypt, buf_ssid);
        pstr = strstr(ptok, "\t");
        sscanf(pstr + 1, "%5s", buf_frq);
        pstr = strstr(pstr + 1, "\t");
        sscanf(pstr + 1, "%4s", buf_signal);
        sprintf(pWlanAP->astAPBox[pWlanAP->APSum].APMAC, "%s", buf_mac);
        sscanf(buf_signal, "%d", &pWlanAP->astAPBox[pWlanAP->APSum].APSignal);
        sscanf(buf_frq, "%d", &pWlanAP->astAPBox[pWlanAP->APSum].APFreq);
        if(pWlanAP->astAPBox[pWlanAP->APSum].APSignal > 200)
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APSignal = 100;
        }
        else
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APSignal = pWlanAP->astAPBox[pWlanAP->APSum].APSignal - 100;
        }
        if(NULL != strstr(buf_encrypt, "WPA2"))
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APEncrypt =  AP_ENCRYPT_WPA2;
        }
        else if(NULL != strstr(buf_encrypt, "WPA"))
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APEncrypt =  AP_ENCRYPT_WPA;
        }
        else if(NULL != strstr(buf_encrypt, "WEP"))
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APEncrypt =  AP_ENCRYPT_WEP;
        }
        else
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APEncrypt =  AP_ENCRYPT_OPEN;
        }
        if(NULL != strstr(buf_encrypt, "WPS"))
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APWPS =  WPS_VALID;
        }
        else
        {
            pWlanAP->astAPBox[pWlanAP->APSum].APWPS =  WPS_INVALID;
        }
        sprintf(pWlanAP->astAPBox[pWlanAP->APSum].APSSID, "%s", buf_ssid);
        pWlanAP->APSum++;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : WlanGetLinkState
 功能描述  : 获取wlan 连接状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : COMPLETED:已经连接 ，DISCONNECTED:未连接，INACTIVE:未活动
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月19日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int WlanGetLinkState(void)
{
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, "STATUS") < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The scan result is %s", gstWpaCtrl.buffer);
    if(NULL != strstr(gstWpaCtrl.buffer, "COMPLETED"))
    {
        return COMPLETED;
    }
    else if(NULL != strstr(gstWpaCtrl.buffer, "DISCONNECTED"))
    {
        return DISCONNECTED;
    }
    else if(NULL != strstr(gstWpaCtrl.buffer, "INACTIVE"))
    {
        return INACTIVE;
    }
    return INACTIVE;
}

/*****************************************************************************
 函 数 名  : WlanConnectAPState
 功能描述  : wlan 连接ap
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月19日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int WlanConnectAP(WLAN_CONN_INFO_STRU WlanConnInfo)
{
    char command[256] = {0};
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    /* before enable network first  remove network */
    sprintf(command, "REMOVE_NETWORK  %x", GNetworkID);
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The REMOVE_NETWORK result is %s", gstWpaCtrl.buffer);
    /* add network */
    sprintf(command, "ADD_NETWORK");
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The ADD_NETWORK result is %s", gstWpaCtrl.buffer);
    if(NULL != strstr(gstWpaCtrl.buffer, "FAIL"))
    {
        LIBHAL_ERROR("wpa_ctrl_command  ADD_NETWORK failed!");
        return -1;
    }
    sscanf(gstWpaCtrl.buffer, "%x", &GNetworkID);
    /* Set Network ssid */
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    sprintf(command, "SET_NETWORK %x ssid \"%s\"", GNetworkID, WlanConnInfo.APSSID);
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The SET_NETWORK result is %s", gstWpaCtrl.buffer);
    if(NULL == strstr(gstWpaCtrl.buffer, "OK"))
    {
        LIBHAL_ERROR("wpa_ctrl_command  SET_NETWORK SSID failed!");
    }
    /* Set Network password */
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    sprintf(command, "SET_NETWORK %x %s \"%s\"", GNetworkID
            , paEncryptType[WlanConnInfo.APEncrypt], WlanConnInfo.APPassWord);
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The SET_NETWORK password is %s", gstWpaCtrl.buffer);
    if(NULL == strstr(gstWpaCtrl.buffer, "OK"))
    {
        LIBHAL_ERROR("wpa_ctrl_command  SET_NETWORK password failed!");
        return -1;
    }
    /* Enable NetWork */
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    sprintf(command, "ENABLE_NETWORK %x", GNetworkID);
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The ENABLE_NETWORK  is %s", gstWpaCtrl.buffer);
    if(NULL == strstr(gstWpaCtrl.buffer, "OK"))
    {
        LIBHAL_ERROR("wpa_ctrl_command  ENABLE_NETWORK  failed!");
        return -1;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : WlanReleaseAP
 功能描述  : 断开wlan 热点
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int WlanReleaseAP(void)
{
    char command[256] = {0};
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    /* before enable network first  remove network */
    sprintf(command, "DISABLE_NETWORK  %x", GNetworkID);
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The DISABLE_NETWORK result is %s", gstWpaCtrl.buffer);
    return 0;
}

/*****************************************************************************
 函 数 名  : WlanDestory
 功能描述  : 销毁wlan wpa_supplicant 退出
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : tango_zhu
    修改内容   : 新生成函数

*****************************************************************************/
int WlanDestory(void)
{
    char command[256] = {0};
    memset(gstWpaCtrl.buffer, 0, sizeof(gstWpaCtrl.ctrl_conn));
    /* before enable network first  remove network */
    sprintf(command, "TERMINATE");
    LIBHAL_INFO("the command is:%s", command);
    if(wpa_ctrl_command(&gstWpaCtrl.ctrl_conn, command) < 0)
    {
        LIBHAL_ERROR("wpa_ctrl_command  failed!");
        return -1;
    }
    LIBHAL_INFO("The TERMINATE result is %s", gstWpaCtrl.buffer);
    return 0;
}



