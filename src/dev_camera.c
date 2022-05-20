#include "dev_camera.h"

static int fd = 0;

/******************************************************************************
 * Function:    usage
 *
 * Description: 使用说明
 * Input:       
 * Output:      NULL
 * Returns:     NULL
 ******************************************************************************/
void usage(int argc)
{

    printf("please input video name height width count");
    printf("./main /dev/video45 640 480 3");
    
}

/******************************************************************************
 * Function:    Uvc_OpenDevice
 *
 * Description: open video device
 * Input:       video_name  
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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
/******************************************************************************
 * Function:    Uvc_GetDeviceCap
 *
 * Description: 获取设备能力
 * Input:       cap  
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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
/******************************************************************************
 * Function:    Uvc_GetVideoFmt
 *
 * Description: 获取视频格式
 * Input:       fmt  
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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

/******************************************************************************
 * Function:    Uvc_SetDeviceFmt
 *
 * Description: 设置视频格式
 * Input:       format  
 *              Width
 *              Height
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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

/******************************************************************************
 * Function:    Uvc_ReqBuf
 *
 * Description: 申请内存缓冲区并且映射
 * Input:       reqbuf  
 *              buf
 *              video_count  帧数
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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

/******************************************************************************
 * Function:    Uvc_StreamOn
 *
 * Description: 取流
 * Input:       buf  
 *              video_count 帧数
 *              type  
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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
/******************************************************************************
 * Function:    Uvc_SaveFile
 *
 * Description: 取到的数据流保存在oufile文件
 * Input:       outfile  
 *              buf 
 *              video_count  帧数
 * Output:      NULL
 * Returns:     0 success
 *              -1 fail
 ******************************************************************************/
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
