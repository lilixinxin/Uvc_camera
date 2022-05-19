#if 0
1.打开video节点
2.查看设备信息和能力
3.查看设备支持的数据格式
4.设置视频格式
5.申请内存缓冲区并且映射到内核空间
struct v4l2_capability {
    __u8    driver[16]; /* i.e. "bttv" */
    __u8    card[32];   /* i.e. "Hauppauge WinTV" */
    __u8    bus_info[32];   /* "PCI:" + pci_name(pci_dev) */
    __u32   version;        /* should use KERNEL_VERSION() */
    __u32   capabilities;   /* Device capabilities */
    __u32   reserved[4];
};


/*
 *	F O R M A T   E N U M E R A T I O N
 */
struct v4l2_fmtdesc {
	__u32		    index;             /* Format number      */
	__u32		    type;              /* enum v4l2_buf_type */
	__u32               flags;
	__u8		    description[32];   /* Description string */
	__u32		    pixelformat;       /* Format fourcc      */
	__u32		    reserved[4];
};

/**
 * struct v4l2_format - stream data format
 * @type:	enum v4l2_buf_type; type of the data stream
 * @pix:	definition of an image format
 * @pix_mp:	definition of a multiplanar image format
 * @win:	definition of an overlaid image
 * @vbi:	raw VBI capture or output parameters
 * @sliced:	sliced VBI capture or output parameters
 * @raw_data:	placeholder for future extensions and custom formats
 */
struct v4l2_format {
	__u32	 type;
	union {
		struct v4l2_pix_format		pix;     /* V4L2_BUF_TYPE_VIDEO_CAPTURE */
		struct v4l2_pix_format_mplane	pix_mp;  /* V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE */
		struct v4l2_window		win;     /* V4L2_BUF_TYPE_VIDEO_OVERLAY */
		struct v4l2_vbi_format		vbi;     /* V4L2_BUF_TYPE_VBI_CAPTURE */
		struct v4l2_sliced_vbi_format	sliced;  /* V4L2_BUF_TYPE_SLICED_VBI_CAPTURE */
		struct v4l2_sdr_format		sdr;     /* V4L2_BUF_TYPE_SDR_CAPTURE */
		struct v4l2_meta_format		meta;    /* V4L2_BUF_TYPE_META_CAPTURE */
		__u8	raw_data[200];                   /* user-defined */
	} fmt;
};

struct v4l2_requestbuffers {
	__u32			count;
	__u32			type;		/* enum v4l2_buf_type */
	__u32			memory;		/* enum v4l2_memory */
	__u32			reserved[2];
};

struct v4l2_buffer {
	__u32			index;
	__u32			type;
	__u32			bytesused;
	__u32			flags;
	__u32			field;
	struct timeval		timestamp;
	struct v4l2_timecode	timecode;
	__u32			sequence;

	/* memory location */
	__u32			memory;
	union {
		__u32           offset;
		unsigned long   userptr;
		struct v4l2_plane *planes;
		__s32		fd;
	} m;
	__u32			length;
	__u32			reserved2;
	__u32			reserved;
};

#endif

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/videodev2.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define video_num  5

struct videobuffer{
    unsigned int length;
    void* start;
};

struct videobuffer framebuf[video_num];
static int fd = 0;

void usage(int argc)
{

    printf("please input video name height width count");
    printf("./main /dev/video45 640 480 3");
    
}

/*open video device*/
int Uvc_OpenDevice(char *video_name)
{
    fd = open(video_name, O_RDWR);
    if(fd < 0)
    {
        perror("open fail");
        return -1;
    }
    return 0;
}
/*get device cap*/
int Uvc_GetDeviceCap(struct v4l2_capability *cap)
{
    int ret = 0;

    ret = ioctl(fd, VIDIOC_QUERYCAP, cap);
    if(ret == -1)
    {
        perror("ioctl fail");
        return -1;
    }

    fprintf(stderr, "cap driver_name:%s \ncapabilities:%x\n", cap->driver, cap->capabilities);
    return 0;
}
/*get video fmt*/
int Uvc_GetVideoFmt(struct v4l2_fmtdesc *fmt)
{
    memset(fmt, 0, sizeof(*fmt));
    fmt->index = 0;
    fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while(ioctl(fd, VIDIOC_ENUM_FMT, fmt) != -1)
    {
        fprintf(stderr, "index:%d   \npixelformat:%c%c%c%c  \ndescription:%s\n",fmt->index, fmt->pixelformat&0xff, \
        (fmt->pixelformat>>8)&0xff,(fmt->pixelformat>>16)&0xff, \
        (fmt->pixelformat>>24)&0xff,fmt->description);
        fmt->index++;
    }
    return 0;
}

