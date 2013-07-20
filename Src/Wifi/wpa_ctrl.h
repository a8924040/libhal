/*
 * wpa_supplicant/hostapd control interface library
 * Copyright (c) 2004-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef WPA_CTRL_H
#define WPA_CTRL_H

#include <sys/un.h>


#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * struct wpa_ctrl - Internal structure for control interface library
     *
     * This structure is used by the wpa_supplicant/hostapd control interface
     * library to store internal data. Programs using the library should not touch
     * this data directly. They can only use the pointer to the data structure as
     * an identifier for the control interface connection and use this as one of
     * the arguments for most of the control interface library functions.
     */


    struct wpa_ctrl
    {
        int s;
        struct sockaddr_un local;
        struct sockaddr_un dest;
    };

#define WPA_BUF_SIZE    2048
    //wpa_supplicant通信接口控制结构体
    typedef struct
    {
        struct wpa_ctrl ctrl_conn;                     //wpa_supplicant通信接口控制结构体
        char buffer[WPA_BUF_SIZE];                              //存放返回值
    } WPA_CONN_CTRL;



    /* wpa_supplicant/hostapd control interface access */

    /**
     * wpa_ctrl_open - Open a control interface to wpa_supplicant/hostapd
     * @ctrl_path: Path for UNIX domain sockets; ignored if UDP sockets are used.
     * Returns: Pointer to abstract control interface data or %NULL on failure
     *
     * This function is used to open a control interface to wpa_supplicant/hostapd.
     * ctrl_path is usually /var/run/wpa_supplicant or /var/run/hostapd. This path
     * is configured in wpa_supplicant/hostapd and other programs using the control
     * interface need to use matching path configuration.
     */
    //struct wpa_ctrl * wpa_ctrl_open(const char *ctrl_path);
    int wpa_ctrl_open(const char *ctrl_path, struct wpa_ctrl *ctrl);


    /**
     * wpa_ctrl_close - Close a control interface to wpa_supplicant/hostapd
     * @ctrl: Control interface data from wpa_ctrl_open()
     *
     * This function is used to close a control interface.
     */
    void wpa_ctrl_close(struct wpa_ctrl *ctrl);


    /**
     * wpa_ctrl_request - Send a command to wpa_supplicant/hostapd
     * @ctrl: Control interface data from wpa_ctrl_open()
     * @cmd: Command; usually, ASCII text, e.g., "PING"
     * @cmd_len: Length of the cmd in bytes
     * @reply: Buffer for the response
     * @reply_len: Reply buffer length
     * @msg_cb: Callback function for unsolicited messages or %NULL if not used
     * Returns: 0 on success, -1 on error (send or receive failed), -2 on timeout
     *
     * This function is used to send commands to wpa_supplicant/hostapd. Received
     * response will be written to reply and reply_len is set to the actual length
     * of the reply. This function will block for up to two seconds while waiting
     * for the reply. If unsolicited messages are received, the blocking time may
     * be longer.
     *
     * msg_cb can be used to register a callback function that will be called for
     * unsolicited messages received while waiting for the command response. These
     * messages may be received if wpa_ctrl_request() is called at the same time as
     * wpa_supplicant/hostapd is sending such a message. This can happen only if
     * the program has used wpa_ctrl_attach() to register itself as a monitor for
     * event messages. Alternatively to msg_cb, programs can register two control
     * interface connections and use one of them for commands and the other one for
     * receiving event messages, in other words, call wpa_ctrl_attach() only for
     * the control interface connection that will be used for event messages.
     */
    int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len,
                         char *reply, size_t *reply_len,
                         void (*msg_cb)(char *msg, size_t len));





#ifdef  __cplusplus
}
#endif

#endif /* WPA_CTRL_H */
