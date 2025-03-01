#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>
#include <inttypes.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/socket.h>
#include <linux/cvi_cv181x_defines.h>

#include "cvi_buffer.h"
#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_vpss.h"
#include "cvi_vdec.h"
#include "cvi_venc.h"

#include "sys.h"



#define MAX_VDEC_NUM 2
#define MAX_STRING_LEN 255
#define NUM_OF_USER_DATA_BUF 4

// static pthread_t send_vo_thread;
// static CVI_VOID *s_h264file[MAX_VDEC_NUM];
VB_POOL g_ahLocalPicVbPool[MAX_VDEC_NUM] = {[0 ...(MAX_VDEC_NUM - 1)] = VB_INVALID_POOLID};

// #define SHOW_STATISTICS_1 1

typedef struct _chnInputCfg_
{
	char codec[64];
	unsigned int width;
	unsigned int height;
	char input_path[MAX_STRING_LEN];
	char vpssSrcPath[MAX_STRING_LEN];
	char output_path[MAX_STRING_LEN];
	char outputFileName[MAX_STRING_LEN];
	char roiCfgFile[MAX_STRING_LEN];
	char roiBinFile[MAX_STRING_LEN];
	int roideltaqp;
	char qpMapCfgFile[MAX_STRING_LEN];
	char user_data[NUM_OF_USER_DATA_BUF][MAX_STRING_LEN];
	CVI_S32 num_frames;
	CVI_S32 bsMode;
	CVI_U32 u32Profile;
	CVI_S32 rcMode;
	CVI_S32 iqp;
	CVI_S32 pqp;
	CVI_S32 gop;
	CVI_U32 gopMode;
	CVI_S32 bitrate;
	CVI_S32 minIprop;
	CVI_S32 maxIprop;
	CVI_U32 u32RowQpDelta;
	CVI_S32 firstFrmstartQp;
	CVI_S32 minIqp;
	CVI_S32 maxIqp;
	CVI_S32 minQp;
	CVI_S32 maxQp;
	CVI_S32 framerate;
	CVI_S32 quality;
	CVI_S32 maxbitrate;
	CVI_S32 s32ChangePos;
	CVI_S32 s32MinStillPercent;
	CVI_U32 u32MaxStillQP;
	CVI_U32 u32MotionSensitivity;
	CVI_S32 s32AvbrFrmLostOpen;
	CVI_S32 s32AvbrFrmGap;
	CVI_S32 s32AvbrPureStillThr;
	CVI_S32 statTime;
	CVI_S32 bind_mode;
	CVI_S32 pixel_format;
	CVI_S32 posX;
	CVI_S32 posY;
	CVI_S32 inWidth;
	CVI_S32 inHeight;
	CVI_S32 srcFramerate;
	CVI_U32 bitstreamBufSize;
	CVI_S32 single_LumaBuf;
	CVI_S32 single_core;
	CVI_S32 vpssGrp;
	CVI_S32 vpssChn;
	CVI_S32 forceIdr;
	CVI_S32 chgNum;
	CVI_S32 chgBitrate;
	CVI_S32 chgFramerate;
	CVI_S32 tempLayer;
	CVI_S32 testRoi;
	CVI_S32 bgInterval;
	CVI_S32 frameLost;
	CVI_U32 frameLostGap;
	CVI_U32 frameLostBspThr;
	CVI_S32 MCUPerECS;
	CVI_S32 bCreateChn;
	CVI_S32 getstream_timeout;
	CVI_S32 sendframe_timeout;
	CVI_S32 s32IPQpDelta;
	CVI_S32 s32BgQpDelta;
	CVI_S32 s32ViQpDelta;
	CVI_S32 bVariFpsEn;
	CVI_S32 initialDelay;
	CVI_U32 u32IntraCost;
	CVI_U32 u32ThrdLv;
	CVI_BOOL bBgEnhanceEn;
	CVI_S32 s32BgDeltaQp;
	CVI_U32 h264EntropyMode;
	CVI_S32 h264ChromaQpOffset;
	CVI_S32 h265CbQpOffset;
	CVI_S32 h265CrQpOffset;
	CVI_U32 enSuperFrmMode;
	CVI_U32 u32SuperIFrmBitsThr;
	CVI_U32 u32SuperPFrmBitsThr;
	CVI_S32 s32MaxReEncodeTimes;

	CVI_U8 aspectRatioInfoPresentFlag;
	CVI_U8 aspectRatioIdc;
	CVI_U8 overscanInfoPresentFlag;
	CVI_U8 overscanAppropriateFlag;
	CVI_U16 sarWidth;
	CVI_U16 sarHeight;

	CVI_U8 timingInfoPresentFlag;
	CVI_U8 fixedFrameRateFlag;
	CVI_U32 numUnitsInTick;
	CVI_U32 timeScale;

	CVI_U8 videoSignalTypePresentFlag;
	CVI_U8 videoFormat;
	CVI_U8 videoFullRangeFlag;
	CVI_U8 colourDescriptionPresentFlag;
	CVI_U8 colourPrimaries;
	CVI_U8 transferCharacteristics;
	CVI_U8 matrixCoefficients;

	CVI_U32 u32FrameQp;
	CVI_BOOL bTestUbrEn;

	CVI_BOOL bEsBufQueueEn;
	CVI_BOOL bIsoSendFrmEn;
	CVI_BOOL bSensorEn;

	CVI_U32 u32SliceCnt;
	CVI_U8 bDisableDeblk;
	CVI_S32 betaOffset;
	CVI_S32 alphaOffset;
	CVI_BOOL bIntraPred;
	FILE *roiFile;

	CVI_BOOL svc_enable;
	CVI_BOOL fg_protect_en;
	CVI_S32 fg_dealt_qp;
	CVI_BOOL complex_scene_detect_en;
	CVI_U32 complex_scene_low_th;
	CVI_U32 complex_scene_hight_th;
	CVI_U32 middle_min_percent;
	CVI_U32 complex_min_percent;
	CVI_BOOL smart_ai_en;

	CVI_U32 u32ResetGop;
} chnInputCfg;

/*******************************************************
 *  enum define
 *******************************************************/
typedef enum _PIC_SIZE_E
{
	PIC_CIF,
	PIC_D1_PAL,	 /* 720 * 576 */
	PIC_D1_NTSC, /* 720 * 480 */
	PIC_720P,	 /* 1280 * 720  */
	PIC_1600x1200,
	PIC_1080P, /* 1920 * 1080 */
	PIC_1088,  /* 1920 * 1088 */
	PIC_1440P, /* 2560 * 1440 */
	PIC_2304x1296,
	PIC_2048x1536,
	PIC_2560x1600,
	PIC_2560x1944,
	PIC_2592x1520,
	PIC_2592x1536,
	PIC_2592x1944,
	PIC_1024X1280,
	PIC_2688x1520,
	PIC_2716x1524,
	PIC_2880x1618,
	PIC_2880x1620,
	PIC_3844x1124,
	PIC_3840x2160,
	PIC_4096x2160,
	PIC_3000x3000,
	PIC_4000x3000,
	PIC_3840x8640,
	PIC_7688x1124,
	PIC_640x480,
	PIC_479P, /* 632 * 479 */
	PIC_400x400,
	PIC_288P, /* 384 * 288 */
	PIC_CUSTOMIZE,
	PIC_BUTT
} PIC_SIZE_E;

typedef enum _SAMPLE_RC_E
{
	SAMPLE_RC_CBR = 0,
	SAMPLE_RC_VBR,
	SAMPLE_RC_AVBR,
	SAMPLE_RC_QVBR,
	SAMPLE_RC_FIXQP,
	SAMPLE_RC_QPMAP,
	SAMPLE_RC_UBR,
	SAMPLE_RC_MAX
} SAMPLE_RC_E;

typedef struct _VDEC_PARAM_S
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
		VDEC_VIDEO_ATTR stSampleVdecVideo;	   /* structure with video ( h265/h264) */
		VDEC_PICTURE_ATTR stSampleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
	};
} VDEC_ATTR;

typedef struct _VENC_THREAD_PARAM_S
{
	VENC_CHN VencChn;
	char filename[256];
	CVI_BOOL stop_thread;
	pthread_t venc_thread;
} VENC_THREAD_PARAM_S;

