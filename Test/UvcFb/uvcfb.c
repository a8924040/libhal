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

static unsigned int capframe = 0;		//�����ͼƬ����
static char filename[30];		//�����ͼƬ�ļ���
FILE *bmpFile;					//����ͼƬʱ�������ļ�ָ��

unsigned char bmp_head_t[] = {			//66�ֽڵ�BMPλͼͷ
         0x42,0x4d,0x42,0x58,0x02,0x00,0x00,0x00,0x00,0x00,
        0x42,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0xf0,0x00,
        0x00,0x00,0x40,0x01,0x00,0x00,0x01,0x00,0x10,0x00,
        0x03,0x00,0x00,0x00,0x00,0x58,0x02,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0xe0,0x07,
        0x00,0x00,0x1f,0x00,0x00,0x00
};
static char *vf_buff;		//������ͷ��ȡ���ݱ�����ڴ�ָ��
static int fb_bpp;				//֡�����豸��ÿ����λ��
unsigned char bmp_head[54];			//54�ֽڵ�BMPλͼͷ

static int cam_fp = -1;			//�򿪵�����ͷ���
static int fb_fp = -1;			//�򿪵�֡������
static char *fb_addr = NULL;		//֡����mmap��ӳ�䷵�ص�ַ
static char *cam_name = "/dev/video0";	//����ͷ�豸·��������
int width=0;				//֡�����resolution width
int height=0;				//֡�����resolution height



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
	if (ioctl(dev_fp, FBIOGET_VSCREENINFO, &vinfo)) {	//��ȡFB������Ϣ
	        printf("Error reading variable information.\n");
		exit(1);
	}
	width=vinfo.xres;
	height=vinfo.yres;
	fb_bpp=vinfo.bits_per_pixel;	//��FBֵ��ֵ��ȫ�ֱ���
	if(fb_bpp==24) 
	{
	    fb_bpp=32;
	}
	fb_size=width*height*fb_bpp/8;	//һ֡FB���ֽڴ�С
	
	if ((fb_addr = (char*)mmap(0, fb_size, 	
			PROT_READ | PROT_WRITE, MAP_SHARED, dev_fp, 0)) < 0) 
	{	//���ļ���0λ�ÿ�ʼ���ڴ����Զ�ӳ��fb_size��С�ռ�
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
	capframe++;       //ͼ����ż���
	sprintf(filename,"/mnt/mntd/test_bin/0%d.bmp",capframe);   //λͼ���ļ���
   	bmpFile=fopen(filename, "w+");		//��д��׷�����ݵķ�ʽ�����ļ�
	if(fb_bpp == 16)
	{
		fwrite(bmp_head_t,1,66,bmpFile);
	}
	else
	{
		fwrite(bmp_head,1,54,bmpFile);		//���ļ���д��BMPλͼͷ
	}
   	fwrite(vf_buff,1,size,bmpFile);			//�ڼ������ļ���׷��λͼ����
   	fclose(bmpFile);
}


