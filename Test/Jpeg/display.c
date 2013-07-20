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
