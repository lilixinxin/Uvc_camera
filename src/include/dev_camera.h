#ifndef __DEV_CAMERA_H__
#define __DEV_CAMERA_H__

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

void usage(int argc);

/*open video device*/
int Uvc_OpenDevice(char *video_name);

/*get device cap*/
int Uvc_GetDeviceCap(struct v4l2_capability *cap);

/*get video fmt*/
int Uvc_GetVideoFmt(struct v4l2_fmtdesc *fmt);

/*Set video fmt*/
int Uvc_SetDeviceFmt(struct v4l2_format *format, int Width, int Height);

/*申请内存缓冲区并且映射*/
int Uvc_ReqBuf(struct v4l2_requestbuffers *reqbuf, struct v4l2_buffer *buf, int video_count);

/*stream on*/
int Uvc_StreamOn(struct v4l2_buffer *buf, int video_count, enum v4l2_buf_type *type);

/*write file*/
int Uvc_SaveFile(char *outfile, struct v4l2_buffer *buf, int video_count);


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
#endif