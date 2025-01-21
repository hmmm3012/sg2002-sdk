#include "cvi_sys.h"
#include "cvi_vdec.h"

#define MAX_VDEC_NUM 2

#define CHECK_RET(express, name)                                                                \
    do                                                                                          \
    {                                                                                           \
        CVI_S32 Ret;                                                                            \
        Ret = express;                                                                          \
        if (Ret != CVI_SUCCESS)                                                                 \
        {                                                                                       \
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __func__, \
                   __LINE__, Ret);                                                              \
            return Ret;                                                                         \
        }                                                                                       \
    } while (0)

typedef struct _SAMPLE_VDEC_PARAM_S
{
    VDEC_CHN VdecChn;
    VDEC_CHN_ATTR_S stChnAttr;
    CVI_CHAR decode_file_name[64];
    CVI_BOOL stop_thread;
    pthread_t vdec_thread;
    RECT_S stDispRect;
    CVI_U32 bind_mode;
    VB_SOURCE_E vdec_vb_source;
    PIXEL_FORMAT_E vdec_pixel_format;
} SAMPLE_VDEC_PARAM_S;

typedef struct _SAMPLE_VDEC_CONFIG_S
{
    CVI_S32 s32ChnNum;
    SAMPLE_VDEC_PARAM_S astVdecParam[MAX_VDEC_NUM];
} SAMPLE_VDEC_CONFIG_S;

typedef struct _SAMPLE_VDEC_VIDEO_ATTR
{
    VIDEO_DEC_MODE_E enDecMode;
    CVI_U32 u32RefFrameNum;
    DATA_BITWIDTH_E enBitWidth;
} SAMPLE_VDEC_VIDEO_ATTR;

typedef struct _SAMPLE_VDEC_PICTURE_ATTR
{
    CVI_U32 u32Alpha;
} SAMPLE_VDEC_PICTURE_ATTR;

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
        SAMPLE_VDEC_VIDEO_ATTR stSampleVdecVideo;     /* structure with video ( h265/h264) */
        SAMPLE_VDEC_PICTURE_ATTR stSampleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
    };
} SAMPLE_VDEC_ATTR;

static CVI_VOID *s_h264file[MAX_VDEC_NUM];
CVI_S32 VDEC_BIND_VPSS_BIND_VENC(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);