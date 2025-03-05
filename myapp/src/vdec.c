#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/cvi_cv181x_defines.h>

#include "cvi_buffer.h"
#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_vdec.h"

#include "misc.h"
#include "vdec.h"

VB_POOL g_ahLocalPicVbPool[MAX_VDEC_NUM] = {[0 ...(MAX_VDEC_NUM - 1)] = VB_INVALID_POOLID};

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
            CVI_VDEC_ERR("NEED TO INIT SYS FIRST\n");
            // s32Ret = COMM_SYS_Init(&stVbConf);
            // if (s32Ret != CVI_SUCCESS)
            // {
            //     CVI_VDEC_ERR("SAMPLE_COMM_SYS_Init, %d\n", s32Ret);
            //     return CVI_FAILURE;
            // }
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

CVI_S32 VDEC_INIT(CVI_S32 decoding_file_num, VDEC_CONFIG_S *stVdecCfg)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    VDEC_PARAM_S *pVdecChn[MAX_VDEC_NUM] = {0};

    stVdecCfg->s32ChnNum = decoding_file_num;

    for (int i = 0; i < stVdecCfg->s32ChnNum; i++)
    {
        pVdecChn[i] = &stVdecCfg->astVdecParam[i];
        pVdecChn[i]->VdecChn = i;
        pVdecChn[i]->stChnAttr.enType = PT_H264;
        pVdecChn[i]->stChnAttr.enMode = VIDEO_MODE_STREAM; // Changed from FRAME to STREAM
        pVdecChn[i]->stChnAttr.u32PicWidth = VDEC_WIDTH;
        pVdecChn[i]->stChnAttr.u32PicHeight = VDEC_HEIGHT;
        pVdecChn[i]->stChnAttr.u32StreamBufSize = VDEC_WIDTH * VDEC_HEIGHT * 3;
        pVdecChn[i]->stChnAttr.u32FrameBufCnt = 8; // Increased buffer count
        pVdecChn[i]->vdec_vb_source = VB_SOURCE_COMMON;
        pVdecChn[i]->vdec_pixel_format = PIXEL_FORMAT_YUV_PLANAR_420;
    }

    ////////////////////////////////////////////////////
    // init VB(for VDEC)
    ////////////////////////////////////////////////////

    VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];

    for (int i = 0; i < stVdecCfg->s32ChnNum; i++)
    {
        astSampleVdec[i].enType = PT_H264;
        astSampleVdec[i].u32Width = VDEC_WIDTH;
        astSampleVdec[i].u32Height = VDEC_HEIGHT;

        astSampleVdec[i].enMode = VIDEO_MODE_STREAM;
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

    for (int i = 0; i < stVdecCfg->s32ChnNum; i++)
    {
        s32Ret = start_vdec(pVdecChn[i]);
        if (s32Ret != CVI_SUCCESS)
        {
            printf("Failed to start VDEC channel %d: %#x\n", i, s32Ret);
            return s32Ret;
        }
    }

    return s32Ret;
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

int write_yuv(FILE *out_f, VIDEO_FRAME_S stVFrame)
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

CVI_S32 VDEC_DESTROY(VDEC_CONFIG_S *vdecCtx)
{
    CVI_S32 s32Ret = CVI_SUCCESS;

    for (int i = 0; i < vdecCtx->s32ChnNum; i++)
    {
        stop_vdec(&vdecCtx->astVdecParam[i]);
    }

    vdec_exit_vb_pool();

    return s32Ret;
}