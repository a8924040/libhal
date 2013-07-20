#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "v4l2.h"
#include "yuv422_rgb.h"
#include <linux/fb.h>

//#define DEBUG_PTIME
#define DEBUG_GTIME

#define IMAGE_WIDTH 240
#define IMAGE_HEIGHT  320
#define WINDOW_W    240
#define WINDOW_H    320
#define WINDOW_X    0
#define WINDOW_Y    0

static void *fbbuf;
struct fb_var_screeninfo fbvar;
struct fb_fix_screeninfo finfo;



int fb_init(char *devname)
{
    unsigned int bpp;
    unsigned int screen_size;
    int fd;
    fd = open(devname, O_RDWR);
    if(ioctl(fd, FBIOGET_VSCREENINFO, &fbvar) < 0)
    { 
        return -1; 
    }
    bpp = fbvar.bits_per_pixel;
    screen_size = fbvar.xres * fbvar.yres * bpp / 8;
    fbbuf = mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return fd;
}

static inline int  make_pixel(unsigned int a, unsigned int r, unsigned int g, unsigned int b)
{
    return (unsigned int)(((r >> 3) << 11) | ((g >> 2) << 5 | (b >> 3)));
}

static void draw_image(unsigned short *pbuf, unsigned char *buf)
{
    unsigned int x, y;
    unsigned int pixel_num;
    if(WINDOW_W <= 240)
    {
        for(y = WINDOW_Y; y < WINDOW_Y + WINDOW_H; y++)
        {
            for(x = WINDOW_X; x < WINDOW_X + WINDOW_W; x++)
            {
                pixel_num = ((y - WINDOW_Y) * IMAGE_WIDTH + x - WINDOW_X) ;
                pbuf[y*240 + x] = make_pixel(0, (unsigned int)buf[pixel_num]
                                               ,(unsigned int)buf[pixel_num + 1]
                                              , (unsigned int)buf[pixel_num + 2]);
            }
        }
    }
    else     /* reverse */
    {
        for(x = 0; x < WINDOW_W; x++)
        {
            for(y = 0; y < WINDOW_H; y++)
            {
                pixel_num = (y * IMAGE_WIDTH + x) * 3;
                pbuf[x * 240 + y] = make_pixel(0, (unsigned int)buf[pixel_num],
                                               (unsigned int)buf[pixel_num + 1], (unsigned int)buf[pixel_num + 2]);
            }
        }
    }
}


void process_image(unsigned char *p, struct camera *cam)
{
    struct timeval tpstart, tpend;
    float timeuse;
#ifdef DEBUG_PTIME
    gettimeofday(&tpstart, NULL);
#endif
    yuv422_rgb24(p, cam->rgbbuf, cam->width, cam->height);
#ifdef DEBUG_PTIME
    gettimeofday(&tpend, NULL);
    timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);
    timeuse /= 1000000;
    printf("Used Time:%10f s\n", timeuse);
#endif
}
int main(int argc, const char *argv[])
{
    int fp = 0;
    unsigned int i;
    /*
    * init struct camera
    */
    struct camera *cam;
    struct timeval tpstart, tpend;
    float timeuse;
    unsigned short *pbuf;
    cam = malloc(sizeof(struct camera));
    if(!cam)
    {
        printf("malloc camera failure!\n");
        exit(1);
    }
    cam->device_name = "/dev/video0";
    cam->buffers = NULL;
    cam->width = IMAGE_WIDTH;
    cam->height = IMAGE_HEIGHT;
    cam->display_depth = 3;  /* RGB24 */
    cam->rgbbuf = malloc(cam->width * cam->height * cam->display_depth);
    if(!cam->rgbbuf)
    {
        printf("malloc rgbbuf failure!\n");
        exit(1);
    }
    open_camera(cam);
    get_cam_cap(cam);
    get_cam_pic(cam);
    get_cam_win(cam);
    cam->video_win.width = cam->width;
    cam->video_win.height = cam->height;
    set_cam_win(cam);
    close_camera(cam);
    open_camera(cam);
    get_cam_win(cam);
    init_camera(cam);
    start_capturing(cam);
    fp = fb_init("/dev/fb0");
    if(fp < 0)
    {
        printf("Error : Can not open framebuffer device\n");
        exit(1);
    }
    pbuf = (unsigned short *)fbbuf;
    for(i = 0; i < 240 * 320; i++)
    {
        pbuf[i] = make_pixel(0, 0x00, 0x88, 0xff);
    }
#ifdef DEBUG_GTIME
    gettimeofday(&tpstart, NULL);
#endif
    for(;;)
    {
        if(read_frame(cam))
        {
            draw_image(pbuf, cam->rgbbuf);
#ifdef DEBUG_GTIME
            gettimeofday(&tpend, NULL);
            timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);
            timeuse /= 1000000;
            printf("Used Time:%10f s\n", timeuse);
            gettimeofday(&tpstart, NULL);
#endif
        }
    }
    return 0;
}