int main(int argc, char *argv[])
{
	struct timeval tv1;		//��ؼ��������selectʱ�����
	unsigned long size;		//һ֡������ֽ���
	int index=0;			//V4L2 input������
	struct v4l2_capability cap;		//V4L2�豸���ܽṹ�����
	struct v4l2_input i;		//V4L2�豸������Ϣ
	struct v4l2_framebuffer fb;		//V4L2��һ֡�豸����
	int on=1;				//����V4L2�豸overlay�Ĳ���
	int tmp;
	fd_set fds1;			//��ؼ��������select fd_set����
	int fd;				//��ؼ��������select���
	char cmd[256];			//�洢�Ӽ��̶�����ַ���
	
	cam_fp = cam_init();		//�������ͷ���
	fb_fp = fb_init();			//���֡������
	
	size=width*height*fb_bpp/8;

	if((tmp=ioctl(cam_fp, VIDIOC_QUERYCAP, &cap))<0) 
	{		//��ѯ��������
		printf("VIDIOC_QUERYCAP error, ret=0x%x\n",tmp);
		goto err;
	}
	printf("Driver:%s, Card:%s, cap=0x%x��bus info is %s\n",cap.driver,cap.card,cap.capabilities,cap.bus_info);
	
	memset(&i, 0, sizeof(i));
	i.index=index;
	if(ioctl(cam_fp, VIDIOC_ENUMINPUT, &i)<0)
	{//ö������Դ
		goto err;
	}
	printf("input name:%s\n",i.name);	
	
	index=0;
	if(ioctl(cam_fp, VIDIOC_S_INPUT, &index)<0)		//��������Դ
	{
	    printf("VIDIOC_S_INPUT failed\n");
		goto err;
    }
	if(ioctl(cam_fp, VIDIOC_S_OUTPUT, &index)<0)		//�������Դ
	{
	    
	    printf(" VIDIOC_S_OUTPUT failed\n");
	        goto err;
	}
	if(ioctl(cam_fp, VIDIOC_G_FBUF, &fb)<0)		//��ȡV4L2�豸FB���Բ���
	{
	    printf(" VIDIOC_G_FBUF failed\n");
		goto err;
	}

	printf("g_fbuf:capabilities=0x%x,flags=0x%x,width=%d,height=%d\n"
	       "pixelformat=0x%x,bytesperline=%d,colorspace=%d,base=0x%x\n",
		    fb.capability,fb.flags,fb.fmt.width,fb.fmt.height
		    ,fb.fmt.pixelformat,fb.fmt.bytesperline,fb.fmt.colorspace
		    ,fb.base);
		    
	fb.capability = cap.capabilities;	//V4L2�豸���ܸ�ֵ��V4L2��FB��������
	fb.fmt.width =width;				//LCD��FB��ȸ�ֵ��V4L2��FB���
	fb.fmt.height = height;				//LCD��FB�߶ȸ�ֵ��V4L2��FB�߶�
	fb.fmt.pixelformat = (fb_bpp==32)?V4L2_PIX_FMT_BGR32:V4L2_PIX_FMT_RGB565;		//��ֵV4L2��FB����λ��

	if(ioctl(cam_fp, VIDIOC_S_FBUF, &fb)<0)		//�����µ�FB���Ը�����ͷ
	{
	
        printf(" VIDIOC_S_FBUF failed\n");
   		goto err;
    }
    
	on = 1;
	if(ioctl(cam_fp, VIDIOC_OVERLAY, &on)<0)//ʹ������ͷ��overlay
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
	{	//16λBMP
	    *((unsigned int*)(bmp_head_t+18)) = width;
	    *((unsigned int*)(bmp_head_t+22)) = height;
	    *((unsigned short*)(bmp_head_t+28)) = 16;
	}
	else
	{
    	bmp_head[0] = 'B';
    	bmp_head[1] = 'M';
    	*((unsigned int*)(bmp_head+2)) = (width*fb_bpp/8)*height+54;		//����λͼ��С
    	*((unsigned int*)(bmp_head+10)) = 54;				//��54�ֽڿ�ʼ��ͼ��
    	*((unsigned int*)(bmp_head+14)) = 40;				//ͼ��������Ϣ��Ĵ�С
    	*((unsigned int*)(bmp_head+18)) = width;
    	*((unsigned int*)(bmp_head+22)) = height;
    	*((unsigned short*)(bmp_head+26)) = 1;				//ͼ���plane����
    	*((unsigned short*)(bmp_head+28)) = fb_bpp;
    	*((unsigned short*)(bmp_head+34)) = (width*fb_bpp/8)*height;		//ͼ����������С
	}
	
	while(1)
	{
	    if (!read_data(cam_fp, vf_buff, width, height, fb_bpp))		//������ͷ���ݵ�vf_buff
	    {
		    printf("read error\n");
	    }
	    memcpy(fb_addr,vf_buff,width*height*fb_bpp/8);		//��������ͼ�����ݴ��ڴ濽����֡�����ַ
	    fd=0;							//���̾��
	    tv1.tv_sec=0;
	    tv1.tv_usec=0;						//���޵ȴ�
	    FD_ZERO(&fds1);
	    FD_SET(fd,&fds1);					//�󶨾������ض���
	    select(fd+1,&fds1,NULL,NULL,&tv1);			//��ؼ�������
	    if(FD_ISSET(fd,&fds1))					//�������������
	    {
		    memset(cmd,0,sizeof(cmd));
		    read(fd,cmd,256);					//��ȡ����������ַ�
		    if(strncmp(cmd,"quit",4)==0)
		    {			//�����������quit
    		    printf("-->quit\n");
    		    on=0;
    		    if(ioctl(cam_fp, VIDIOC_OVERLAY, &on)<0)//�ص�V4L2�豸��overlay
    		    {
    			    goto err;
    			}
    		    close(fb_fp);
    		    close(cam_fp);					//�ͷ�FB������ͷ�ľ��
    		    return 0;
		    }
		    else if(strncmp(cmd,"capt",4)==0)
		    {			//�����������capt
    		    printf("-->capture\n");
    		    printf("write to img --> ");
    		    writeImageToFile(size);				//��FB���ݱ��浽λͼ��
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
