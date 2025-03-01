#ifndef __VDEC_H__
#define __VDEC_H__

#include "cvi_sys.h"
#include "cvi_vdec.h"
#include "config.h"

typedef struct _VDEC_PARAM_S
{
    VDEC_CHN VdecChn;
    VDEC_CHN_ATTR_S stChnAttr;
    CVI_CHAR decode_file_name[64];
    CVI_BOOL stop_thread;
    // pthread_t vdec_thread;
    RECT_S stDispRect;
    CVI_U32 bind_mode;
    VB_SOURCE_E vdec_vb_source;
    PIXEL_FORMAT_E vdec_pixel_format;
} VDEC_PARAM_S;

typedef struct _VDEC_CONFIG_S
{
    CVI_S32 s32ChnNum;
    VDEC_PARAM_S astVdecParam[MAX_VDEC_NUM];
} VDEC_CONFIG_S;

typedef struct _VDEC_VIDEO_ATTR
{
    VIDEO_DEC_MODE_E enDecMode;
    CVI_U32 u32RefFrameNum;
    DATA_BITWIDTH_E enBitWidth;
} VDEC_VIDEO_ATTR;

typedef struct _VDEC_PICTURE_ATTR
{
    CVI_U32 u32Alpha;
} VDEC_PICTURE_ATTR;

typedef struct _SAMPLE_VDEC_ATTR
{
    PAYLOAD_TYPE_E enType;
    PIXEL_FORMAT_E enPixelFormat;
    VIDEO_MODE_E enMode;
    CVI_U32 u32Width;
    CVI_U32 u32Height;
    CVI_U32 u32FrameBufCnt;
    CVI_U32 u32DisplayFrameNum;
    union
    {
        VDEC_VIDEO_ATTR stSampleVdecVideo;     /* structure with video ( h265/h264) */
        VDEC_PICTURE_ATTR stSampleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
    };
} VDEC_ATTR;
CVI_S32 VDEC_INIT(CVI_S32 decoding_file_num, VDEC_CONFIG_S *stVdecCfg);
CVI_S32 VDEC_DESTROY(VDEC_CONFIG_S *vdecCtx);
int write_yuv(FILE *out_f, VIDEO_FRAME_S stVFrame);

#endif /* __VDEC_H__ */