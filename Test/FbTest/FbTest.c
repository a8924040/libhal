#include <System.h>
#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <math.h>
#include <linux/fb.h>


#if 1
#define PAGE_SHIFT  12
#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))

#define TRUE    1
#define FALSE   0

//#define MIN(x,y)        ((x)>(y)?(y):(x))
//#define MAX(x,y)        ((x)>(y)?(x):(y))

typedef struct fbdev
{
    int fb;
    unsigned long fb_mem_offset;
    unsigned long fb_mem;
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    char dev[20];
} FBDEV, *PFBDEV;

typedef struct point
{
    int x;
    int y;
    int z;
} POINT, *PPOINT;

/* These math function Writed by
 * kf701 <kf701.ye@gmail.com>
 * 2007-04-05 HeFei
 */
void draw_dot(PFBDEV pFbdev, POINT p, uint8_t r, uint8_t g, uint8_t b);
void draw_line(PFBDEV pFbdev, POINT p1, POINT p2, uint8_t r, uint8_t g, uint8_t b);
void draw_polygon(PFBDEV pFbdev, uint32_t num, PPOINT array, uint8_t r, uint8_t g, uint8_t b);
void draw_triangle(PFBDEV pFbdev, POINT p1, POINT p2, POINT p3, uint8_t r, uint8_t g, uint8_t b);
void draw_circle(PFBDEV pFbdev, POINT center, uint32_t radius, uint8_t r, uint8_t g, uint8_t b);
void draw_parabola_x(PFBDEV pFbdev, POINT center, int a, uint8_t r, uint8_t g, uint8_t b);
void draw_parabola_y(PFBDEV pFbdev, POINT center, int a, uint8_t r, uint8_t g, uint8_t b);

int fb_open(PFBDEV pFbdev)
{
    pFbdev->fb = open(pFbdev->dev, O_RDWR);
    if(pFbdev->fb < 0)
    {
        printf("Error opening %s: %m. Check kernel config\n", pFbdev->dev);
        return FALSE;
    }
    if(-1 == ioctl(pFbdev->fb, FBIOGET_VSCREENINFO, & (pFbdev->fb_var)))
    {
        printf("ioctl FBIOGET_VSCREENINFO\n");
        return FALSE;
    }
    if(-1 == ioctl(pFbdev->fb, FBIOGET_FSCREENINFO, & (pFbdev->fb_fix)))
    {
        printf("ioctl FBIOGET_FSCREENINFO\n");
        return FALSE;
    }
    pFbdev->fb_mem_offset = (unsigned long)(pFbdev->fb_fix.smem_start) & (~PAGE_MASK);
    pFbdev->fb_mem = (unsigned long int) mmap(NULL, pFbdev->fb_fix.smem_len +
                     pFbdev->fb_mem_offset,
                     PROT_READ | PROT_WRITE, MAP_SHARED, pFbdev->fb, 0);
    if(-1L == (long) pFbdev->fb_mem)
    {
        printf("mmap error! mem:%ld offset:%ld\n", pFbdev->fb_mem,
               pFbdev->fb_mem_offset);
        return FALSE;
    }
    printf("The xres is :%d\n", pFbdev->fb_var.xres);
    printf("The yres is :%d\n", pFbdev->fb_var.yres);
    printf("depth(bits per pixel) =  %d\n", pFbdev->fb_var.bits_per_pixel);
    printf("smemlen = %d\n", pFbdev->fb_fix.smem_len);
    printf("fix_line(in byte) = %d\n", pFbdev->fb_fix.line_length);
    return TRUE;
}

void fb_close(PFBDEV pFbdev)
{
    close(pFbdev->fb);
    pFbdev->fb = -1;
}

void fb_memcpy(void *addr, void *color, size_t len)
{
    memcpy(addr, color, len);
}

void draw_dot(PFBDEV pFbdev, POINT p, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t offset;
    uint8_t color[4];
    color[0] = b;
    color[1] = g;
    color[2] = r;
    color[3] = 0x0;
    offset = p.y * pFbdev->fb_fix.line_length + 4 * p.x;
    fb_memcpy((void *)(pFbdev->fb_mem + pFbdev->fb_mem_offset + offset), color, 4);
}

void draw_line(PFBDEV pFbdev, POINT p1, POINT p2, uint8_t r, uint8_t g, uint8_t b)
{
    POINT p;
    int x, y;
    for(x = MIN(p1.x, p2.x); x <= MAX(p1.x, p2.x); x++)
    {
        y = (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x) + p1.y;
        p.x = x;
        p.y = y;
        draw_dot(pFbdev, p, r, g, b);
    }
}