CVI_VOID COMM_VENC_InitChnInputCfg(chnInputCfg *pIc)
{
	if (!pIc)
	{
		CVI_VENC_ERR("pIc = NULL\n");
		return;
	}

	memset(pIc, 0, sizeof(chnInputCfg));
	pIc->u32Profile = CVI_H264_PROFILE_DEFAULT;
	pIc->rcMode = -1;
	pIc->iqp = -1;
	pIc->pqp = -1;
	pIc->gop = CVI_H26X_GOP_DEFAULT;
	pIc->gopMode = CVI_H26X_GOP_MODE_DEFAULT;
	pIc->bitrate = -1;
	pIc->firstFrmstartQp = -1;
	pIc->num_frames = -1;
	pIc->framerate = 30;
	pIc->bVariFpsEn = 0;
	pIc->maxIprop = CVI_H26X_MAX_I_PROP_DEFAULT;
	pIc->minIprop = CVI_H26X_MIN_I_PROP_DEFAULT;
	pIc->maxQp = -1;
	pIc->minQp = -1;
	pIc->maxIqp = -1;
	pIc->minIqp = -1;
	pIc->quality = -1;
	pIc->maxbitrate = -1;
	pIc->statTime = -1;
	pIc->bind_mode = VENC_BIND_VPSS;
	pIc->pixel_format = 0;
	pIc->bitstreamBufSize = 0;
	pIc->single_LumaBuf = 0;
	pIc->single_core = 0;
	pIc->forceIdr = -1;
	pIc->u32ResetGop = 0;
	pIc->chgNum = -1;
	pIc->tempLayer = 0;
	pIc->bgInterval = CVI_H26X_SMARTP_BG_INTERVAL_DEFAULT;
	pIc->frameLost = -1;
	pIc->frameLostBspThr = -1;
	pIc->frameLostGap = -1;
	pIc->MCUPerECS = 0;
	pIc->sendframe_timeout = 20000;
	pIc->getstream_timeout = -1;
	pIc->s32IPQpDelta = CVI_H26X_NORMALP_IP_QP_DELTA_DEFAULT;
	pIc->s32BgQpDelta = CVI_H26X_SMARTP_BG_QP_DELTA_DEFAULT;
	pIc->s32ViQpDelta = CVI_H26X_SMARTP_VI_QP_DELTA_DEFAULT;
	pIc->initialDelay = CVI_INITIAL_DELAY_DEFAULT;
	pIc->h264EntropyMode = H264E_ENTROPY_CABAC;
	pIc->h264ChromaQpOffset = 0;
	pIc->h265CbQpOffset = 0;
	pIc->h265CrQpOffset = 0;
	pIc->u32RowQpDelta = CVI_H26X_ROW_QP_DELTA_DEFAULT;
	pIc->enSuperFrmMode = CVI_H26X_SUPER_FRM_MODE_DEFAULT;
	pIc->u32SuperIFrmBitsThr = CVI_H26X_SUPER_I_BITS_THR_DEFAULT;
	pIc->u32SuperPFrmBitsThr = CVI_H26X_SUPER_P_BITS_THR_DEFAULT;
	pIc->s32MaxReEncodeTimes = CVI_H26X_MAX_RE_ENCODE_DEFAULT;

	pIc->aspectRatioInfoPresentFlag = CVI_H26X_ASPECT_RATIO_INFO_PRESENT_FLAG_DEFAULT;
	pIc->aspectRatioIdc = CVI_H26X_ASPECT_RATIO_IDC_DEFAULT;
	pIc->overscanInfoPresentFlag = CVI_H26X_OVERSCAN_INFO_PRESENT_FLAG_DEFAULT;
	pIc->overscanAppropriateFlag = CVI_H26X_OVERSCAN_APPROPRIATE_FLAG_DEFAULT;
	pIc->sarWidth = CVI_H26X_SAR_WIDTH_DEFAULT;
	pIc->sarHeight = CVI_H26X_SAR_HEIGHT_DEFAULT;

	pIc->timingInfoPresentFlag = CVI_H26X_TIMING_INFO_PRESENT_FLAG_DEFAULT;
	pIc->fixedFrameRateFlag = CVI_H264_FIXED_FRAME_RATE_FLAG_DEFAULT;
	pIc->numUnitsInTick = CVI_H26X_NUM_UNITS_IN_TICK_DEFAULT;
	pIc->timeScale = CVI_H26X_TIME_SCALE_DEFAULT;

	pIc->videoSignalTypePresentFlag = CVI_H26X_VIDEO_SIGNAL_TYPE_PRESENT_FLAG_DEFAULT;
	pIc->videoFormat = CVI_H26X_VIDEO_FORMAT_DEFAULT;
	pIc->videoFullRangeFlag = CVI_H26X_VIDEO_FULL_RANGE_FLAG_DEFAULT;
	pIc->colourDescriptionPresentFlag = CVI_H26X_COLOUR_DESCRIPTION_PRESENT_FLAG_DEFAULT;
	pIc->colourPrimaries = CVI_H26X_COLOUR_PRIMARIES_DEFAULT;
	pIc->transferCharacteristics = CVI_H26X_TRANSFER_CHARACTERISTICS_DEFAULT;
	pIc->matrixCoefficients = CVI_H26X_MATRIX_COEFFICIENTS_DEFAULT;

	pIc->u32FrameQp = CVI_H26X_FRAME_QP_DEFAULT;
	pIc->bTestUbrEn = CVI_H26X_TEST_UBR_EN_DEFAULT;
	pIc->bEsBufQueueEn = CVI_H26X_ES_BUFFER_QUEUE_DEFAULT;
	pIc->bIsoSendFrmEn = CVI_H26X_ISO_SEND_FRAME_DEFAUL;
	pIc->bSensorEn = CVI_H26X_SENSOR_EN_DEFAULT;

	pIc->u32SliceCnt = 1;
	pIc->bIntraPred = 0;
}



CVI_S32 COMM_SYS_Init(VB_CONFIG_S *pstVbConfig)
{
	CVI_S32 s32Ret = CVI_FAILURE;

	CVI_SYS_Exit();
	CVI_VB_Exit();

	if (pstVbConfig == NULL)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "input parameter is null, it is invaild!\n");
		return CVI_FAILURE;
	}

	s32Ret = CVI_VB_SetConfig(pstVbConfig);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VB_SetConf failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_VB_Init();
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VB_Init failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_SYS_Init();
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_Init failed!\n");
		CVI_VB_Exit();
		return s32Ret;
	}

	return CVI_SUCCESS;
}

/******************************************************************************
 * function : vb exit & MMF system exit
 ******************************************************************************/
CVI_VOID COMM_SYS_Exit(void)
{
	// CVI_BOOL abChnEnable[VPSS_MAX_CHN_NUM] = {CVI_TRUE, };

	// for (VPSS_GRP VpssGrp = 0; VpssGrp < VPSS_MAX_GRP_NUM; ++VpssGrp)
	// SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	// SAMPLE_COMM_VO_Exit();
	CVI_SYS_Exit();
	CVI_VB_Exit();
}

CVI_S32 COMM_VPSS_Init(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
					   VPSS_CHN_ATTR_S *pastVpssChnAttr)
{
	VPSS_CHN VpssChn;
	CVI_S32 s32Ret;
	CVI_S32 j;

	s32Ret = CVI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VPSS_ResetGrp(VpssGrp);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VPSS_ResetGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
		return CVI_FAILURE;
	}

	for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
	{
		if (pabChnEnable[j])
		{
			VpssChn = j;
			s32Ret = CVI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastVpssChnAttr[VpssChn]);

			if (s32Ret != CVI_SUCCESS)
			{
				printf("CVI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
				return CVI_FAILURE;
			}

			s32Ret = CVI_VPSS_EnableChn(VpssGrp, VpssChn);

			if (s32Ret != CVI_SUCCESS)
			{
				printf("CVI_VPSS_EnableChn failed with %#x\n", s32Ret);
				return CVI_FAILURE;
			}
		}
	}

	return CVI_SUCCESS;
}

/*****************************************************************************
 * function : start vpss grp.
 *****************************************************************************/
CVI_S32 COMM_VPSS_Start(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
						VPSS_CHN_ATTR_S *pastVpssChnAttr)
{
	CVI_S32 s32Ret;
	UNUSED(pabChnEnable);
	UNUSED(pstVpssGrpAttr);
	UNUSED(pastVpssChnAttr);

	s32Ret = CVI_VPSS_StartGrp(VpssGrp);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VPSS_StartGrp failed with %#x\n", s32Ret);
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

/* COMM_VPSS_Stop: stop vpss grp
 *
 * VpssGrp: the VPSS Grp to control
 * pabChnEnable: array of VPSS CHN, stop if true.
 */
CVI_S32 COMM_VPSS_Stop(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable)
{
	CVI_S32 j;
	CVI_S32 s32Ret = CVI_SUCCESS;
	VPSS_CHN VpssChn;

	for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
	{
		if (pabChnEnable[j])
		{
			VpssChn = j;
			s32Ret = CVI_VPSS_DisableChn(VpssGrp, VpssChn);
			if (s32Ret != CVI_SUCCESS)
			{
				printf("Vpss stop Grp %d channel %d failed! Please check param\n",
					   VpssGrp, VpssChn);
				return CVI_FAILURE;
			}
		}
	}

	s32Ret = CVI_VPSS_StopGrp(VpssGrp);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("Vpss Stop Grp %d failed! Please check param\n", VpssGrp);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VPSS_DestroyGrp(VpssGrp);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("Vpss Destroy Grp %d failed! Please check\n", VpssGrp);
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VPSS-VENC)");

	return CVI_SUCCESS;
}

CVI_S32 COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VPSS-VENC)");

	return CVI_SUCCESS;
}

