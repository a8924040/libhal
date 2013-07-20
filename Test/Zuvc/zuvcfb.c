#if 1
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
#define NTSC_WIDTH 80   // 
#define NTSC_HEIGHT 60
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
    if (!dev)
    {
        dev = DEFAULT_DEVICE;
    } ;
    
    if ((vd->fd = open(dev, O_RDWR, 10705)) < 0)
    {
        return -1;
    };
    
    if (v4l_get_capability(vd) < 0)
    {
        return -1;
    }
    
    if (v4l_get_picture(vd) < 0)
    {
        return -1;
    }
    
    return 0;
}


int v4l_get_capability(v4ldevice *vd)

{
    if (ioctl(vd->fd, VIDIOCGCAP, &(vd->capability)) < 0)
    {
        perror("v4l_get_capability:");
        return -1;
    };
    
    return 0;
}


int v4l_get_picture(v4ldevice *vd)

{
    if (ioctl(vd->fd, VIDIOCGPICT, &(vd->picture)) < 0)
    {
        perror("v4l_get_picture");
        return -1;
    };
    
    return 0;
}




int v4l_set_norm(v4ldevice *vd, int nrm)


{
    int i;
    
    for (i = 0; i < vd->capability.channels ; i++)
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
    if (v4l_get_mbuf(vd) < 0)
    {
        return -1;
    }
    
    if ((vd->map = mmap(0, vd->mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, vd->fd, 0)) < 0)
    {
        return -1;
    }
    
    return 0;
}


int v4l_get_mbuf(v4ldevice *vd)

{
    if (ioctl(vd->fd, VIDIOCGMBUF, &(vd->mbuf)) < 0)
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
    
    if (ioctl(vd->fd, VIDIOCMCAPTURE, &(vd->mmap)) < 0)
    {
        exit(-1);
        return -1;
    }
    
    vd->framestat[frame] = 1;
    return 0;
}

int v4l_grab_sync(v4ldevice *vd, int frame)

