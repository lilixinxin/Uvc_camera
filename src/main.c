#include "dev_camera.h"
#include "dev_jpeg.h"

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
    FILE *jpegfd = NULL;

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
    
    uint8_t* rgb = calloc(Width * Height * 3, sizeof (uint8_t));

    jpegfd = fopen("jepg", "wb");
    if(jpegfd == NULL)
    {   
        perror("open jpeg fail\n");
        return -1;
    }
    for(int i = 0; i < video_num; i++){
        yuyv_to_rgb(rgb, framebuf[i].start, 640, 480);
        rgb_compress_jpeg(jpegfd, rgb, 640, 480, 100);
    }
    fclose(jpegfd);


    return 0;
}