CVI_S32 COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VDEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = VdecChn;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VDEC-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VDEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = VdecChn;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VDEC-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 COMM_VENC_Stop(VENC_CHN VencChn)
{
	CVI_S32 s32Ret;
	// stop transmission of venc frame data reception
	s32Ret = CVI_VENC_StopRecvFrame(VencChn);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("CVI_VENC_StopRecvPic vechn[%d] failed with %#x!\n",
					 VencChn, s32Ret);
		return CVI_FAILURE;
	}
	// // stop thread execution
	// if (gs_VencTask[VencChn] != 0)
	// {
	// 	pthread_join(gs_VencTask[VencChn], CVI_NULL);
	// 	CVI_VENC_SYNC("GetVencStreamProc done\n");
	// 	gs_VencTask[VencChn] = 0;
	// }
	// destroy the venc channel
	s32Ret = CVI_VENC_DestroyChn(VencChn);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("CVI_VENC_DestroyChn vechn[%d] failed with %#x!\n",
					 VencChn, s32Ret);
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 COMM_VENC_SetChnParam(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_CHN_PARAM_S stChnParam, *pstChnParam = &stChnParam;
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VENC_GetChnParam(VencChn, pstChnParam);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("CVI_VENC_GetJpegParam\n");
		return CVI_FAILURE;
	}

	pstChnParam->stCropCfg.bEnable = (pIc->posX || pIc->posY);
	pstChnParam->stCropCfg.stRect.s32X = pIc->posX;
	pstChnParam->stCropCfg.stRect.s32Y = pIc->posY;
	pstChnParam->stCropCfg.stRect.u32Width = pIc->width;
	pstChnParam->stCropCfg.stRect.u32Height = pIc->height;
	CVI_VENC_TRACE("s32X = %d, s32Y = %d\n",
				   pstChnParam->stCropCfg.stRect.s32X,
				   pstChnParam->stCropCfg.stRect.s32Y);

	pstChnParam->stFrameRate.s32SrcFrmRate = pIc->srcFramerate;
	pstChnParam->stFrameRate.s32DstFrmRate = pIc->framerate;

	s32Ret = CVI_VENC_SetChnParam(VencChn, pstChnParam);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("CVI_VENC_SetChnParam fail\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 COMM_VENC_SetChnAttr(
	chnInputCfg *pIc,
	VENC_CHN_ATTR_S *pstVencChnAttr,
	PAYLOAD_TYPE_E enType,
	PIC_SIZE_E enSize,
	SAMPLE_RC_E enRcMode,
	CVI_U32 u32Profile,
	VENC_GOP_ATTR_S *pstGopAttr,
	CVI_BOOL bRcnRefShareBuf)
{
	SIZE_S stPicSize;
	CVI_U32 u32StatTime = 0;
	CVI_U32 u32Gop = 30;
	CVI_U32 u32FrameRate = pIc->framerate;
	CVI_U32 u32SrcFrameRate = pIc->srcFramerate;
	CVI_S32 s32Ret = CVI_SUCCESS;

	stPicSize.u32Width = pIc->width;
	stPicSize.u32Height = pIc->height;
	// else
	// {
	//  	s32Ret = COMM_SYS_GetPicSize(enSize, &stPicSize);
	// }

	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("Get picture size failed!\n");
		return CVI_FAILURE;
	}

	memset(pstVencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

	pstVencChnAttr->stVencAttr.enType = enType;
	pstVencChnAttr->stVencAttr.u32MaxPicWidth = stPicSize.u32Width;
	pstVencChnAttr->stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
	pstVencChnAttr->stVencAttr.u32PicWidth = stPicSize.u32Width;
	pstVencChnAttr->stVencAttr.u32PicHeight = stPicSize.u32Height;
	pstVencChnAttr->stVencAttr.u32BufSize = pIc->bitstreamBufSize;
	pstVencChnAttr->stVencAttr.bEsBufQueueEn = pIc->bEsBufQueueEn;
	pstVencChnAttr->stVencAttr.bIsoSendFrmEn = pIc->bIsoSendFrmEn;
	pstVencChnAttr->stVencAttr.u32Profile = u32Profile;
	pstVencChnAttr->stVencAttr.bByFrame = CVI_TRUE; // get stream mode is slice mode or
													// frame mode ?

	if (pstGopAttr->enGopMode == VENC_GOPMODE_NORMALP)
	{
		pstGopAttr->stNormalP.s32IPQpDelta = pIc->s32IPQpDelta;
		CVI_VENC_CFG("s32IPQpDelta = %d\n", pstGopAttr->stNormalP.s32IPQpDelta);
		u32StatTime = pIc->statTime;
	}
	else if (pstGopAttr->enGopMode == VENC_GOPMODE_SMARTP)
	{
		pstGopAttr->stSmartP.u32BgInterval = pIc->bgInterval;
		u32StatTime = pstGopAttr->stSmartP.u32BgInterval / u32Gop;

		pstGopAttr->stSmartP.s32BgQpDelta = pIc->s32BgQpDelta;
		pstGopAttr->stSmartP.s32ViQpDelta = pIc->s32ViQpDelta;
		CVI_VENC_CFG("s32BgQpDelta = %d\n", pstGopAttr->stSmartP.s32BgQpDelta);
		CVI_VENC_CFG("s32ViQpDelta = %d\n", pstGopAttr->stSmartP.s32ViQpDelta);
	}
	else
	{
		u32StatTime = pIc->statTime;
	}

	switch (enType)
	{
	case PT_H265:
	{
		if (enRcMode == SAMPLE_RC_CBR)
		{
			VENC_H265_CBR_S *pstH265Cbr = &pstVencChnAttr->stRcAttr.stH265Cbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
			pstH265Cbr->u32Gop = pIc->gop;
			pstH265Cbr->u32StatTime = u32StatTime;
			pstH265Cbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265Cbr->fr32DstFrameRate = u32FrameRate;
			pstH265Cbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265Cbr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("u32BitRate = %d\n", pstH265Cbr->u32BitRate);
		}
		else if (enRcMode == SAMPLE_RC_FIXQP)
		{
			VENC_H265_FIXQP_S *pstH265FixQp = &pstVencChnAttr->stRcAttr.stH265FixQp;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
			pstH265FixQp->u32Gop = pIc->gop;
			pstH265FixQp->u32SrcFrameRate = u32SrcFrameRate;
			pstH265FixQp->fr32DstFrameRate = u32FrameRate;
			pstH265FixQp->bVariFpsEn = pIc->bVariFpsEn;
			pstH265FixQp->u32IQp = pIc->iqp;
			pstH265FixQp->u32PQp = pIc->pqp;
			CVI_VENC_CFG("u32Gop = %d, u32IQp = %d, u32PQp = %d\n",
						 pstH265FixQp->u32Gop,
						 pstH265FixQp->u32IQp,
						 pstH265FixQp->u32PQp);
		}
		else if (enRcMode == SAMPLE_RC_VBR)
		{
			VENC_H265_VBR_S *pstH265Vbr = &pstVencChnAttr->stRcAttr.stH265Vbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
			pstH265Vbr->u32Gop = pIc->gop;
			pstH265Vbr->u32StatTime = u32StatTime;
			pstH265Vbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265Vbr->fr32DstFrameRate = u32FrameRate;
			pstH265Vbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265Vbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
						 pstH265Vbr->u32StatTime,
						 pstH265Vbr->u32Gop,
						 pstH265Vbr->u32MaxBitRate);
		}
		else if (enRcMode == SAMPLE_RC_AVBR)
		{
			VENC_H265_AVBR_S *pstH265AVbr = &pstVencChnAttr->stRcAttr.stH265AVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
			pstH265AVbr->u32Gop = pIc->gop;
			pstH265AVbr->u32StatTime = u32StatTime;
			pstH265AVbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265AVbr->fr32DstFrameRate = u32FrameRate;
			pstH265AVbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265AVbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
						 pstH265AVbr->u32StatTime,
						 pstH265AVbr->u32Gop,
						 pstH265AVbr->u32MaxBitRate);
		}
		else if (enRcMode == SAMPLE_RC_QVBR)
		{
			VENC_H265_QVBR_S stH265QVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
			stH265QVbr.u32Gop = u32Gop;
			stH265QVbr.u32StatTime = u32StatTime;
			stH265QVbr.u32SrcFrameRate = u32SrcFrameRate;
			stH265QVbr.fr32DstFrameRate = u32FrameRate;

			switch (enSize)
			{
			case PIC_720P:
				stH265QVbr.u32TargetBitRate = 1024 * 2 + 1024 * u32FrameRate / 30;
				break;
			case PIC_1080P:
				stH265QVbr.u32TargetBitRate = 1024 * 2 + 2048 * u32FrameRate / 30;
				break;
			case PIC_2592x1944:
				stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072 * u32FrameRate / 30;
				break;
			case PIC_3840x2160:
				stH265QVbr.u32TargetBitRate = 1024 * 5 + 5120 * u32FrameRate / 30;
				break;
			case PIC_4000x3000:
				stH265QVbr.u32TargetBitRate = 1024 * 10 + 5120 * u32FrameRate / 30;
				break;
			default:
				stH265QVbr.u32TargetBitRate = 1024 * 15 + 2048 * u32FrameRate / 30;
				break;
			}
			memcpy(&pstVencChnAttr->stRcAttr.stH265QVbr, &stH265QVbr, sizeof(VENC_H265_QVBR_S));
		}
		else if (enRcMode == SAMPLE_RC_QPMAP)
		{
			VENC_H265_QPMAP_S *pstH265QpMap = &pstVencChnAttr->stRcAttr.stH265QpMap;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
			pstH265QpMap->u32Gop = pIc->gop;
			pstH265QpMap->u32StatTime = u32StatTime;
			pstH265QpMap->u32SrcFrameRate = u32SrcFrameRate;
			pstH265QpMap->fr32DstFrameRate = u32FrameRate;
			pstH265QpMap->bVariFpsEn = pIc->bVariFpsEn;
			pstH265QpMap->enQpMapMode = VENC_RC_QPMAP_MODE_MEANQP;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d\n",
						 pstH265QpMap->u32StatTime,
						 pstH265QpMap->u32Gop);
		}
		else if (enRcMode == SAMPLE_RC_UBR)
		{
			VENC_H265_UBR_S *pstH265Ubr = &pstVencChnAttr->stRcAttr.stH265Ubr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265UBR;
			pstH265Ubr->u32Gop = pIc->gop;
			pstH265Ubr->u32StatTime = u32StatTime;
			pstH265Ubr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265Ubr->fr32DstFrameRate = u32FrameRate;
			pstH265Ubr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265Ubr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("u32BitRate = %d\n", pstH265Ubr->u32BitRate);
		}
		else
		{
			CVI_VENC_ERR("enRcMode(%d) not support\n", enRcMode);
			return CVI_FAILURE;
		}
		pstVencChnAttr->stVencAttr.stAttrH265e.bRcnRefShareBuf = bRcnRefShareBuf;
	}
	break;
	case PT_H264:
	{
		if (enRcMode == SAMPLE_RC_CBR)
		{
			VENC_H264_CBR_S *pstH264Cbr = &pstVencChnAttr->stRcAttr.stH264Cbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
			pstH264Cbr->u32Gop = pIc->gop;
			pstH264Cbr->u32StatTime = u32StatTime;
			pstH264Cbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264Cbr->fr32DstFrameRate = u32FrameRate;
			pstH264Cbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264Cbr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("bitrate = %d, u32BitRate = %d\n",
						 pIc->bitrate,
						 pstH264Cbr->u32BitRate);
		}
		else if (enRcMode == SAMPLE_RC_FIXQP)
		{
			VENC_H264_FIXQP_S *pstH264FixQp = &pstVencChnAttr->stRcAttr.stH264FixQp;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
			pstH264FixQp->u32Gop = pIc->gop;
			pstH264FixQp->u32SrcFrameRate = u32SrcFrameRate;
			pstH264FixQp->fr32DstFrameRate = u32FrameRate;
			pstH264FixQp->bVariFpsEn = pIc->bVariFpsEn;
			pstH264FixQp->u32IQp = pIc->iqp;
			pstH264FixQp->u32PQp = pIc->pqp;
			CVI_VENC_CFG("u32Gop = %d, u32IQp = %d, u32PQp = %d\n",
						 pstH264FixQp->u32Gop,
						 pstH264FixQp->u32IQp,
						 pstH264FixQp->u32PQp);
		}
		else if (enRcMode == SAMPLE_RC_VBR)
		{
			VENC_H264_VBR_S *pstH264Vbr = &pstVencChnAttr->stRcAttr.stH264Vbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
			pstH264Vbr->u32Gop = pIc->gop;
			pstH264Vbr->u32StatTime = u32StatTime;
			pstH264Vbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264Vbr->fr32DstFrameRate = u32FrameRate;
			pstH264Vbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264Vbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
						 pstH264Vbr->u32StatTime,
						 pstH264Vbr->u32Gop,
						 pstH264Vbr->u32MaxBitRate);
		}
		else if (enRcMode == SAMPLE_RC_AVBR)
		{
			VENC_H264_AVBR_S *pstH264AVbr = &pstVencChnAttr->stRcAttr.stH264AVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
			pstH264AVbr->u32Gop = pIc->gop;
			pstH264AVbr->u32StatTime = u32StatTime;
			pstH264AVbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264AVbr->fr32DstFrameRate = u32FrameRate;
			pstH264AVbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264AVbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
						 pstH264AVbr->u32StatTime,
						 pstH264AVbr->u32Gop,
						 pstH264AVbr->u32MaxBitRate);
		}
		else if (enRcMode == SAMPLE_RC_QVBR)
		{
			VENC_H264_QVBR_S stH264QVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
			stH264QVbr.u32Gop = u32Gop;
			stH264QVbr.u32StatTime = u32StatTime;
			stH264QVbr.u32SrcFrameRate = u32SrcFrameRate;
			stH264QVbr.fr32DstFrameRate = u32FrameRate;
			switch (enSize)
			{
			case PIC_720P:
				stH264QVbr.u32TargetBitRate = 1024 * 2 + 1024 * u32FrameRate / 30;
				break;
			case PIC_1080P:
				stH264QVbr.u32TargetBitRate = 1024 * 2 + 2048 * u32FrameRate / 30;
				break;
			case PIC_2592x1944:
				stH264QVbr.u32TargetBitRate = 1024 * 3 + 3072 * u32FrameRate / 30;
				break;
			case PIC_3840x2160:
				stH264QVbr.u32TargetBitRate = 1024 * 5 + 5120 * u32FrameRate / 30;
				break;
			case PIC_4000x3000:
				stH264QVbr.u32TargetBitRate = 1024 * 10 + 5120 * u32FrameRate / 30;
				break;
			default:
				stH264QVbr.u32TargetBitRate = 1024 * 15 + 2048 * u32FrameRate / 30;
				break;
			}
			memcpy(&pstVencChnAttr->stRcAttr.stH264QVbr, &stH264QVbr, sizeof(VENC_H264_QVBR_S));
		}
		else if (enRcMode == SAMPLE_RC_UBR)
		{
			VENC_H264_UBR_S *pstH264Ubr = &pstVencChnAttr->stRcAttr.stH264Ubr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264UBR;
			pstH264Ubr->u32Gop = pIc->gop;
			pstH264Ubr->u32StatTime = u32StatTime;
			pstH264Ubr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264Ubr->fr32DstFrameRate = u32FrameRate;
			pstH264Ubr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264Ubr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("u32BitRate = %d\n", pstH264Ubr->u32BitRate);
		}
		else
		{
			CVI_VENC_ERR("H.264 enRcMode(%d) not support\n", enRcMode);
			return CVI_FAILURE;
		}
		pstVencChnAttr->stVencAttr.stAttrH264e.bRcnRefShareBuf = bRcnRefShareBuf;
		pstVencChnAttr->stVencAttr.stAttrH264e.bSingleLumaBuf = pIc->single_LumaBuf;
	}
	break;
	case PT_MJPEG:
	{
		if (enRcMode == SAMPLE_RC_FIXQP)
		{
			VENC_MJPEG_FIXQP_S *pstMjpegeFixQp = &pstVencChnAttr->stRcAttr.stMjpegFixQp;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;

			// 0 use old q-table for forward compatible.
			pstMjpegeFixQp->u32Qfactor = (pIc->quality > 0) ? pIc->quality : 0;
			pstMjpegeFixQp->u32SrcFrameRate = u32SrcFrameRate;
			pstMjpegeFixQp->fr32DstFrameRate = u32FrameRate;
			pstMjpegeFixQp->bVariFpsEn = pIc->bVariFpsEn;
		}
		else if (enRcMode == SAMPLE_RC_CBR)
		{
			VENC_MJPEG_CBR_S *pstMjpegeCbr = &pstVencChnAttr->stRcAttr.stMjpegCbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
			pstMjpegeCbr->u32StatTime = u32StatTime;
			pstMjpegeCbr->u32SrcFrameRate = u32SrcFrameRate;
			pstMjpegeCbr->fr32DstFrameRate = u32FrameRate;
			pstMjpegeCbr->bVariFpsEn = pIc->bVariFpsEn;
			pstMjpegeCbr->u32BitRate = pIc->bitrate;
		}
		else if ((enRcMode == SAMPLE_RC_VBR) || (enRcMode == SAMPLE_RC_AVBR) ||
				 (enRcMode == SAMPLE_RC_QVBR))
		{
			VENC_MJPEG_VBR_S stMjpegVbr;

			if (enRcMode == SAMPLE_RC_AVBR)
				CVI_VENC_INFO("Mjpege not support AVBR, so change rcmode to VBR!\n");

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
			stMjpegVbr.u32StatTime = u32StatTime;
			stMjpegVbr.u32SrcFrameRate = u32SrcFrameRate;
			stMjpegVbr.fr32DstFrameRate = 5;

			switch (enSize)
			{
			case PIC_720P:
				stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024 * u32FrameRate / 30;
				break;
			case PIC_1080P:
				stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048 * u32FrameRate / 30;
				break;
			case PIC_2592x1944:
				stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072 * u32FrameRate / 30;
				break;
			case PIC_3840x2160:
				stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120 * u32FrameRate / 30;
				break;
			case PIC_4000x3000:
				stMjpegVbr.u32MaxBitRate = 1024 * 30 + 5120 * u32FrameRate / 30;
				break;
			default:
				stMjpegVbr.u32MaxBitRate = 1024 * 20 + 2048 * u32FrameRate / 30;
				break;
			}

			memcpy(&pstVencChnAttr->stRcAttr.stMjpegVbr, &stMjpegVbr, sizeof(VENC_MJPEG_VBR_S));
		}
		else
		{
			CVI_VENC_ERR("cann't support other mode(%d) in this version!\n", enRcMode);
			return CVI_FAILURE;
		}
	}
	break;

	case PT_JPEG:
	{
		VENC_ATTR_JPEG_S *pstJpegAttr = &pstVencChnAttr->stVencAttr.stAttrJpege;

		pstJpegAttr->bSupportDCF = CVI_FALSE;
		pstJpegAttr->stMPFCfg.u8LargeThumbNailNum = 0;
		pstJpegAttr->enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
	}
	break;

	default:
		CVI_VENC_ERR("cann't support this enType (%d) in this version!\n", enType);
		return CVI_ERR_VENC_NOT_SUPPORT;
	}

	if (PT_MJPEG == enType || PT_JPEG == enType)
	{
		pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
		pstVencChnAttr->stGopAttr.stNormalP.s32IPQpDelta = 0;
	}
	else
	{
		memcpy(&pstVencChnAttr->stGopAttr, pstGopAttr, sizeof(VENC_GOP_ATTR_S));

		if ((pstGopAttr->enGopMode == VENC_GOPMODE_BIPREDB) && (enType == PT_H264))
		{
			if (pstVencChnAttr->stVencAttr.u32Profile == 0)
			{
				pstVencChnAttr->stVencAttr.u32Profile = 1;

				CVI_VENC_INFO("H.264 base not support BIPREDB, change to main\n");
			}
		}

		if ((pstVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H264QPMAP) ||
			(pstVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H265QPMAP))
		{
			if (pstGopAttr->enGopMode == VENC_GOPMODE_ADVSMARTP)
			{
				pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

				CVI_VENC_INFO("advsmartp not support QPMAP, so change gopmode to smartp!\n");
			}
		}
	}

	return s32Ret;
}