void draw_triangle(PFBDEV pFbdev, POINT p1, POINT p2, POINT p3, uint8_t r, uint8_t g, uint8_t b)
{
    POINT p[3];
    p[0] = p1;
    p[1] = p2;
    p[2] = p3;
    draw_polygon(pFbdev, 3, p, r, g, b);
}

void draw_polygon(PFBDEV pFbdev, uint32_t num, PPOINT array, uint8_t r, uint8_t g, uint8_t b)
{
    int i;
    for(i = 0; i < num; i++)
    {
        draw_line(pFbdev, array[i], array[(i + 1) % num], r, g, b);
    }
}

void draw_circle(PFBDEV pFbdev, POINT center, uint32_t radius, uint8_t r, uint8_t g, uint8_t b)
{
    POINT p;
    int x, y, tmp;
    for(x = center.x - radius; x <= center.x + radius; x++)
    {
        p.x = x;
        tmp = sqrt(radius * radius - (x - center.x) * (x - center.x));
        p.y = center.y + tmp;
        draw_dot(pFbdev, p, r, g, b);
        p.y = center.y - tmp;
        draw_dot(pFbdev, p, r, g, b);
    }
    for(y = center.y - radius; y <= center.y + radius; y++)
    {
        p.y = y;
        tmp = sqrt(radius * radius - (y - center.y) * (y - center.y));
        p.x = center.x + tmp;
        draw_dot(pFbdev, p, r, g, b);
        p.x = center.x - tmp;
        draw_dot(pFbdev, p, r, g, b);
    }
}

void draw_parabola_x(PFBDEV pFbdev, POINT center, int a, uint8_t r, uint8_t g, uint8_t b)
{
    int x;
    POINT p;
    for(x = center.x - 100; x < center.x + 100; x++)
    {
        p.x = x;
        p.y = (x - center.x) * (x - center.x) / a + center.y;
        draw_dot(pFbdev, p, r, g, b);
    }
}

void draw_parabola_y(PFBDEV pFbdev, POINT center, int a, uint8_t r, uint8_t g, uint8_t b)
{
    int y;
    POINT p;
    for(y = center.y - 100; y < center.y + 100; y++)
    {
        p.y = y;
        p.x = (y - center.y) * (y - center.y) / a + center.x;
        draw_dot(pFbdev, p, r, g, b);
    }
}

void clear_screen(PFBDEV pFbdev)
{
    memset(pFbdev->fb_mem, 0, pFbdev->fb_fix.smem_len);
}

void ShowChinese(char *ptr)
{
    int p, q, j, k, i, a, h, z, fd[2];
    unsigned char incode[4] = "±±¾©";
    unsigned char qh[2], wh[2];
    unsigned long offset[2];
    char by[2][32], buy[2][16][2];
    unsigned char *framebuffer;
    //get the HZmatrix
    for(a = 0; a < 2; a++)
    {
        fd[a] = open("HZK16", O_RDONLY);
        qh[a] = incode[0 + a * 2] - 0xa0;
        wh[a] = incode[1 + a * 2] - 0xa0;
        offset[a] = (94 * (qh[a] - 1) + (wh[a] - 1)) * 32;
        lseek(fd[a], offset[a], SEEK_SET);
        read(fd[a], by[a], 32);
        close(fd[a]);
    }
    //store the matrix in a good way
    for(z = 0; z < 2; z++)
    {
        for(p = 0; p < 16; p++)
        {
            for(q = 0; q < 2; q++)
            { buy[z][p][q] = by[z][2 * p + q]; }
        }
    }
}
int main(void)
{
    FBDEV fb;
    memset(&fb, 0, sizeof(FBDEV));
    strcpy(fb.dev, "/dev/fb0");
    if(fb_open(&fb) == FALSE)
    {
        printf("open frame buffer error\n");
        return 0;
    }
    sleep(1);
    clear_screen(&fb);
    POINT p1, p2, p3;
    p1.x = 100;
    p1.y = 250;
    p2.x = 200;
    p2.y = 300;
    draw_line(&fb, p1, p2, 0x0, 0xff, 0x0);
    sleep(1);
    clear_screen(&fb);
    POINT center = { 100, 100, 0 };
    int radius = 30;
    draw_circle(&fb, center, radius, 0x0, 0xff, 0x0);
    sleep(1);
    clear_screen(&fb);
    int i;
    for(i = radius - 2; i >= 0; i--)
    {
        draw_circle(&fb, center, i, 0x30, 0x20, 0x0);
    }
    sleep(1);
    clear_screen(&fb);
    ShowChinese(NULL);
    printf("o999999999999999999999\n");
    sleep(1);
    fb_close(&fb);
    return 0;
}
#endif