{
    if (ioctl(vd->fd, VIDIOCSYNC, &frame) < 0)
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

#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <System.h>
#include "Smtp.h"

int main()

{
    v4ldevice VD;
    v4ldevice *vd = &VD;
    int frame = 0;
    int f_d;
    int NUM, write_number = 0;

    S8 s8EmailTo[128] = {"tango_zhu@126.com"};/* �ʼ����͵�ַ */
    S8 s8EmailFrom[128] = {"tango_zhu@163.com"};/* �ʼ������˻� */
    S8 s8EmailPwd[128]= {"3306492351"}; /* �ʼ������˻����� */
    S8 s8EmailTitle[128]={"test"};/* �ʼ����� */
    S8 s8EmailBody[1024]={"It's an email test with two jpeg picture!"};/* �ʼ����� */ 
    S8 s8EmailAccount[128] ={"tango_zhu"};
    S8 s8AttachName[32] = {"picture.jpg"};    
    S8 s8AttachPath[32] = {"./picture.jpg"};
    int i;
    EMAIL_INFO_STRU stEmailInfo;
    ATTACHMENT_ARRAY_STRU stAttach;
    stEmailInfo.ps8EmailBody = s8EmailBody;
    stEmailInfo.ps8EmailFrom = s8EmailFrom;
    stEmailInfo.ps8EmailPwd = s8EmailPwd;
    stEmailInfo.ps8EmailTitle = s8EmailTitle;
    stEmailInfo.ps8EmailTo = s8EmailTo;   
    stEmailInfo.ps8EmailAccount = s8EmailAccount;
    memset(&stAttach, 0, sizeof(ATTACHMENT_ARRAY_STRU));
    stAttach.stAttachment[0].ps8FileName = s8AttachName;
    stAttach.stAttachment[0].ps8FilePath = s8AttachPath;
    stAttach.u32AttachSum = 1;


    printf("f_d = %d \n", f_d);
    
    if (0 == v4l_open("/dev/video0", vd))
    {
        printf("open success!\n");
    }
    
    else
    {
        printf("open failure\n");
    }
    
    if (0 == v4l_set_norm(vd, norm))
    {
        printf("set_norm success\n");
    }
    
    else
    {
        printf("set_norm failure\n");
    }
    
    if (0 == v4l_grab_init(vd, NTSC_WIDTH, NTSC_HEIGHT))
    {
        printf("init success!\n");
    }
    
    else
    {
        printf("init failure\n");
    }
    
    if (0 == v4l_mmap_init(vd))
    {
        printf("memory map success!\n");
    }
    
    else
    {
        printf("memory map failure\n");
    }
    
    while (1)
    {
        f_d = open(DEFAULT_FILE_NAME, O_RDWR | O_CREAT, 0666);
        
        if (0 == v4l_grab_start(vd, frame))
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
        write_number = write(f_d, buffer, NTSC_WIDTH * 3 * NTSC_HEIGHT); //ÿһ֡���ֽ��� 80*60*3=14400byte
        printf("write_number = %d bytes\n", write_number);
        close(f_d);        sleep(10);

        i++;
        printf("send email %d times \n",i);
        SendEmail(&stEmailInfo, &stAttach);
        sleep(1800);
    
        
    }
    
    v4l_close(vd);
    return 0;
}
#endif

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>



#define CLEAR(x) memset (&(x), 0, sizeof (x))



struct buffer
{
    void *start;
    size_t length;
};



static char *dev_name = "/dev/video0"; //����ͷ�豸��
static int fd = -1;
struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
FILE *file_fd;

static unsigned long file_length;
static unsigned char *file_name;

//////////////////////////////////////////////////////

//��ȡһ֡����

//////////////////////////////////////////////////////

static int read_frame(void)

{
    struct v4l2_buffer buf;
    unsigned int i;
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    /*8.��������ȡ���Ѳɼ����ݵ�֡���壬ȡ��ԭʼ�ɼ����ݡ�VIDIOC_DQBUF*/
    int ff = ioctl(fd, VIDIOC_DQBUF, &buf);
    
    if (ff < 0)
    {
        printf("failture\n");
    } //���вɼ���֡����
    
    assert(buf.index < n_buffers);
    printf("buf.index dq is %d,\n", buf.index);
    printf("buffers[buf.index].start = #x, buffers[buf.index].length = #x\n", buffers[buf.index].start, buffers[buf.index].length);
    fwrite(buffers[buf.index].start, buffers[buf.index].length, 1, file_fd); //����д���ļ���
    /*9.���������������β,��������ѭ���ɼ���VIDIOC_QBUF*/
    ff = ioctl(fd, VIDIOC_QBUF, &buf); //�ٽ�������
    
    if (ff < 0) //�����ݴӻ����ж�ȡ����
    {
        printf("failture VIDIOC_QBUF\n");
    }
    
    return 1;
}



int main(int argc, char **argv)

{
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    unsigned int i;
    enum v4l2_buf_type type;
    file_fd = fopen("test-mmap.jpg", "w");//ͼƬ�ļ���
    /*1.���豸�ļ��� int fd=open(��/dev/video0��,O_RDWR);*********/
    fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0); //���豸
    /*2.ȡ���豸��capability�������豸����ʲô���ܣ������Ƿ������Ƶ����,������Ƶ��������ȡ�VIDIOC_QUERYCAP,struct v4l2_capability*/
    int ff = ioctl(fd, VIDIOC_QUERYCAP, &cap); //��ȡ����ͷ����
    
    if (ff < 0)
    {
        printf("failture VIDIOC_QUERYCAP\n");
    }
    
    /*3.������Ƶ����ʽ��֡��ʽ����ʽ����PAL��NTSC��֡�ĸ�ʽ��������Ⱥ͸߶ȵȡ�*/
    struct v4l2_fmtdesc fmt1;
    int ret;
    memset(&fmt1, 0, sizeof(fmt1));
    fmt1.index = 0;
    fmt1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    //��ȡ��ǰ����֧�ֵ���Ƶ��ʽ
    while ((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt1)) == 0)
    {
        fmt1.index++;
        printf("{ pixelformat = '%c%c%c%c', description = '%s' }\n", fmt1.pixelformat & 0xFF, (fmt1.pixelformat >> 8) & 0xFF, (fmt1.pixelformat >> 16) & 0xFF, (fmt1.pixelformat >> 24) & 0xFF, fmt1.description);
    }
    
    //֡�ĸ�ʽ�������ȣ��߶ȵ�
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //���������ͣ�������Զ��V4L2_BUF_TYPE_VIDEO_CAPTURE
    fmt.fmt.pix.width = 640;//��������16�ı���
    fmt.fmt.pix.height = 480;////�ߣ�������16�ı���
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;//��Ƶ���ݴ洢����//V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YVU420;//V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    //���õ�ǰ������Ƶ�����ʽ
    ff = ioctl(fd, VIDIOC_S_FMT, &fmt);
    
    if (ff < 0)
    {
        printf("failture VIDIOC_S_FMT\n");
    }
    
    //����ͼƬ��С
    file_length = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    /*4.����������֡���壬һ�㲻����5����struct v4l2_requestbuffers*/
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = 1;//����������Ҳ����˵�ڻ�������ﱣ�ֶ�������Ƭ
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;//��V4L2_MEMORY_USERPTR
    ff = ioctl(fd, VIDIOC_REQBUFS, &req);  //���뻺�壬count�����������
    
    if (ff < 0)
    {
        printf("failture VIDIOC_REQBUFS\n");
    }
    
    if (req.count < 1)
    {
        printf("Insufficient buffer memory\n");
    }
    
    buffers = (struct buffer *)calloc(req.count, sizeof(*buffers)); //�ڴ��н�����Ӧ�ռ�
    
    /*5.�����뵽��֡����ӳ�䵽�û��ռ䣬�����Ϳ���ֱ�Ӳ����ɼ�����֡�ˣ�������ȥ���ơ�mmap*/
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        struct v4l2_buffer buf; //�����е�һ֡
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        
        //��VIDIOC_REQBUFS�з�������ݻ���ת���������ַ
        if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf))  //ӳ���û��ռ�
        {
            printf("VIDIOC_QUERYBUF error\n");
        }
        
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */
                                        , MAP_SHARED /* recommended */, fd, buf.m.offset); //ͨ��mmap����ӳ���ϵ,����ӳ��������ʼ��ַ
                                        
        if (MAP_FAILED == buffers[n_buffers].start)
        {
            printf("mmap failed\n");
        }
    }
    
    /*6.�����뵽��֡����ȫ������У��Ա��Ųɼ���������.VIDIOC_QBUF,struct v4l2_buffer*/
    for (i = 0; i < n_buffers; ++i)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        
        //�����ݴӻ����ж�ȡ����
        if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) //���뵽�Ļ�������ж�
        {
            printf("VIDIOC_QBUF failed\n");
        }
    }
    
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    /*7.��ʼ��Ƶ�Ĳɼ���VIDIOC_STREAMON*/
    if (-1 == ioctl(fd, VIDIOC_STREAMON, &type))  //��ʼ��׽ͼ������
    {
        printf("VIDIOC_STREAMON failed\n");
    }
    
    for (;;) //��һ���漰���첽IO
    {
        fd_set fds;
        struct timeval tv;
        int r;
        FD_ZERO(&fds); //��ָ����?�������������
        FD_SET(fd, &fds); //���ļ������������и��µ��ļ�������
        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        r = select(fd + 1, &fds, NULL, NULL, &tv); //�ж��Ƿ�ɶ���������ͷ�Ƿ�׼���ã���tv�Ƕ�ʱ
        
        if (-1 == r)
        {
            if (EINTR == errno)
            {
                continue;
            }
            
            printf("select err\n");
        }
        
        if (0 == r)
        {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }
        
        if (read_frame()) //����ɶ���ִ��read_frame ()������������ѭ��
        {
            break;
        }
    }
    
unmap:

    for (i = 0; i < n_buffers; ++i)
    {
        if (-1 == munmap(buffers->start, buffers->length))
        {
            printf("munmap error");
        }
    }
    
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    /*10.ֹͣ��Ƶ�Ĳɼ���VIDIOC_STREAMOFF*/
    if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
    {
        printf("VIDIOC_STREAMOFF");
    }
    
    /*11.�ر���Ƶ�豸��close(fd);*/
    close(fd);
    fclose(file_fd);
    exit(EXIT_SUCCESS);
    return 0;
}

#endif