CVI_S32 COMM_VENC_Create(
	chnInputCfg *pIc,
	VENC_CHN VencChn,
	PAYLOAD_TYPE_E enType,
	PIC_SIZE_E enSize,
	SAMPLE_RC_E enRcMode,
	CVI_U32 u32Profile,
	CVI_BOOL bRcnRefShareBuf,
	VENC_GOP_ATTR_S *pstGopAttr)
{
	CVI_S32 s32Ret;
	VENC_CHN_ATTR_S stVencChnAttr, *pstVencChnAttr = &stVencChnAttr;

	s32Ret = COMM_VENC_SetChnAttr(
		pIc,
		pstVencChnAttr,
		enType,
		enSize,
		enRcMode,
		u32Profile,
		pstGopAttr,
		bRcnRefShareBuf);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("Get picture size failed!\n");
		return CVI_FAILURE;
	}

	if (pIc->bCreateChn == CVI_FALSE)
	{
		s32Ret = CVI_VENC_CreateChn(VencChn, pstVencChnAttr);
		if (s32Ret != CVI_SUCCESS)
		{
			CVI_VENC_ERR("CVI_VENC_CreateChn [%d] failed with %d\n", VencChn, s32Ret);
			return s32Ret;
		}
		pIc->bCreateChn = CVI_TRUE;
	}

	// if (enType == PT_H264)
	// {
	// 	VENC_H264_ENTROPY_S h264Entropy = {0};

	// 	switch (pIc->h264EntropyMode)
	// 	{
	// 	case 0:
	// 		h264Entropy.u32EntropyEncModeI = H264E_ENTROPY_CAVLC;
	// 		h264Entropy.u32EntropyEncModeP = H264E_ENTROPY_CAVLC;
	// 		break;
	// 	case 1:
	// 		h264Entropy.u32EntropyEncModeI = H264E_ENTROPY_CABAC;
	// 		h264Entropy.u32EntropyEncModeP = H264E_ENTROPY_CABAC;
	// 		break;
	// 	default:
	// 		h264Entropy.u32EntropyEncModeI = H264E_ENTROPY_CABAC;
	// 		h264Entropy.u32EntropyEncModeP = H264E_ENTROPY_CABAC;
	// 		break;
	// 	}

	// s32Ret = CVI_VENC_SetH264Entropy(VencChn, &h264Entropy);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	CVI_VENC_ERR("CVI_VENC_SetH264Entropy, %d\n", s32Ret);
	// 	goto ERR_SAMPLE_COMM_VENC_CREATE;
	// }

	// s32Ret = SAMPLE_COMM_VENC_SetH264Trans(pIc, VencChn);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264Trans, %d\n", s32Ret);
	// 	goto ERR_SAMPLE_COMM_VENC_CREATE;
	// }

	// s32Ret = SAMPLE_COMM_VENC_SetH264Vui(pIc, VencChn);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264Vui, %d\n", s32Ret);
	// 	goto ERR_SAMPLE_COMM_VENC_CREATE;
	// }

	// s32Ret = SAMPLE_COMM_VENC_SetH264SliceSplit(pIc, VencChn);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264SliceSplit, %d\n", s32Ret);
	// 	goto ERR_SAMPLE_COMM_VENC_CREATE;
	// }

	// s32Ret = SAMPLE_COMM_VENC_SetH264Dblk(pIc, VencChn);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264Dblk, %d\n", s32Ret);
	// 	goto ERR_SAMPLE_COMM_VENC_CREATE;
	// }

	// s32Ret = SAMPLE_COMM_VENC_SetH264IntraPred(pIc, VencChn);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264IntraPred, %d\n", s32Ret);
	// 	goto ERR_SAMPLE_COMM_VENC_CREATE;
	// }
	// }

	s32Ret = COMM_VENC_SetChnParam(pIc, VencChn);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("COMM_VENC_SetChnParam, %d\n", s32Ret);
		CVI_VENC_DestroyChn(VencChn);
		goto ERR_SAMPLE_COMM_VENC_CREATE;
	}

	return s32Ret;
