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

#include "cvi_buffer.h"
#include "cvi_sys.h"
#include "cvi_comm_vpss.h"
#include "cvi_vpss.h"
#include "cvi_sys.h"
#include "cvi_comm_sys.h"
#include "cvi_vb.h"
#include "cvi_comm_vb.h"
#include "cvi_comm_vdec.h"
#include "sys_init.h"
#include "vdec_init.h"
#include "venc_init.h"

VB_POOL g_ahLocalPicVbPool[MAX_VDEC_NUM] = {[0 ...(MAX_VDEC_NUM - 1)] = VB_INVALID_POOLID};

CVI_S32 vdec_init_vb_pool(VDEC_CHN ChnIndex, SAMPLE_VDEC_ATTR *pastSampleVdec, CVI_BOOL is_user)
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
            s32Ret = SYS_Init(&stVbConf);
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

CVI_S32 VPSS_Start(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
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

CVI_S32 start_vdec(SAMPLE_VDEC_PARAM_S *pVdecParam)
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

CVI_VOID stop_vdec(SAMPLE_VDEC_PARAM_S *pVdecParam)
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

CVI_S32 VPSS_Init(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
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

CVI_S32 SYS_Init(VB_CONFIG_S *pstVbConfig)
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

CVI_S32 VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
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

CVI_VOID *thread_vdec_send_stream(CVI_VOID *arg)
{
    FILE *fpStrm = NULL;
    SAMPLE_VDEC_PARAM_S *param = (SAMPLE_VDEC_PARAM_S *)arg;
    CVI_BOOL bEndOfStream = CVI_FALSE;
    CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
    CVI_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    CVI_BOOL bFindStart, bFindEnd;
    CVI_U64 u64PTS = 0;
    CVI_U32 u32Len, u32Start;
    CVI_S32 s32Ret, i;
    CVI_S32 bufsize = (param->stChnAttr.u32PicWidth * param->stChnAttr.u32PicHeight * 3) >> 1;
    char strBuf[64];
    int cur_cnt = 0;

    FILE *outFd = NULL;
    outFd = fopen("out.264", "wb");
    CVI_S32 VencChn = 0;
    VENC_STREAM_S stEncStream;
    VENC_CHN_STATUS_S stStat;
#ifdef SHOW_STATISTICS_1
    struct timeval pre_tv, tv1, tv2;
    int pre_cnt = 0;
    int accum_ms = 0;

    pre_tv.tv_usec =
        pre_tv.tv_sec = 0;
#endif

    snprintf(strBuf, sizeof(strBuf), "thread_vdec-%d", param->VdecChn);
    prctl(PR_SET_NAME, strBuf);

    if (param->decode_file_name != 0)
    {
        fpStrm = fopen(param->decode_file_name, "rb");
        if (fpStrm == NULL)
        {
            printf("open file err, %s\n", param->decode_file_name);
            return (CVI_VOID *)(CVI_FAILURE);
        }
        pu8Buf = malloc(bufsize);
        if (pu8Buf == NULL)
        {
            printf("chn %d can't alloc %d in send stream thread!\n",
                   param->VdecChn,
                   bufsize);
            fclose(fpStrm);
            return (CVI_VOID *)(CVI_FAILURE);
        }
    }
    fflush(stdout);

    printf("thread_vdec_send_stream %d\n", param->VdecChn);
    u64PTS = 0;
    while (!param->stop_thread)
    {
        int retry = 0;

        bEndOfStream = CVI_FALSE;
        bFindStart = CVI_FALSE;
        bFindEnd = CVI_FALSE;
        u32Start = 0;
        s32Ret = fseek(fpStrm, s32UsedBytes, SEEK_SET);
        s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);
        if (s32ReadLen == 0)
        { // file end
            memset(&stStream, 0, sizeof(VDEC_STREAM_S));
            stStream.bEndOfStream = CVI_TRUE;
            s32UsedBytes = 0;
            fseek(fpStrm, 0, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);
        }
        if (param->stChnAttr.enMode == VIDEO_MODE_FRAME &&
            param->stChnAttr.enType == PT_H264)
        {
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
            if (bFindStart == CVI_FALSE)
            {
                CVI_VDEC_TRACE("chn %d can not find H264 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                               param->VdecChn, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == CVI_FALSE)
            {
                s32ReadLen = i + 8;
            }
        }
        else if (param->stChnAttr.enMode == VIDEO_MODE_FRAME &&
                 param->stChnAttr.enType == PT_H265)
        {
            CVI_BOOL bNewPic = CVI_FALSE;

            for (i = 0; i < s32ReadLen - 6; i++)
            {
                CVI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;

                bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
                           (tmp <= 21) && ((pu8Buf[i + 5] & 0x80) == 0x80));

                if (bNewPic)
                {
                    bFindStart = CVI_TRUE;
                    i += 6;
                    break;
                }
            }

            for (; i < s32ReadLen - 6; i++)
            {
                CVI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;

                bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
                           (tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 ||
                            ((tmp <= 21) && (pu8Buf[i + 5] & 0x80) == 0x80)));

                if (bNewPic)
                {
                    bFindEnd = CVI_TRUE;
                    break;
                }
            }
            if (i > 0)
                s32ReadLen = i;

            if (bFindStart == CVI_FALSE)
            {
                CVI_VDEC_TRACE("chn 0 can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                               s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == CVI_FALSE)
            {
                s32ReadLen = i + 6;
            }
        }
        else if (param->stChnAttr.enType == PT_MJPEG || param->stChnAttr.enType == PT_JPEG)
        {
            for (i = 0; i < s32ReadLen - 1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD8)
                {
                    u32Start = i;
                    bFindStart = CVI_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i < s32ReadLen - 3; i++)
            {
                if ((pu8Buf[i] == 0xFF) && (pu8Buf[i + 1] & 0xF0) == 0xE0)
                {
                    u32Len = (pu8Buf[i + 2] << 8) + pu8Buf[i + 3];
                    i += 1 + u32Len;
                }
                else
                {
                    break;
                }
            }

            for (; i < s32ReadLen - 1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD9)
                {
                    bFindEnd = CVI_TRUE;
                    break;
                }
            }
            s32ReadLen = i + 2;

            if (bFindStart == CVI_FALSE)
            {
                CVI_VDEC_TRACE("chn %d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                               param->VdecChn, s32ReadLen, s32UsedBytes);
            }
        }
        else
        {
            if ((s32ReadLen != 0) && (s32ReadLen < bufsize))
            {
                bEndOfStream = CVI_TRUE;
            }
        }

        stStream.u64PTS = u64PTS;
        stStream.pu8Addr = pu8Buf + u32Start;
        stStream.u32Len = s32ReadLen;
        stStream.bEndOfFrame = CVI_TRUE;
        stStream.bEndOfStream = bEndOfStream;
        stStream.bDisplay = 1;
    SendAgain:
#ifdef SHOW_STATISTICS_1
        gettimeofday(&tv1, NULL);
#endif
        s32Ret = CVI_VDEC_SendStream(param->VdecChn, &stStream, -1);
        if (s32Ret != CVI_SUCCESS)
        {
            // printf("%d dec chn CVI_VDEC_SendStream err ret=%d\n",param->vdec_chn,s32Ret);
            retry++;
            if (param->stop_thread)
                break;
            usleep(10000);
            goto SendAgain;
        }
        else
        {
            bEndOfStream = CVI_FALSE;
            s32UsedBytes = s32UsedBytes + s32ReadLen + u32Start;
            u64PTS += 1;
            cur_cnt++;
        }
        if (param->bind_mode == VDEC_BIND_VENC)
        {
            s32Ret = CVI_VENC_QueryStatus(VencChn, &stStat);
            if (s32Ret != CVI_SUCCESS)
            {
                CVI_VENC_ERR("CVI_VENC_QueryStatus, Vench = %d, s32Ret = %d\n",
                             VencChn, s32Ret);
            }

            if (!stStat.u32CurPacks)
            {
                CVI_VENC_ERR("u32CurPacks = NULL!\n");
            }
            stEncStream.pstPack =
                (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (stEncStream.pstPack == NULL)
            {
                CVI_VENC_ERR("malloc memory failed!\n");
            }

            s32Ret = CVI_VENC_GetStream(VencChn, &stEncStream, -1);
            if (!s32Ret)
            {
                s32Ret = SAMPLE_COMM_VENC_SaveStream(
                    param->stChnAttr.enType,
                    outFd,
                    &stEncStream);
                if (s32Ret != CVI_SUCCESS)
                {
                    CVI_VENC_ERR("SAMPLE_COMM_VENC_SaveStream, s32Ret = %d\n", s32Ret);
                    break;
                }
                CVI_VENC_ReleaseStream(VencChn, &stEncStream);
                free(stEncStream.pstPack);
                stEncStream.pstPack = NULL;
            }
        }
#ifdef SHOW_STATISTICS_1
        gettimeofday(&tv2, NULL);
        int curr_ms =
            (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec / 1000) - (tv1.tv_usec / 1000);

        accum_ms += curr_ms;
        if (pre_tv.tv_usec == 0 && pre_tv.tv_sec == 0)
        {
            pre_tv = tv2;
        }
        else
        {
            unsigned long diffus = 0;

            if (pre_tv.tv_sec == tv2.tv_sec)
            {
                if (tv2.tv_usec > pre_tv.tv_usec)
                {
                    diffus = tv2.tv_usec - pre_tv.tv_usec;
                }
            }
            else if (tv2.tv_sec > pre_tv.tv_sec)
            {
                diffus = (tv2.tv_sec - pre_tv.tv_sec) * 1000000;
                diffus = diffus + tv2.tv_usec - pre_tv.tv_usec;
            }

            if (diffus == 0)
            {
                pre_tv = tv2;
                pre_cnt = cur_cnt;
            }
            else if (diffus > 1000000)
            {
                int add_cnt = cur_cnt - pre_cnt;
                double avg_fps = (add_cnt * 1000000.0 / (double)diffus);

                printf("[%d] CVI_VDEC_SendStream Avg. %d ms FPS %.2lf\n", param->VdecChn, accum_ms / add_cnt, avg_fps);
                pre_tv = tv2;
                pre_cnt = cur_cnt;
                accum_ms = 0;
            }
        }
#endif
        usleep(20000);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
    stStream.bEndOfStream = CVI_TRUE;
    CVI_VDEC_SendStream(param->VdecChn, &stStream, -1);

    printf("thread_vdec_send_stream %d exit\n", param->VdecChn);

    fflush(stdout);
    if (pu8Buf != CVI_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);
    if (param->bind_mode == VDEC_BIND_VENC)
    {
        fclose(outFd);
    }
    return (CVI_VOID *)CVI_SUCCESS;
}

CVI_S32 start_thread(SAMPLE_VDEC_CONFIG_S *pstVdecCfg)
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
    usleep(100000);
    if (pstVdecCfg->astVdecParam[0].bind_mode == VDEC_BIND_DISABLE)
    {
        s32Ret = pthread_create(&send_vo_thread, &attr, thread_send_vo, (CVI_VOID *)pstVdecCfg);
        if (s32Ret != 0)
        {
            return CVI_FAILURE;
        }
    }

    return CVI_SUCCESS;
}

CVI_VOID SYS_Exit(void)
{
    // CVI_BOOL abChnEnable[VPSS_MAX_CHN_NUM] = {CVI_TRUE, };

    // for (VPSS_GRP VpssGrp = 0; VpssGrp < VPSS_MAX_GRP_NUM; ++VpssGrp)
    // SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
    // SAMPLE_COMM_VO_Exit();
    CVI_SYS_Exit();
    CVI_VB_Exit();
}

CVI_S32 VPSS_Stop(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable)
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

CVI_S32 VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
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

CVI_VOID stop_thread(SAMPLE_VDEC_CONFIG_S *pstVdecCfg)
{
    pstVdecCfg->astVdecParam[0].stop_thread = CVI_TRUE;
    if (send_vo_thread != 0)
        pthread_join(send_vo_thread, NULL);

    for (int i = 0; i < pstVdecCfg->s32ChnNum; i++)
    {
        pstVdecCfg->astVdecParam[i].stop_thread = CVI_TRUE;
        if (pstVdecCfg->astVdecParam[i].vdec_thread != 0)
            pthread_join(pstVdecCfg->astVdecParam[i].vdec_thread, NULL);
    }
}

CVI_S32 VDEC_BIND_VPSS_BIND_VENC(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;
    VB_CONFIG_S stVbConf;
    CVI_U32 u32BlkSize;
    SIZE_S stSize;
    CVI_S32 s32Ret = CVI_SUCCESS;
    int filelen;

#ifndef VDEC_WIDTH
#define VDEC_WIDTH 1920
#endif
#ifndef VDEC_HEIGHT
#define VDEC_HEIGHT 1080
#endif
#ifndef VPSS_WIDTH
#define VPSS_WIDTH 1280
#endif
#ifndef VPSS_HEIGHT
#define VPSS_HEIGHT 720
#endif

    stSize.u32Width = VDEC_WIDTH;
    stSize.u32Height = VDEC_HEIGHT;

    /************************************************
     * step1:  Init SYS and common VB
     ************************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = 1;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 5;
    stVbConf.astCommPool[0].enRemapMode = VB_REMAP_MODE_NONE;
    printf("common pool[0] BlkSize %d\n", u32BlkSize);

    s32Ret = SYS_Init(&stVbConf);
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

    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.u8VpssDev = 0;

    astVpssChnAttr[VpssChn].u32Width = VPSS_WIDTH;
    astVpssChnAttr[VpssChn].u32Height = VPSS_HEIGHT;
    astVpssChnAttr[VpssChn].enVideoFormat = VIDEO_FORMAT_LINEAR;
    astVpssChnAttr[VpssChn].enPixelFormat = SAMPLE_PIXEL_FORMAT;
    astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
    astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
    astVpssChnAttr[VpssChn].u32Depth = 1;
    astVpssChnAttr[VpssChn].bMirror = CVI_FALSE;
    astVpssChnAttr[VpssChn].bFlip = CVI_FALSE;
    astVpssChnAttr[VpssChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;

    abChnEnable[0] = CVI_TRUE;
    s32Ret = VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (s32Ret != CVI_SUCCESS)
    {
        printf("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        return s32Ret;
    }

    s32Ret = VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (s32Ret != CVI_SUCCESS)
    {
        printf("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        return s32Ret;
    }

    /************************************************
     * step4:  Init VDEC
     ************************************************/
    SAMPLE_VDEC_CONFIG_S stVdecCfg = {0};
    SAMPLE_VDEC_PARAM_S *pVdecChn[MAX_VDEC_NUM];
    stVdecCfg.s32ChnNum = 1;

    for (int i = 0; i < stVdecCfg.s32ChnNum; i++)
    {
        pVdecChn[i] = &stVdecCfg.astVdecParam[i];
        pVdecChn[i]->VdecChn = i;
        pVdecChn[i]->stop_thread = CVI_FALSE;
        pVdecChn[i]->bind_mode = VDEC_BIND_VPSS_VO;
        filelen = snprintf(pVdecChn[i]->decode_file_name, 63, "%s", (char *)s_h264file[i]);
        pVdecChn[i]->stChnAttr.enType = PT_H264;
        pVdecChn[i]->stChnAttr.enMode = VIDEO_MODE_FRAME;
        pVdecChn[i]->stChnAttr.u32PicWidth = VDEC_WIDTH;
        pVdecChn[i]->stChnAttr.u32PicHeight = VDEC_HEIGHT;
        pVdecChn[i]->stChnAttr.u32StreamBufSize = VDEC_WIDTH * VDEC_HEIGHT;
        pVdecChn[i]->stChnAttr.u32FrameBufCnt = 1;
        if (pVdecChn[i]->stChnAttr.enType == PT_JPEG || pVdecChn[i]->stChnAttr.enType == PT_MJPEG)
        {
            pVdecChn[i]->stChnAttr.u32FrameBufSize = VDEC_GetPicBufferSize(
                pVdecChn[i]->stChnAttr.enType, pVdecChn[i]->stChnAttr.u32PicWidth,
                pVdecChn[i]->stChnAttr.u32PicHeight, PIXEL_FORMAT_YUV_PLANAR_444,
                DATA_BITWIDTH_8, COMPRESS_MODE_NONE);
        }
        pVdecChn[i]->stDispRect.s32X = (VPSS_WIDTH >> (stVdecCfg.s32ChnNum - 1)) * i;
        pVdecChn[i]->stDispRect.s32Y = 0;
        pVdecChn[i]->stDispRect.u32Width = (VPSS_WIDTH >> (stVdecCfg.s32ChnNum - 1));
        pVdecChn[i]->stDispRect.u32Height = 720;
        pVdecChn[i]->vdec_vb_source = VB_SOURCE_USER;
        pVdecChn[i]->vdec_pixel_format = PIXEL_FORMAT_YUV_PLANAR_420;
    }

    ////////////////////////////////////////////////////
    // init VB(for VDEC)
    ////////////////////////////////////////////////////
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];

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
        start_vdec(pVdecChn[i]);
    }

    VDEC_Bind_VPSS(0, VpssGrp);
    // SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, VoDev, VoChn);
    start_thread(&stVdecCfg);

    PAUSE();

    stop_thread(&stVdecCfg);
    printf("stop thread\n");
    // SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, VoDev, VoChn);
    for (int i = 0; i < stVdecCfg.s32ChnNum; i++)
    {
        VDEC_UnBind_VPSS(i, VpssGrp);
        stop_vdec(pVdecChn[i]);
    }
    // SAMPLE_COMM_VO_StopVO(&stVoConfig);
    VPSS_Stop(VpssGrp, abChnEnable);

    vdec_exit_vb_pool();

    SYS_Exit();
    return s32Ret;
}