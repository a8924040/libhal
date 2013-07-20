#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <asm/types.h> 
#include <linux/videodev2.h>
#include <linux/fb.h>

static unsigned int capframe = 0;		//保存的图片计数
static char filename[30];		//保存的图片文件名
FILE *bmpFile;					//保存图片时创建的文件指针

unsigned char bmp_head_t[] = {			//66字节的BMP位图头
         0x42,0x4d,0x42,0x58,0x02,0x00,0x00,0x00,0x00,0x00,
        0x42,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0xf0,0x00,
        0x00,0x00,0x40,0x01,0x00,0x00,0x01,0x00,0x10,0x00,
        0x03,0x00,0x00,0x00,0x00,0x58,0x02,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0xe0,0x07,
        0x00,0x00,0x1f,0x00,0x00,0x00
};
static char *vf_buff;		//从摄像头读取数据保存的内存指针
static int fb_bpp;				//帧缓冲设备的每像素位数
unsigned char bmp_head[54];			//54字节的BMP位图头

static int cam_fp = -1;			//打开的摄像头句柄
static int fb_fp = -1;			//打开的帧缓冲句柄
static char *fb_addr = NULL;		//帧缓冲mmap的映射返回地址
static char *cam_name = "/dev/video0";	//摄像头设备路径和名字
int width=0;				//帧缓冲的resolution width
int height=0;				//帧缓冲的resolution height



static inline int fb_init(void)
{
	int dev_fp = -1;
	int fb_size;
	struct fb_var_screeninfo vinfo;

	dev_fp = open("/dev/fb0", O_RDWR);
	if (dev_fp < 0) 
	{
		perror("/dev/fb0");
		return -1;
	}
	if (ioctl(dev_fp, FBIOGET_VSCREENINFO, &vinfo)) {	//获取FB基本信息
	        printf("Error reading variable information.\n");
		exit(1);
	}
	width=vinfo.xres;
	height=vinfo.yres;
	fb_bpp=vinfo.bits_per_pixel;	//将FB值赋值给全局变量
	if(fb_bpp==24) 
	{
	    fb_bpp=32;
	}
	fb_size=width*height*fb_bpp/8;	//一帧FB的字节大小
	
	if ((fb_addr = (char*)mmap(0, fb_size, 	
			PROT_READ | PROT_WRITE, MAP_SHARED, dev_fp, 0)) < 0) 
	{	//从文件的0位置开始在内存中自动映射fb_size大小空间
		perror("mmap()");
		return -1;
	}
	printf("%dx%d bpp:%d \n",width,height,fb_bpp);
	return dev_fp;
}
static inline int cam_init(void)
{
	int dev_fp = -1;

	dev_fp = open(cam_name, O_RDWR);
	if (dev_fp < 0) 
	{
		printf("error open %s\n",cam_name);
		return -1;
	}

	return dev_fp;
}


static inline int read_data(int fp, char *buf, int width, int height, int bpp)
{
	int ret=0;
	if ((ret = read(fp, buf, (width*height*bpp/8))) != (width*height*bpp/8)) 
	{
	    printf("read %d--0x%x\n",ret,ret);
		return 0;
	}
	return ret;
}

void writeImageToFile(unsigned int size)
{
	capframe++;       //图像序号计数
	sprintf(filename,"/mnt/mntd/test_bin/0%d.bmp",capframe);   //位图的文件名
   	bmpFile=fopen(filename, "w+");		//可写可追加内容的方式创建文件
	if(fb_bpp == 16)
	{
		fwrite(bmp_head_t,1,66,bmpFile);
	}
	else
	{
		fwrite(bmp_head,1,54,bmpFile);		//往文件中写入BMP位图头
	}
   	fwrite(vf_buff,1,size,bmpFile);			//在继续往文件中追加位图数据
   	fclose(bmpFile);
}