ERR_SAMPLE_COMM_VENC_CREATE:
	CVI_VENC_DestroyChn(VencChn);

	return s32Ret;
}

CVI_S32 COMM_VENC_Start(
	chnInputCfg *pIc,
	VENC_CHN VencChn,
	PAYLOAD_TYPE_E enType,
	PIC_SIZE_E enSize,
	SAMPLE_RC_E enRcMode,
	CVI_U32 u32Profile,
	CVI_BOOL bRcnRefShareBuf,
	VENC_GOP_ATTR_S *pstGopAttr)
{
	CVI_S32 s32Ret;
	VENC_RECV_PIC_PARAM_S stRecvParam;

	s32Ret = COMM_VENC_Create(
		pIc, VencChn, enType, enSize, enRcMode,
		u32Profile, bRcnRefShareBuf, pstGopAttr);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("SAMPLE_COMM_VENC_Create failed with %d\n", s32Ret);
		return CVI_FAILURE;
	}

	if (pIc->bind_mode == VENC_BIND_VPSS)
	{
		CVI_VENC_BIND("VPSS_Bind_VENC, vpss Grp = %d, Chn = %d, VencChn = %d\n",
					  pIc->vpssGrp, pIc->vpssChn, VencChn);
		COMM_VPSS_Bind_VENC(pIc->vpssGrp, pIc->vpssChn, VencChn);
	}

	stRecvParam.s32RecvPicNum = pIc->num_frames;
	s32Ret = CVI_VENC_StartRecvFrame(VencChn, &stRecvParam);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_VENC_ERR("CVI_VENC_StartRecvPic failed with %d\n", s32Ret);
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 start_vdec(VDEC_PARAM_S *pVdecParam)
{
	VDEC_CHN_PARAM_S stChnParam = {0};
	VDEC_MOD_PARAM_S stModParam;
	CVI_S32 s32Ret = CVI_SUCCESS;
	VDEC_CHN VdecChn = pVdecParam->VdecChn;

	printf("VdecChn = %d\n", VdecChn);

	CVI_VDEC_GetModParam(&stModParam);
	stModParam.enVdecVBSource = pVdecParam->vdec_vb_source;
	CVI_VDEC_SetModParam(&stModParam);

	s32Ret = CVI_VDEC_CreateChn(VdecChn, &pVdecParam->stChnAttr);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_CreateChn chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}

	if (pVdecParam->vdec_vb_source == VB_SOURCE_USER)
	{
		VDEC_CHN_POOL_S stPool;

		stPool.hPicVbPool = g_ahLocalPicVbPool[VdecChn];
		stPool.hTmvVbPool = VB_INVALID_POOLID;

		s32Ret = CVI_VDEC_AttachVbPool(VdecChn, &stPool);
		if (s32Ret != CVI_SUCCESS)
		{
			printf("CVI_VDEC_AttachVbPool chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
			return s32Ret;
		}
	}

	s32Ret = CVI_VDEC_GetChnParam(VdecChn, &stChnParam);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_GetChnParam chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}
	stChnParam.enPixelFormat = pVdecParam->vdec_pixel_format;
	stChnParam.u32DisplayFrameNum =
		(pVdecParam->stChnAttr.enType == PT_JPEG || pVdecParam->stChnAttr.enType == PT_MJPEG) ? 0 : 2;
	s32Ret = CVI_VDEC_SetChnParam(VdecChn, &stChnParam);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_SetChnParam chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VDEC_StartRecvStream(VdecChn);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_StartRecvStream chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_VOID stop_vdec(VDEC_PARAM_S *pVdecParam)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VDEC_StopRecvStream(pVdecParam->VdecChn);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_StopRecvStream chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
	}

	if (pVdecParam->vdec_vb_source == VB_SOURCE_USER)
	{
		CVI_VDEC_TRACE("detach in user mode\n");
		s32Ret = CVI_VDEC_DetachVbPool(pVdecParam->VdecChn);
		if (s32Ret != CVI_SUCCESS)
		{
			printf("CVI_VDEC_DetachVbPool chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		}
	}

	s32Ret = CVI_VDEC_ResetChn(pVdecParam->VdecChn);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_ResetChn chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
	}

	s32Ret = CVI_VDEC_DestroyChn(pVdecParam->VdecChn);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("CVI_VDEC_DestroyChn chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
	}
}

PAYLOAD_TYPE_E find_file_type(const char *filename, int filelen)
{
	if (strcmp(filename + filelen - 3, "265") == 0)
	{
		return PT_H265;
	}
	else if (strcmp(filename + filelen - 3, "264") == 0)
	{
		return PT_H264;
	}
	else if (strcmp(filename + filelen - 3, "jpg") == 0)
	{
		return PT_JPEG;
	}
	else if (strcmp(filename + filelen - 3, "mjp") == 0)
	{
		return PT_MJPEG;
	}
	else
	{
		return PT_BUTT;
	}
}

CVI_S32 vdec_init_vb_pool(VDEC_CHN ChnIndex, VDEC_ATTR *pastSampleVdec, CVI_BOOL is_user)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32BlkSize;
	VB_CONFIG_S stVbConf;

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize =
		VDEC_GetPicBufferSize(pastSampleVdec->enType, pastSampleVdec->u32Width,
							  pastSampleVdec->u32Height,
							  pastSampleVdec->enPixelFormat, DATA_BITWIDTH_8,
							  COMPRESS_MODE_NONE);
	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = pastSampleVdec->u32FrameBufCnt;
	stVbConf.astCommPool[0].enRemapMode = VB_REMAP_MODE_NONE;
	printf("VDec Init Pool[VdecChn%d], u32BlkSize = %d, u32BlkCnt = %d\n", ChnIndex,
		   stVbConf.astCommPool[0].u32BlkSize, stVbConf.astCommPool[0].u32BlkCnt);

	if (!is_user)
	{
		if (stVbConf.u32MaxPoolCnt == 0 && stVbConf.astCommPool[0].u32BlkSize == 0)
		{
			CVI_SYS_Exit();
			s32Ret = CVI_SYS_Init();
			if (s32Ret != CVI_SUCCESS)
			{
				CVI_VDEC_ERR("CVI_SYS_Init, %d\n", s32Ret);
				return CVI_FAILURE;
			}
		}
		else
		{
			s32Ret = COMM_SYS_Init(&stVbConf);
			if (s32Ret != CVI_SUCCESS)
			{
				CVI_VDEC_ERR("SAMPLE_COMM_SYS_Init, %d\n", s32Ret);
				return CVI_FAILURE;
			}
		}
	}
	else
	{
		for (CVI_U32 i = 0; i < stVbConf.u32MaxPoolCnt; i++)
		{
			g_ahLocalPicVbPool[ChnIndex] = CVI_VB_CreatePool(&stVbConf.astCommPool[0]);

			if (g_ahLocalPicVbPool[ChnIndex] == VB_INVALID_POOLID)
			{
				CVI_VDEC_ERR("CVI_VB_CreatePool Fail\n");
				return CVI_FAILURE;
			}
		}
		printf("CVI_VB_CreatePool : %d, u32BlkSize=0x%x, u32BlkCnt=%d\n",
			   g_ahLocalPicVbPool[ChnIndex], stVbConf.astCommPool[0].u32BlkSize, stVbConf.astCommPool[0].u32BlkCnt);
	}

	return s32Ret;
}

