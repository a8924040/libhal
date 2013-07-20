#include <System.h>
#include <CommonDef.h>
#include <CommonInclude.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>


#include "jpeglib.h"

#include "jerror.h"

#define FB_DEV  "/dev/fb0"

/***************** function declaration ******************/
void            usage(char *msg);
unsigned short  RGB888toRGB565(unsigned char red, unsigned char green, unsigned char blue);
int             fb_open(char *fb_device);
int             fb_close(int fd);
int             fb_stat(int fd, unsigned int *width, unsigned int *height, unsigned int *depth);
void           *fb_mmap(int fd, unsigned int screensize);
int             fb_munmap(void *start, size_t length);
int             fb_pixel(void *fbmem, int width, int height, int x, int y, unsigned short color);

void usage(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    printf("Usage: fv some-jpeg-file.jpg\n");
}

/*
 * convert 24bit RGB888 to 16bit RGB565 color format
 */
unsigned short RGB888toRGB565(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned short  R = (red >> 3) << 11;
    unsigned short  G = (green  >> 2) << 5;
    unsigned short  B = (blue >> 3);
    return (unsigned short)(R | G | B);
}

/*
 * open framebuffer device.
 * return positive file descriptor if success,
 * else return -1.
 */
int fb_open(char *fb_device)
{
    int             fd;
    if((fd = open(fb_device, O_RDWR)) < 0)
    {
        perror(__func__);
        return (-1);
    }
    return (fd);
}

/*
 * get framebuffer's width,height,and depth.
 * return 0 if success, else return -1.
 */
int fb_stat(int fd, unsigned int *width, unsigned int *height, unsigned int *depth)
{
    struct fb_fix_screeninfo fb_finfo;
    struct fb_var_screeninfo fb_vinfo;
    if(ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo))
    {
        perror(__func__);
        return (-1);
    }
    if(ioctl(fd, FBIOGET_VSCREENINFO, &fb_vinfo))
    {
        perror(__func__);
        return (-1);
    }
    *width = fb_vinfo.xres;
    *height = fb_vinfo.yres;
    *depth = fb_vinfo.bits_per_pixel;
    return (0);
}

/*
 * map shared memory to framebuffer device.
 * return maped memory if success,
 * else return -1, as mmap dose.
 */