int main(int argc, char *argv[])
{
	struct timeval tv1;		//监控键盘输入的select时间参数
	unsigned long size;		//一帧画面的字节数
	int index=0;			//V4L2 input索引号
	struct v4l2_capability cap;		//V4L2设备功能结构体变量
	struct v4l2_input i;		//V4L2设备输入信息
	struct v4l2_framebuffer fb;		//V4L2的一帧设备缓存
	int on=1;				//控制V4L2设备overlay的参数
	int tmp;
	fd_set fds1;			//监控键盘输入的select fd_set变量
	int fd;				//监控键盘输入的select句柄
	char cmd[256];			//存储从键盘读入的字符串
	
	cam_fp = cam_init();		//获得摄像头句柄
	fb_fp = fb_init();			//获得帧缓冲句柄
	
	size=width*height*fb_bpp/8;

	if((tmp=ioctl(cam_fp, VIDIOC_QUERYCAP, &cap))<0) 
	{		//查询驱动功能
		printf("VIDIOC_QUERYCAP error, ret=0x%x\n",tmp);
		goto err;
	}
	printf("Driver:%s, Card:%s, cap=0x%x，bus info is %s\n",cap.driver,cap.card,cap.capabilities,cap.bus_info);
	
	memset(&i, 0, sizeof(i));
	i.index=index;
	if(ioctl(cam_fp, VIDIOC_ENUMINPUT, &i)<0)
	{//枚举输入源
		goto err;
	}
	printf("input name:%s\n",i.name);	
	
	index=0;
	if(ioctl(cam_fp, VIDIOC_S_INPUT, &index)<0)		//设置输入源
	{
	    printf("VIDIOC_S_INPUT failed\n");
		goto err;
    }
	if(ioctl(cam_fp, VIDIOC_S_OUTPUT, &index)<0)		//设置输出源
	{
	    
	    printf(" VIDIOC_S_OUTPUT failed\n");
	        goto err;
	}
	if(ioctl(cam_fp, VIDIOC_G_FBUF, &fb)<0)		//获取V4L2设备FB属性参数
	{
	    printf(" VIDIOC_G_FBUF failed\n");
		goto err;
	}

	printf("g_fbuf:capabilities=0x%x,flags=0x%x,width=%d,height=%d\n"
	       "pixelformat=0x%x,bytesperline=%d,colorspace=%d,base=0x%x\n",
		    fb.capability,fb.flags,fb.fmt.width,fb.fmt.height
		    ,fb.fmt.pixelformat,fb.fmt.bytesperline,fb.fmt.colorspace
		    ,fb.base);
		    
	fb.capability = cap.capabilities;	//V4L2设备功能赋值给V4L2的FB功能属性
	fb.fmt.width =width;				//LCD的FB宽度赋值给V4L2的FB宽度
	fb.fmt.height = height;				//LCD的FB高度赋值给V4L2的FB高度
	fb.fmt.pixelformat = (fb_bpp==32)?V4L2_PIX_FMT_BGR32:V4L2_PIX_FMT_RGB565;		//赋值V4L2的FB像素位数

	if(ioctl(cam_fp, VIDIOC_S_FBUF, &fb)<0)		//设置新的FB属性给摄像头
	{
	
        printf(" VIDIOC_S_FBUF failed\n");
   		goto err;
    }
    
	on = 1;
	if(ioctl(cam_fp, VIDIOC_OVERLAY, &on)<0)//使能摄像头的overlay
	{
	
        printf(" VIDIOC_OVERLAY failed\n");
		goto err;
	}
	
	vf_buff = (char*)malloc(size);
	if(vf_buff==NULL)
	{
		goto err;
	}

	if(fb_bpp==16)
	{	//16位BMP
	    *((unsigned int*)(bmp_head_t+18)) = width;
	    *((unsigned int*)(bmp_head_t+22)) = height;
	    *((unsigned short*)(bmp_head_t+28)) = 16;
	}
	else
	{
    	bmp_head[0] = 'B';
    	bmp_head[1] = 'M';
    	*((unsigned int*)(bmp_head+2)) = (width*fb_bpp/8)*height+54;		//整个位图大小
    	*((unsigned int*)(bmp_head+10)) = 54;				//从54字节开始存图像
    	*((unsigned int*)(bmp_head+14)) = 40;				//图像描述信息块的大小
    	*((unsigned int*)(bmp_head+18)) = width;
    	*((unsigned int*)(bmp_head+22)) = height;
    	*((unsigned short*)(bmp_head+26)) = 1;				//图像的plane总数
    	*((unsigned short*)(bmp_head+28)) = fb_bpp;
    	*((unsigned short*)(bmp_head+34)) = (width*fb_bpp/8)*height;		//图像数据区大小
	}
	
	while(1)
	{
	    if (!read_data(cam_fp, vf_buff, width, height, fb_bpp))		//读摄像头数据到vf_buff
	    {
		    printf("read error\n");
	    }
	    memcpy(fb_addr,vf_buff,width*height*fb_bpp/8);		//将读到的图像数据从内存拷贝到帧缓冲地址
	    fd=0;							//键盘句柄
	    tv1.tv_sec=0;
	    tv1.tv_usec=0;						//无限等待
	    FD_ZERO(&fds1);
	    FD_SET(fd,&fds1);					//绑定句柄跟监控对象
	    select(fd+1,&fds1,NULL,NULL,&tv1);			//监控键盘输入
	    if(FD_ISSET(fd,&fds1))					//如果键盘有输入
	    {
		    memset(cmd,0,sizeof(cmd));
		    read(fd,cmd,256);					//读取键盘输入的字符
		    if(strncmp(cmd,"quit",4)==0)
		    {			//如果键盘输入quit
    		    printf("-->quit\n");
    		    on=0;
    		    if(ioctl(cam_fp, VIDIOC_OVERLAY, &on)<0)//关掉V4L2设备的overlay
    		    {
    			    goto err;
    			}
    		    close(fb_fp);
    		    close(cam_fp);					//释放FB跟摄像头的句柄
    		    return 0;
		    }
		    else if(strncmp(cmd,"capt",4)==0)
		    {			//如果键盘输入capt
    		    printf("-->capture\n");
    		    printf("write to img --> ");
    		    writeImageToFile(size);				//把FB数据保存到位图中
    		    printf("OK\n");
		    }
	    }
	}

err:
	if (cam_fp)
		close(cam_fp);
	if (fb_fp)
		close(fb_fp);
	return 1;
}