CVI_VOID vdec_exit_vb_pool(CVI_VOID)
{
	CVI_S32 i, s32Ret;
	VDEC_MOD_PARAM_S stModParam;
	VB_SOURCE_E vb_source;
	CVI_VDEC_GetModParam(&stModParam);
	vb_source = stModParam.enVdecVBSource;
	if (vb_source != VB_SOURCE_USER)
		return;

	for (i = MAX_VDEC_NUM - 1; i >= 0; i--)
	{
		if (g_ahLocalPicVbPool[i] != VB_INVALID_POOLID)
		{
			CVI_VDEC_TRACE("CVI_VB_DestroyPool : %d\n", g_ahLocalPicVbPool[i]);

			s32Ret = CVI_VB_DestroyPool(g_ahLocalPicVbPool[i]);
			if (s32Ret != CVI_SUCCESS)
			{
				CVI_VDEC_ERR("CVI_VB_DestroyPool : %d fail!\n", g_ahLocalPicVbPool[i]);
			}

			g_ahLocalPicVbPool[i] = VB_INVALID_POOLID;
		}
	}
}

static CVI_S32 COMM_VENC_GetDataType(PAYLOAD_TYPE_E enType, VENC_PACK_S *ppack)
{
	if (enType == PT_H264)
		return ppack->DataType.enH264EType;
	else if (enType == PT_H265)
		return ppack->DataType.enH265EType;
	else if (enType == PT_JPEG || enType == PT_MJPEG)
		return ppack->DataType.enJPEGEType;

	CVI_VENC_ERR("enType = %d\n", enType);
	return CVI_FAILURE;
}

CVI_S32 COMM_VENC_SaveStream(PAYLOAD_TYPE_E enType,
							 FILE *pFd, VENC_STREAM_S *pstStream)
{
	VENC_PACK_S *ppack;
	CVI_S32 dataType;

	if (!pFd)
	{
		CVI_VENC_ERR("pFd = NULL\n");
		return CVI_FAILURE;
	}

	CVI_VENC_TRACE("u32PackCount = %d\n", pstStream->u32PackCount);

	for (CVI_U32 i = 0; i < pstStream->u32PackCount; i++)
	{
		ppack = &pstStream->pstPack[i];
		fwrite(ppack->pu8Addr + ppack->u32Offset,
			   ppack->u32Len - ppack->u32Offset, 1, pFd);

		dataType = COMM_VENC_GetDataType(enType, ppack);
		if (dataType < 0)
		{
			CVI_VENC_ERR("dataType = %d\n", dataType);
			return CVI_FAILURE;
		}

		CVI_VENC_BS("pack[%d], PTS = %llu, DataType = %d\n",
					i, ppack->u64PTS, dataType);
		CVI_VENC_BS("Addr = %p, Len = 0x%X, Offset = 0x%X\n",
					ppack->pu8Addr, ppack->u32Len, ppack->u32Offset);
	}

	return CVI_SUCCESS;
}

CVI_VOID *thread_vdec_send_stream(CVI_VOID *arg)
{
	FILE *fpStrm = NULL;
	VDEC_PARAM_S *param = (VDEC_PARAM_S *)arg;
	CVI_BOOL bEndOfStream = CVI_FALSE;
	CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
	CVI_U8 *pu8Buf = NULL;
	VDEC_STREAM_S stStream;
	CVI_BOOL bFindStart, bFindEnd;
	CVI_U64 u64PTS = 0;
	CVI_U32 u32Start;
	CVI_S32 s32Ret, i;
	CVI_S32 bufsize = (param->stChnAttr.u32PicWidth * param->stChnAttr.u32PicHeight * 3) >> 1;
	char strBuf[64];

	// Set thread name for debugging
	snprintf(strBuf, sizeof(strBuf), "thread_vdec-%d", param->VdecChn);
	prctl(PR_SET_NAME, strBuf);

	// Open input file
	if (param->decode_file_name != 0)
	{
		fpStrm = fopen(param->decode_file_name, "rb");
		if (fpStrm == NULL)
		{
			printf("Failed to open file: %s\n", param->decode_file_name);
			return (CVI_VOID *)(CVI_FAILURE);
		}

		// Allocate read buffer
		pu8Buf = malloc(bufsize);
		if (pu8Buf == NULL)
		{
			printf("Failed to allocate buffer of size %d for channel %d\n",
				   bufsize, param->VdecChn);
			fclose(fpStrm);
			return (CVI_VOID *)(CVI_FAILURE);
		}
	}

	printf("VDEC send stream thread started for channel %d\n", param->VdecChn);

	// Main decode loop
	while (!param->stop_thread)
	{
		bEndOfStream = CVI_FALSE;
		bFindStart = CVI_FALSE;
		bFindEnd = CVI_FALSE;
		u32Start = 0;

		// Read frame data
		s32Ret = fseek(fpStrm, s32UsedBytes, SEEK_SET);
		s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);

		// Handle EOF - loop back to start
		if (s32ReadLen == 0)
		{
			memset(&stStream, 0, sizeof(VDEC_STREAM_S));
			stStream.bEndOfStream = CVI_TRUE;
			s32UsedBytes = 0;
			fseek(fpStrm, 0, SEEK_SET);
			s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);
		}

		// Parse H.264 frame
		if (param->stChnAttr.enMode == VIDEO_MODE_FRAME &&
			param->stChnAttr.enType == PT_H264)
		{

			// Find frame start
			for (i = 0; i < s32ReadLen - 8; i++)
			{
				int tmp = pu8Buf[i + 3] & 0x1F;

				if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					(((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
					 (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)))
				{
					bFindStart = CVI_TRUE;
					i += 8;
					break;
				}
			}

			// Find frame end
			for (; i < s32ReadLen - 8; i++)
			{
				int tmp = pu8Buf[i + 3] & 0x1F;

				if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					(tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
					 ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
					 (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)))
				{
					bFindEnd = CVI_TRUE;
					break;
				}
			}

			// Adjust read length based on frame boundaries
			if (i > 0)
				s32ReadLen = i;
			if (!bFindStart)
			{
				printf("Cannot find H264 start code for channel %d\n", param->VdecChn);
			}
			if (!bFindEnd)
			{
				s32ReadLen = i + 8;
			}
		}

		// Set up stream structure
		stStream.u64PTS = u64PTS;
		stStream.pu8Addr = pu8Buf + u32Start;
		stStream.u32Len = s32ReadLen;
		stStream.bEndOfFrame = CVI_TRUE;
		stStream.bEndOfStream = bEndOfStream;
		stStream.bDisplay = 1;

	SendAgain:
		// Send stream to decoder
		s32Ret = CVI_VDEC_SendStream(param->VdecChn, &stStream, 1000); // Add 1s timeout
		if (s32Ret == CVI_ERR_VDEC_BUSY || s32Ret == CVI_ERR_VDEC_BUF_FULL)
		{
			if (param->stop_thread)
				break;
			// Buffer full - wait longer before retry
			usleep(50000); // Increased wait time to 50ms
			goto SendAgain;
		}
		else if (s32Ret != CVI_SUCCESS)
		{
			printf("CVI_VDEC_SendStream failed with %#x\n", s32Ret);
			if (param->stop_thread)
				break;
			usleep(10000);
			goto SendAgain;
		}

		// Update state
		s32UsedBytes = s32UsedBytes + s32ReadLen + u32Start;
		u64PTS += 1;

		// Increased frame interval to prevent buffer overflow
		usleep(40000); // 40ms interval ~= 25fps
	}

	// Send end of stream
	memset(&stStream, 0, sizeof(VDEC_STREAM_S));
	stStream.bEndOfStream = CVI_TRUE;
	CVI_VDEC_SendStream(param->VdecChn, &stStream, -1);

	// Cleanup
	printf("VDEC send stream thread exiting for channel %d\n", param->VdecChn);
	if (pu8Buf != NULL)
	{
		free(pu8Buf);
	}
	fclose(fpStrm);

	return (CVI_VOID *)CVI_SUCCESS;
}

CVI_S32 start_thread(VDEC_CONFIG_S *pstVdecCfg)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	struct sched_param param;
	pthread_attr_t attr;

	param.sched_priority = 80;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	for (int i = 0; i < pstVdecCfg->s32ChnNum; i++)
	{
		s32Ret = pthread_create(&pstVdecCfg->astVdecParam[i].vdec_thread, &attr,
								thread_vdec_send_stream, (CVI_VOID *)&pstVdecCfg->astVdecParam[i]);
		if (s32Ret != 0)
		{
			return CVI_FAILURE;
		}
		usleep(100000);
	}
	// usleep(100000);
	// if (pstVdecCfg->astVdecParam[0].bind_mode == VDEC_BIND_DISABLE)
	// {
	// 	s32Ret = pthread_create(&send_vo_thread, &attr, thread_send_vo, (CVI_VOID *)pstVdecCfg);
	// 	if (s32Ret != 0)
	// 	{
	// 		return CVI_FAILURE;
	// 	}
	// }

	return CVI_SUCCESS;
}