void *fb_mmap(int fd, unsigned int screensize)
{
    caddr_t         fbmem;
    if((fbmem = mmap(0, screensize, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        perror(__func__);
        return (void *)(-1);
    }
    return (fbmem);
}

/*
 * unmap map memory for framebuffer device.
 */
int fb_munmap(void *start, size_t length)
{
    return (munmap(start, length));
}

/*
 * close framebuffer device
 */
int fb_close(int fd)
{
    return (close(fd));
}

/*
 * display a pixel on the framebuffer device.
 * fbmem is the starting memory of framebuffer,
 * width and height are dimension of framebuffer,
 * x and y are the coordinates to display,
 * color is the pixel's color value.
 * return 0 if success, otherwise return -1.
 */
int fb_pixel(void *fbmem, int width, int height,
             int x, int y, unsigned short color)
{
    if((x > width) || (y > height))
    { return (-1); }
    unsigned short *dst = ((unsigned short *) fbmem + y * width + x);
    *dst = color;
    return (0);
}


#include <sys/mman.h>
#include <sys/ioctl.h>
#include <error.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/videodev.h>
#include <linux/videodev.h>
#include <sys/types.h>
#include <string.h>


#define norm VIDEO_MODE_NTSC
#define DEFAULT_FILE_NAME "picture.jpg"
#define DEFAULT_DEVICE "/dev/video0"

//PAL CIF NTSC tuner mode

#define PAL_WIDTH 768
#define PAL_HEIGHT 576
#define CIF_WIDTH 352
#define CIF_HEIGHT 288
#define NTSC_WIDTH 320   // 
#define NTSC_HEIGHT 240
#define DEFAULT_PALETTE VIDEO_PALETTE_RGB565

struct _v4l_device

{

    int fd;

    struct video_capability capability;

    struct video_picture picture;

    struct video_window window;

    struct video_channel channel[8];

    struct video_mbuf mbuf;

    struct video_capture capture;

    struct video_buffer vbuffer;

    struct video_mmap mmap;

    unsigned char *map;

    int frame;

    int framestat[2];

};

typedef struct _v4l_device v4ldevice;



int v4l_open(char *dev, v4ldevice *vd)

{
    if(!dev) {dev = DEFAULT_DEVICE;} ;
    if((vd->fd = open(dev, O_RDWR, 10705)) < 0)
    {
        return -1;
    };
    if(v4l_get_capability(vd) < 0)
    { return -1; }
    if(v4l_get_picture(vd) < 0)
    { return -1; }
    return 0;
}


int v4l_get_capability(v4ldevice *vd)

{
    if(ioctl(vd->fd, VIDIOCGCAP, &(vd->capability)) < 0)
    {
        perror("v4l_get_capability:");
        return -1;
    };
    return 0;
}


int v4l_get_picture(v4ldevice *vd)

{
    if(ioctl(vd->fd, VIDIOCGPICT, &(vd->picture)) < 0)
    {
        perror("v4l_get_picture");
        return -1;
    };
    return 0;
}




int v4l_set_norm(v4ldevice *vd, int nrm)


{
    int i;
    for(i = 0; i < vd->capability.channels ; i++)
    {
// vd->channel[i]=nrm ;
    };
    return 0;
}






int v4l_grab_init(v4ldevice *vd, int width, int height)

{
    vd->mmap.width = width;
    vd->mmap.height = height;
    vd->mmap.format = vd->picture.palette;
    vd->frame = 0;
    vd->framestat[0] = 0;
    vd->framestat[1] = 0;
    return 0;
}


int v4l_mmap_init(v4ldevice *vd)

{
    if(v4l_get_mbuf(vd) < 0)
    { return -1; }
    if((vd->map = mmap(0, vd->mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, vd->fd, 0)) < 0)
    {
        return -1;
    }
    return 0;
}


int v4l_get_mbuf(v4ldevice *vd)

{
    if(ioctl(vd->fd, VIDIOCGMBUF, &(vd->mbuf)) < 0)
    {
        perror("v4l_get_mbuf:");
        return -1;
    }
    printf("size=%d\n", vd->mbuf.size);
    return 0;
}


int v4l_grab_start(v4ldevice *vd, int frame)

{
    vd->mmap.frame = frame;
    if(ioctl(vd->fd, VIDIOCMCAPTURE, &(vd->mmap)) < 0)
    {
        exit(-1);
        return -1;
    }
    vd->framestat[frame] = 1;
    return 0;
}

int v4l_grab_sync(v4ldevice *vd, int frame)

{
    if(ioctl(vd->fd, VIDIOCSYNC, &frame) < 0)
    {
        return -1;
    }
    vd->framestat[frame] = 0;
    return 0;
}


unsigned char *v4l_get_address(v4ldevice *vd)

{
    return (vd->map + vd->mbuf.offsets[vd->frame]);
//   return (vd->map+vd->mbuf.offsets[0]);
}



int v4l_close(v4ldevice *vd)

{
    close(vd->fd);
    return 0;
}


/*************************************************************/


/******************main.c************************/


/*name :main.c


  date:2009-5-20


  author:kevin


  copyright:all is reserved


************************************/


char *buffer = NULL;


int uvc_main(void)
{
    v4ldevice VD;
    v4ldevice *vd = &VD;
    int frame = 0;
    int f_d;
    int NUM, write_number = 0;
    printf("f_d = %d \n", f_d);
    if(0 == v4l_open("/dev/video0", vd))
    { printf("open success!\n"); }
    else
    { printf("open failure\n"); }
    if(0 == v4l_set_norm(vd, norm))
    { printf("set_norm success\n"); }
    else
    { printf("set_norm failure\n"); }
    if(0 == v4l_grab_init(vd, NTSC_WIDTH, NTSC_HEIGHT))
    { printf("init success!\n"); }
    else
    { printf("init failure\n"); }
    if(0 == v4l_mmap_init(vd))
    { printf("memory map success!\n"); }
    else
    { printf("memory map failure\n"); }
    while(1)
    {
        if(0 == v4l_grab_start(vd, frame))
        { 
            printf("get picture success!\n"); 
        }
        else
        { 
            printf("get picture failure\n"); 
        }
        v4l_grab_sync(vd, frame);
        buffer = (char *)v4l_get_address(vd);
        printf("img address %p\n", buffer);
        sleep(5);
    }
    v4l_close(vd);
    return 0;
}


/************ function implementation ********************/
int main(int argc, char *argv[])
{
    /*
     * declaration for jpeg decompression
     */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE           *infile;
    unsigned char  *buffer;
    /*
     * declaration for framebuffer device
     */
    int             fbdev;
    char           *fb_device;
    unsigned char  *fbmem;
    unsigned int    screensize;
    unsigned int    fb_width;
    unsigned int    fb_height;
    unsigned int    fb_depth;
    unsigned int    x;
    unsigned int    y;
    /*
     * check auguments
     */
    /*if (argc != 2) {
        usage("insuffient auguments");
        exit(-1);
    }*/
    /*
     * open framebuffer device
     */
    if((fb_device = getenv("FRAMEBUFFER")) == NULL)
    { 
        fb_device = FB_DEV; 
    }
        
    fbdev = fb_open(fb_device);
    /*
     * get status of framebuffer device
     */
    fb_stat(fbdev, &fb_width, &fb_height, &fb_depth);
    printf("%d ", fb_width);
    printf("%d ", fb_height);
    printf("%d\n", fb_depth);
    /*
     * map framebuffer device to shared memory
     */
    screensize = fb_width * fb_height * fb_depth / 8;
    fbmem = fb_mmap(fbdev, screensize);
    if(fbmem == 0)
    {
        printf("fb mmap error");
    }
    /*open input jpeg file   */
    char jpgname[32] = {0};
    int jpgcnt = 0;
   // SystemSetBlankInterVal(0);
    while(1)
    {
        //for(jpgcnt = 0; jpgcnt < 10; jpgcnt++)
        {
            /*
            * init jpeg decompress object error handler
            */
            sprintf(jpgname, "%s","./picture.jpg");
            //sprintf(jpgname, "/var/run/%d.jpg", jpgcnt);
            printf("begint to display %s\n", jpgname);
            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_decompress(&cinfo);
            if((infile = fopen(jpgname, "rb+")) == NULL)
            {
                fprintf(stderr, "open %s failed\n", "1.jpg");
                exit(-1);
            }
            /*
             * bind jpeg decompress object to infile
             */
            jpeg_stdio_src(&cinfo, infile);
            /*
             * read jpeg header
             */
            jpeg_read_header(&cinfo, TRUE);
            /*
             * decompress process.
             * note: after jpeg_start_decompress() is called
             * the dimension infomation will be known,
             * so allocate memory buffer for scanline immediately
             */
            jpeg_start_decompress(&cinfo);
            /*if ((cinfo.output_width > fb_width) ||
                (cinfo.output_height > fb_height)) {
                printf("too large JPEG file,cannot display\n");
                return (-1);
            }*/
            buffer = (unsigned char *) malloc(cinfo.output_width *
                                              cinfo.output_components);
            y = 0;
            while(cinfo.output_scanline < cinfo.output_height)
            {
                jpeg_read_scanlines(&cinfo, &buffer, 1);
                if(fb_depth == 16)
                {
                    unsigned short  color;
                    for(x = 0; x < cinfo.output_width; x++)
                    {
                        color = RGB888toRGB565(buffer[x * 3],
                                               buffer[x * 3 + 1]
                                               , buffer[x * 3 + 2]);
                        fb_pixel(fbmem, fb_width, fb_height, x, y, color);
                    }
                }
                else if(fb_depth == 24)
                {
                    memcpy((unsigned char *) fbmem + y * fb_width * 3,
                           buffer, cinfo.output_width * cinfo.output_components);
                }
                y++;                    // next scanline
            }
            /*
             * finish decompress, destroy decompress object
             */
            jpeg_finish_decompress(&cinfo);
            jpeg_destroy_decompress(&cinfo);
            /*
             * release memory buffer
             */
            free(buffer);
            /*
             * close jpeg inputing file
             */
            fclose(infile);
            usleep(400000);
        }
    }
    /*
     * unmap framebuffer's shared memory
     */
    fb_munmap(fbmem, screensize);
    /*
     * close framebuffer device
     */
    fb_close(fbdev);
    return (0);
}