/*Set video fmt*/
int Uvc_SetDeviceFmt(struct v4l2_format *format, int Width, int Height)
{
    int ret = 0;

    memset(format, 0, sizeof(*format));
    format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format->fmt.pix.width = Width;
    format->fmt.pix.height = Height;
    format->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format->fmt.pix.field = V4L2_FIELD_INTERLACED;

    ret = ioctl(fd, VIDIOC_S_FMT, format);
    if(ret == -1)
    {
        fprintf(stderr, "ioctl s fmt fail\n");
        return -1;
    }

    memset(format, 0, sizeof(*format));
    format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_G_FMT, format);
    if(ret < 0)
    {
        printf("VIDIOC_G_FMT fail\n");
        return -1;
    }
    printf("-----------------VIDIOC_G_FMT----------------------\n");
    printf("width:%d   \nheight:%d   \ntype:%x   pixelformat:%c%c%c%c\n",format->fmt.pix.width,format->fmt.pix.height,
        format->type,format->fmt.pix.pixelformat&0xff,(format->fmt.pix.pixelformat>>8)&0xff,(format->fmt.pix.pixelformat>>16)&0xff,
        (format->fmt.pix.pixelformat>>24)&0xff);

    return 0;
}

/*申请内存缓冲区并且映射*/
int Uvc_ReqBuf(struct v4l2_requestbuffers *reqbuf, struct v4l2_buffer *buf, int video_count)
{
    //申请内存缓冲区
    int ret = 0;

    memset(reqbuf, 0, sizeof(*reqbuf));
    reqbuf->count = video_count;
    reqbuf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf->memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_REQBUFS, reqbuf);
    if(ret == -1)
    {
        printf("VIDIOC_REQBUFS fail\n");
        return -1;
    }

    for(int i = 0; i < reqbuf->count; i++){
        memset(buf, 0, sizeof(*buf));
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd, VIDIOC_QUERYBUF, buf);

        framebuf[i].length = buf->length;
        framebuf[i].start = mmap(NULL, buf->length, PROT_READ|PROT_WRITE, 
            MAP_SHARED, fd, buf->m.offset);
        if(framebuf[i].start == MAP_FAILED){
            perror("mmap fail.\n");
            return -1;
        }
        printf("start:%x  length:%d\n",(unsigned int)framebuf[i].start,framebuf[i].length);
    }
    return 0;
}

/*stream on*/
int Uvc_StreamOn(struct v4l2_buffer *buf, int video_count, enum v4l2_buf_type *type)
{
    int ret = 0;

    for(int i = 0; i < video_count; i++){
        memset(buf, 0, sizeof(*buf));
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        buf->index = i;
        ret = ioctl(fd, VIDIOC_QBUF, buf);
        if(0 != ret){
            perror("VIDIOC_QBUF fail.\n");
            return -1;
        }
    }

    *type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, type);
    if(ret == -1)
    {
        perror("VIDIOC_STREAMON fail\n");
        return -1;
    }
    return 0;
}
/*write file*/
int Uvc_SaveFile(char *outfile, struct v4l2_buffer *buf, int video_count)
{
    // 直接保存的yuyv数据
    int ret = 0;
    
    FILE  *filefd  = NULL;
    filefd = fopen(outfile, "wb");
    if(filefd == NULL)
    {
        perror("open file fail\n");
        return -1;
    }

    for(int index = 0; index < video_count; index++)
    {
        memset(buf, 0, sizeof(*buf));
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd, VIDIOC_DQBUF, buf);
        if(0 != ret){
            perror("VIDIOC_QBUF fail.\n");
            return -1;        
        }
        fwrite((char*)framebuf[buf->index].start, 1, buf->length, filefd);

    }
       
    fclose(filefd);
    return 0;

}
int  main(int argc, char *argv[])
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmt;
    struct v4l2_format format;
    struct v4l2_requestbuffers reqbuf;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;
    char video_name[24] = {0};
    char outfile[24] = {0};
    int ret = 0;
    int Width, Height;

    usage(argc);

    strcpy(outfile, argv[1]);
    strcpy(video_name, argv[2]);
    Width = atoi(argv[3]);
    Height = atoi(argv[4]);

    fprintf(stderr, "out file :%s \ninput video_name:%s \nWidth=%d Height = %d\n ", outfile, video_name, Width, Height);


    ret = Uvc_OpenDevice(video_name);
    if(ret != 0)
        perror("open device fail\n");

    ret = Uvc_GetDeviceCap(&cap);
    if(ret != 0)
        perror("Uvc_GetDeviceCap fail\n");

    ret = Uvc_GetVideoFmt(&fmt);
    if(ret != 0)
        perror("Uvc_GetVideoFmt fail\n");

    ret = Uvc_SetDeviceFmt(&format, Width, Height);
    if(ret != 0)
        perror("Uvc_SetDeviceFmt fail\n");

    ret = Uvc_ReqBuf(&reqbuf, &buf, video_num);
    if(ret != 0)
        perror("Uvc_ReqBuf fail\n");

    ret = Uvc_StreamOn(&buf, video_num, &type);
    if(ret != 0)
        perror("Uvc_StreamOn fail\n");

    ret = Uvc_SaveFile(outfile, &buf, video_num);
    if(ret != 0)
        perror("Uvc_SaveFile fail\n");    

    return 0;
}