CVI_VOID stop_thread(VDEC_CONFIG_S *pstVdecCfg)
{
	pstVdecCfg->astVdecParam[0].stop_thread = CVI_TRUE;
	// if (send_vo_thread != 0)
	// 	pthread_join(send_vo_thread, NULL);

	for (int i = 0; i < pstVdecCfg->s32ChnNum; i++)
	{
		pstVdecCfg->astVdecParam[i].stop_thread = CVI_TRUE;
		if (pstVdecCfg->astVdecParam[i].vdec_thread != 0)
			pthread_join(pstVdecCfg->astVdecParam[i].vdec_thread, NULL);
	}
}

static inline CVI_S32 SAVE_FILE_NAME(CVI_CHAR *aFileName, CVI_S32 chn, CVI_CHAR *cStreamName,
									 PIXEL_FORMAT_E enPixelFormat)
{
	CVI_CHAR *Postfix;

	if (enPixelFormat == PIXEL_FORMAT_RGB_888)
	{
		Postfix = "rgb888";
	}
	else if (enPixelFormat == PIXEL_FORMAT_BGR_888)
	{
		Postfix = "bgr888";
	}
	else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888)
	{
		Postfix = "argb8888";
	}
	else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555)
	{
		Postfix = "argb1555";
	}
	else if (enPixelFormat == PIXEL_FORMAT_YUV_400 ||
			 enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420 ||
			 enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_422 ||
			 enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_444 ||
			 enPixelFormat == PIXEL_FORMAT_NV12 ||
			 enPixelFormat == PIXEL_FORMAT_NV21)
	{
		Postfix = "yuv";
	}
	else
	{
		CVI_VDEC_ERR("[%s]-%d: enPixelFormat type err", __func__, __LINE__);
		Postfix = "unk";
	}
	CVI_VDEC_INFO("aFileName = %s, cStreamName = %s\n", aFileName, cStreamName);
	sprintf(aFileName, "chn%d_%s.%s", chn, cStreamName, Postfix);
	CVI_VDEC_INFO("aFileName = %s\n", aFileName);

	return 0;
}

static void get_chroma_size_shift_factor(PIXEL_FORMAT_E enPixelFormat, CVI_S32 *w_shift, CVI_S32 *h_shift)
{
	switch (enPixelFormat)
	{
	case PIXEL_FORMAT_YUV_PLANAR_420:
		*w_shift = 1;
		*h_shift = 1;
		break;
	case PIXEL_FORMAT_YUV_PLANAR_422:
		*w_shift = 1;
		*h_shift = 0;
		break;
	case PIXEL_FORMAT_YUV_PLANAR_444:
		*w_shift = 0;
		*h_shift = 0;
		break;
	case PIXEL_FORMAT_NV12:
	case PIXEL_FORMAT_NV21:
		*w_shift = 0;
		*h_shift = 1;
		break;
	case PIXEL_FORMAT_YUV_400: // no chroma
	default:
		*w_shift = 31;
		*h_shift = 31;
		break;
	}
}

static int write_yuv(FILE *out_f, VIDEO_FRAME_S stVFrame)
{
	CVI_S32 c_w_shift, c_h_shift; // chroma width/height shift
	CVI_U8 *w_ptr;

	CVI_VDEC_INFO("u32Width = %d, u32Height = %d\n",
				  stVFrame.u32Width, stVFrame.u32Height);
	CVI_VDEC_INFO("u32Stride[0] = %d, u32Stride[1] = %d, u32Stride[2] = %d\n",
				  stVFrame.u32Stride[0], stVFrame.u32Stride[1], stVFrame.u32Stride[2]);
	CVI_VDEC_INFO("u32Length[0] = %d, u32Length[1] = %d, u32Length[2] = %d\n",
				  stVFrame.u32Length[0], stVFrame.u32Length[1], stVFrame.u32Length[2]);

	get_chroma_size_shift_factor(stVFrame.enPixelFormat, &c_w_shift, &c_h_shift);

	w_ptr = stVFrame.pu8VirAddr[0];
	for (CVI_U32 i = 0; i < stVFrame.u32Height; i++)
	{
		fwrite(w_ptr + i * stVFrame.u32Stride[0], 1, stVFrame.u32Width, out_f);
	}

	if (stVFrame.pu8VirAddr[1])
	{
		w_ptr = stVFrame.pu8VirAddr[1];
		for (CVI_U32 i = 0; i < (stVFrame.u32Height >> c_h_shift); i++)
		{
			fwrite(w_ptr + i * stVFrame.u32Stride[1], 1, stVFrame.u32Width >> c_w_shift, out_f);
		}
	}

	if (stVFrame.pu8VirAddr[2])
	{
		w_ptr = stVFrame.pu8VirAddr[2];
		for (CVI_U32 i = 0; i < (stVFrame.u32Height >> c_h_shift); i++)
		{
			fwrite(w_ptr + i * stVFrame.u32Stride[2], 1, stVFrame.u32Width >> c_w_shift, out_f);
		}
	}

	return 0;
}

CVI_S32 VDEC_BIND_VPSS_BIND_VENC(CVI_S32 decoding_file_num, CVI_VOID *s_h264file[])
{
	COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;
	VB_CONFIG_S stVbConf;
	CVI_U32 u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 VencChn = 0;
	int filelen;

#ifndef VDEC_WIDTH
#define VDEC_WIDTH 1920
#endif
#ifndef VDEC_HEIGHT
#define VDEC_HEIGHT 1080
#endif
#ifndef VPSS_WIDTH
#define VPSS_WIDTH 1920
#endif
#ifndef VPSS_HEIGHT
#define VPSS_HEIGHT 1080
#endif

	stSize.u32Width = VDEC_WIDTH;
	stSize.u32Height = VDEC_HEIGHT;

	/************************************************
	 * step1:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, VI_PIXEL_FORMAT, DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = 5;
	stVbConf.astCommPool[0].enRemapMode = VB_REMAP_MODE_NONE;
	printf("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step2:  Init VPSS
	 ************************************************/
	VPSS_GRP VpssGrp = 0;
	VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
	VPSS_CHN VpssChn = VPSS_CHN0;
	CVI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate = 30;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate = 30;
	stVpssGrpAttr.enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420;
	stVpssGrpAttr.u32MaxW = stSize.u32Width;
	stVpssGrpAttr.u32MaxH = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev = 0;

	astVpssChnAttr[VpssChn].u32Width = VPSS_WIDTH;
	astVpssChnAttr[VpssChn].u32Height = VPSS_HEIGHT;
	astVpssChnAttr[VpssChn].enVideoFormat = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth = 4;
	astVpssChnAttr[VpssChn].bMirror = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;

	abChnEnable[0] = CVI_TRUE;
	s32Ret = COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init VDEC
	 ************************************************/
	VDEC_CONFIG_S stVdecCfg = {0};
	VDEC_PARAM_S *pVdecChn[MAX_VDEC_NUM];

	stVdecCfg.s32ChnNum = decoding_file_num;

	for (int i = 0; i < stVdecCfg.s32ChnNum; i++)
	{
		pVdecChn[i] = &stVdecCfg.astVdecParam[i];
		pVdecChn[i]->VdecChn = i;
		pVdecChn[i]->stop_thread = CVI_FALSE;
		// pVdecChn[i]->bind_mode = VDEC_BIND_VPSS;
		filelen = snprintf(pVdecChn[i]->decode_file_name, 63, "%s", (char *)s_h264file[i]);
		pVdecChn[i]->stChnAttr.enType = find_file_type(pVdecChn[i]->decode_file_name, filelen);
		pVdecChn[i]->stChnAttr.enMode = VIDEO_MODE_FRAME;
		pVdecChn[i]->stChnAttr.u32PicWidth = VDEC_WIDTH;
		pVdecChn[i]->stChnAttr.u32PicHeight = VDEC_HEIGHT;
		pVdecChn[i]->stChnAttr.u32StreamBufSize = VDEC_WIDTH * VDEC_HEIGHT * 3; // Triple buffer size
		pVdecChn[i]->stChnAttr.u32FrameBufCnt = 4;
		// if (pVdecChn[i]->stChnAttr.enType == PT_JPEG || pVdecChn[i]->stChnAttr.enType == PT_MJPEG)
		// {
		// 	pVdecChn[i]->stChnAttr.u32FrameBufSize = VDEC_GetPicBufferSize(
		// 		pVdecChn[i]->stChnAttr.enType, pVdecChn[i]->stChnAttr.u32PicWidth,
		// 		pVdecChn[i]->stChnAttr.u32PicHeight, PIXEL_FORMAT_YUV_PLANAR_444,
		// 		DATA_BITWIDTH_8, COMPRESS_MODE_NONE);
		// }
		pVdecChn[i]->stDispRect.s32X = (VPSS_WIDTH >> (stVdecCfg.s32ChnNum - 1)) * i;
		pVdecChn[i]->stDispRect.s32Y = 0;
		pVdecChn[i]->stDispRect.u32Width = (VPSS_WIDTH >> (stVdecCfg.s32ChnNum - 1));
		pVdecChn[i]->stDispRect.u32Height = 1080;
		pVdecChn[i]->vdec_vb_source = VB_SOURCE_COMMON;
		pVdecChn[i]->vdec_pixel_format = PIXEL_FORMAT_YUV_PLANAR_420;
	}

	/************************************************
	 * step4:  Init VENC with modified settings
	 ************************************************/
	// chnInputCfg inputCfg;
	// COMM_VENC_InitChnInputCfg(&inputCfg);

	// // Configure VENC parameters
	// inputCfg.width = VPSS_WIDTH;	 // Match VPSS output width
	// inputCfg.height = VPSS_HEIGHT;	 // Match VPSS output height
	// inputCfg.framerate = 30;		 // Target frame rate
	// inputCfg.gop = 30;				 // GOP size
	// inputCfg.bitrate = 2000;		 // Target bitrate in Kbps
	// inputCfg.rcMode = SAMPLE_RC_CBR; // Use CBR mode
	// inputCfg.bind_mode = 0;
	// inputCfg.vpssGrp = VpssGrp; // Bind to VPSS group
	// inputCfg.vpssChn = VpssChn; // Bind to VPSS channel
	// inputCfg.bEsBufQueueEn = CVI_TRUE;
	// inputCfg.num_frames = -1; // Continuous encoding

	// // Initialize GOP attributes
	// VENC_GOP_ATTR_S stGopAttr;
	// stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
	// stGopAttr.stNormalP.s32IPQpDelta = 3;

	// // Create and start VENC channel using COMM function
	// s32Ret = COMM_VENC_Start(&inputCfg, VencChn, PT_H264, PIC_1080P,
	// 						 SAMPLE_RC_CBR, 0, CVI_TRUE, &stGopAttr);
	// if (s32Ret != CVI_SUCCESS)
	// {
	// 	printf("COMM_VENC_Start failed with %#x\n", s32Ret);
	// 	return s32Ret;
	// }

	////////////////////////////////////////////////////
	// init VB(for VDEC)
	////////////////////////////////////////////////////
	VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];

	for (int i = 0; i < stVdecCfg.s32ChnNum; i++)
	{
		astSampleVdec[i].enType = pVdecChn[i]->stChnAttr.enType;
		astSampleVdec[i].u32Width = VDEC_WIDTH;
		astSampleVdec[i].u32Height = VDEC_HEIGHT;

		astSampleVdec[i].enMode = VIDEO_MODE_FRAME;
		astSampleVdec[i].stSampleVdecVideo.enDecMode = VIDEO_DEC_MODE_IP;
		astSampleVdec[i].stSampleVdecVideo.enBitWidth = DATA_BITWIDTH_8;
		astSampleVdec[i].stSampleVdecVideo.u32RefFrameNum = 2;
		astSampleVdec[i].u32DisplayFrameNum =
			(astSampleVdec[i].enType == PT_JPEG || astSampleVdec[i].enType == PT_MJPEG) ? 0 : 2;
		astSampleVdec[i].enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420;
		astSampleVdec[i].u32FrameBufCnt =
			(astSampleVdec[i].enType == PT_JPEG || astSampleVdec[i].enType == PT_MJPEG) ? 1 : 4;

		s32Ret = vdec_init_vb_pool(i, &astSampleVdec[i], CVI_TRUE);
		if (s32Ret != CVI_SUCCESS)
		{
			CVI_VDEC_ERR("SAMPLE_COMM_VDEC_InitVBPool fail\n");
		}
	}

	for (int i = 0; i < stVdecCfg.s32ChnNum; i++)
	{
		s32Ret = start_vdec(pVdecChn[i]);
		if (s32Ret != CVI_SUCCESS)
		{
			printf("Failed to start VDEC channel %d: %#x\n", i, s32Ret);
			return s32Ret;
		}
	}

	s32Ret = COMM_VDEC_Bind_VPSS(0, VpssGrp);
	if (s32Ret != CVI_SUCCESS)
	{
		printf("Failed to bind VDEC to VPSS: %#x\n", s32Ret);
		return s32Ret;
	}

	VDEC_CHN VdecChn = 0;
	VIDEO_FRAME_INFO_S stFrameInfo;
	FILE *fpStrm = NULL;
	FILE *fpYuv = NULL;
	CVI_BOOL bEndOfStream = CVI_FALSE;
	CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
	CVI_U8 *pu8Buf = NULL;
	VDEC_STREAM_S stStream;
	CVI_BOOL bFindEnd = CVI_FALSE;
	CVI_U32 u32Start = 0;
	CVI_S32 i;
	CVI_U64 u64PTS = 0;
	int bufsize = (VDEC_WIDTH * VDEC_HEIGHT * 3) >> 1;

	fpStrm = fopen((char *)s_h264file[0], "rb");
	fpYuv = fopen("output.yuv", "wb");
	if (fpStrm == NULL || fpYuv == NULL)
	{
		printf("Open file failed!\n");
		return CVI_FAILURE;
	}

	pu8Buf = malloc(bufsize);
	if (pu8Buf == NULL)
	{
		printf("malloc failed!\n");
		fclose(fpStrm);
		fclose(fpYuv);
		return CVI_FAILURE;
	}

	printf("Buffer size: %d bytes, Frame size: %dx%d\n",
		   bufsize, VDEC_WIDTH, VDEC_HEIGHT);

	while (!bEndOfStream)
	{
		printf("\n--- Frame Processing ---\n");
		printf("Reading at offset: %d, Read size: %d\n", s32UsedBytes, s32ReadLen);

		bFindEnd = CVI_FALSE;
		u32Start = 0;
		s32Ret = fseek(fpStrm, s32UsedBytes, SEEK_SET);
		s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);
		if (s32ReadLen == 0)
		{
			bEndOfStream = CVI_TRUE;
			break;
		}

		// Find frame boundaries for H.264
		for (i = 0; i < s32ReadLen - 8; i++)
		{
			int tmp = pu8Buf[i + 3] & 0x1F;

			if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
				(((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
				 (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)))
			{
				i += 8;
				break;
			}
		}

		for (; i < s32ReadLen - 8; i++)
		{
			int tmp = pu8Buf[i + 3] & 0x1F;

			if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
				(tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
				 ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
				 (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)))
			{
				bFindEnd = CVI_TRUE;
				break;
			}
		}

		if (i > 0)
			s32ReadLen = i;
		if (bFindEnd == CVI_FALSE)
		{
			s32ReadLen = i + 8;
		}

		// Print frame info
		if (bFindEnd)
		{
			printf("Found complete frame: size=%d bytes\n", s32ReadLen);
		}

		stStream.u64PTS = u64PTS;
		stStream.pu8Addr = pu8Buf + u32Start;
		stStream.u32Len = s32ReadLen;
		stStream.bEndOfFrame = CVI_TRUE;
		stStream.bEndOfStream = bEndOfStream;
		stStream.bDisplay = 1;

		printf("Sending frame to decoder: PTS=%llu, size=%d\n", u64PTS, s32ReadLen);

		s32Ret = CVI_VDEC_SendStream(VdecChn, &stStream, -1);
		if (s32Ret != CVI_SUCCESS)
		{
			printf("CVI_VDEC_SendStream failed with %#x!\n", s32Ret);
			continue;
		}
		printf("Frame sent successfully\n");

		usleep(10000);

		// // Before getting decoded frame
		// printf("Trying to get frame from VPSS...\n");

		// s32Ret = CVI_VDEC_GetFrame(VdecChn, &stFrameInfo, -1);
		// if (s32Ret == CVI_SUCCESS)
		// {
		// 	printf("Got frame from VDEC: W=%d, H=%d, stride=%d\n",
		// 		   stFrameInfo.stVFrame.u32Width,
		// 		   stFrameInfo.stVFrame.u32Height,
		// 		   stFrameInfo.stVFrame.u32Stride[0]);
		// 	printf("Frame released from VDEC\n");
		// 	CVI_VDEC_ReleaseFrame(VdecChn, &stFrameInfo);
		// }
		// else
		// {
		// 	printf("Failed to get frame from VDEC, error: 0x%x\n", s32Ret);
		// }

		// CVI_VPSS_SendChnFrame(VpssGrp,VpssChn,&stFrameInfo, -1);
		// if(s32Ret != CVI_SUCCESS)
		// {
		// 	printf("Failed to send frame to VPSS, error: 0x%x\n", s32Ret);
		// }
		// else
		// {
		// 	printf("Frame sent to VPSS\n");
		// }
		// printf("Frame sent to VPSS\n");
		// s32Ret = CVI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrameInfo, -1);
		s32Ret = CVI_VDEC_GetFrame(VdecChn, &stFrameInfo, -1);
		if (s32Ret == CVI_SUCCESS)
		{
			printf("Got frame from VPSS: W=%d, H=%d, stride=%d\n",
				   stFrameInfo.stVFrame.u32Width,
				   stFrameInfo.stVFrame.u32Height,
				   stFrameInfo.stVFrame.u32Stride[0]);

			write_yuv(fpYuv, stFrameInfo.stVFrame);
			printf("Frame written to YUV file\n");

			// Release VPSS frame instead of VDEC frame
			CVI_VDEC_ReleaseFrame(VdecChn, &stFrameInfo);
			printf("Frame released from VPSS\n");
		}
		else
		{
			printf("Failed to get frame from VPSS, error: 0x%x\n", s32Ret);
		}

		s32UsedBytes = s32UsedBytes + s32ReadLen + u32Start;
		printf("Total bytes processed: %d\n", s32UsedBytes);
		printf("Frame count: %llu\n", u64PTS);
		u64PTS++;
	}

	printf("\nDecode loop finished\n");
	printf("Total frames processed: %llu\n", u64PTS);

	// Cleanup
	if (pu8Buf != NULL)
	{
		free(pu8Buf);
	}
	if (fpStrm != NULL)
	{
		fclose(fpStrm);
	}
	if (fpYuv != NULL)
	{
		fclose(fpYuv);
	}

	for (int i = 0; i < stVdecCfg.s32ChnNum; i++)
	{
		COMM_VDEC_UnBind_VPSS(i, VpssGrp);
		stop_vdec(pVdecChn[i]);
	}

	COMM_VPSS_Stop(VpssGrp, abChnEnable);
	// COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn, VencChn);
	CVI_VENC_StopRecvFrame(VencChn);
	CVI_VENC_ResetChn(VencChn);
	CVI_VENC_DestroyChn(VencChn);
	vdec_exit_vb_pool();

	COMM_SYS_Exit();
	return s32Ret;
}