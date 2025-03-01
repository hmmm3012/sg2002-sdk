#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "cvi_vdec.h"
// #include "cvi_vpss.h"
// #include "cvi_venc.h"

#include "vdec.h"
#include "sys.h"
#include "misc.h"
#include "config.h"

// Include header files of FFmpeg
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/motion_vector.h>

// Video Pipeline of this sample:
//                                                       +------+
//                                     CHN0 (VBPool 0)    | VENC |--------> RTSP
//  +----+                     +----------------+---------------------> +------+
//  |Sample Videos -----|VDEC |----->| VPSS 0 (DEV 1) |            +-----------------------+
//  +----+                    +----------------+----------> | VPSS 1 (DEV 0) TDL SDK |------------> AI model
//                                     CHN1 (VBPool 1) +-----------------------+  CHN0 (VBPool 2)

//////////////////////////////////////////////////

static volatile CVI_BOOL bExit = CVI_FALSE;

CVI_VOID VDECVO_HandleSig(CVI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        printf("Program termination abnormally\n");
    }
    exit(1);
}

void Print_Usage(CVI_CHAR *sPrgNm)
{
    printf("Usage : %s <input.264>\n", sPrgNm);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Print_Usage(argv[0]);
        return -1;
    }

    // CVI_VOID *s_h264file[MAX_VDEC_NUM];
    CVI_S32 s32Ret = CVI_SUCCESS;
    VDEC_CONFIG_S stVdecCfg = {0};
    AVDictionary *opts = NULL;
    int retry = 0;
    int avFrameCounter = 0;
    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext *pCodecCtx = NULL;
    const AVCodec *pCodec = NULL;
    AVPacket pkt;
    int videoStreamIndex = -1;
    unsigned int i;

    // s_h264file[0] = argv[1];

    signal(SIGINT, VDECVO_HandleSig);
    signal(SIGTERM, VDECVO_HandleSig);

    // Initialize system
    s32Ret = SYS_INIT();
    if (s32Ret != CVI_SUCCESS)
    {
        printf("SYS_INIT failed with %#x\n", s32Ret);
        return -1;
    }

    // Initialize vdec
    s32Ret = VDEC_INIT(1, &stVdecCfg);
    if (s32Ret != CVI_SUCCESS)
    {
        printf("VDEC_INIT failed with %#x\n", s32Ret);
        return -1;
    }

    // Read and decode H.264 stream
    VIDEO_FRAME_INFO_S stFrameInfo;
    CVI_BOOL bEndOfStream = CVI_FALSE;
    VDEC_STREAM_S stStream;

    // CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
    // CVI_U8 *pu8Buf = NULL;
    // CVI_BOOL bFindEnd = CVI_FALSE;
    // CVI_U32 u32Start = 0;
    // CVI_U64 u64PTS = 0;
    // int bufsize = (VDEC_WIDTH * VDEC_HEIGHT * 3) >> 1;

    // pu8Buf = malloc(bufsize);
    // if (pu8Buf == NULL)
    // {
    //     printf("malloc failed!\n");
    //     return CVI_FAILURE;
    // }

    // printf("Buffer size: %d bytes, Frame size: %dx%d\n",
    //        bufsize, VDEC_WIDTH, VDEC_HEIGHT);

    while (1)
    {
        av_dict_set(&opts, "rw_timeout", "3000000", 0);
        av_dict_set(&opts, "stimeout", "3000000", 0);
        av_dict_set(&opts, "rtsp_transport", "tcp", 0);

        avFrameCounter = 0;
        int isEnd = 0;
        while (1)
        {
            pFormatCtx = avformat_alloc_context();
            if (avformat_open_input(&pFormatCtx, "rtsp://10.157.93.100:8554/h264", NULL, &opts) < 0)
            {
                avformat_close_input(&pFormatCtx);
                retry++;
                continue;
            }

            if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
            {
                avformat_close_input(&pFormatCtx);
                retry++;
                continue;
            }

            videoStreamIndex = -1;
            for (i = 0; i < pFormatCtx->nb_streams; i++)
            {
                if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    videoStreamIndex = i;
                    break;
                }
            }

            pCodecCtx = avcodec_alloc_context3(NULL);
            if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStreamIndex]->codecpar) < 0)
            {
                avformat_close_input(&pFormatCtx);
                avcodec_free_context(&pCodecCtx);
                retry++;
                continue;
            }

            pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
            if (pCodec == NULL)
            {
                avformat_close_input(&pFormatCtx);
                avcodec_free_context(&pCodecCtx);
                retry++;
                continue;
            }

            if (avcodec_open2(pCodecCtx, pCodec, &opts) < 0)
            {
                avcodec_free_context(&pCodecCtx);
                avformat_close_input(&pFormatCtx);
                retry++;
                continue;
            }

            while (av_read_frame(pFormatCtx, &pkt) >= 0 && !isEnd)
            {
                if (pkt.stream_index == videoStreamIndex)
                {
                    if (avFrameCounter == 300){
                        isEnd = 1;
                        break;
                    }
                    memset(&stStream, 0, sizeof(stStream));
                    stStream.u64PTS = pkt.pts;
                    stStream.pu8Addr = pkt.data;
                    stStream.u32Len = pkt.size;
                    stStream.bEndOfFrame = CVI_TRUE;
                    stStream.bEndOfStream = bEndOfStream;

                    CVI_S32 retry_count = 0;
                    while (retry_count < 3)
                    {
                        s32Ret = CVI_VDEC_SendStream(stVdecCfg.s32ChnNum, &stStream, 1000);
                        if (s32Ret == CVI_SUCCESS)
                        {
                            break;
                        }
                        retry_count++;
                        usleep(1000);
                    }

                    if (s32Ret != CVI_SUCCESS)
                    {
                        printf("Error sending stream to VDEC after retries: %x\n", s32Ret);
                        av_packet_unref(&pkt);
                        continue;
                    }

                    s32Ret = CVI_VDEC_GetFrame(stVdecCfg.s32ChnNum, &stFrameInfo, 1000);
                    if (s32Ret == CVI_SUCCESS)
                    {
                        if (avFrameCounter % 100 == 0)
                        {
                            printf("Got frame %d: W=%d, H=%d, stride=%d\n",
                                   avFrameCounter,
                                   stFrameInfo.stVFrame.u32Width,
                                   stFrameInfo.stVFrame.u32Height,
                                   stFrameInfo.stVFrame.u32Stride[0]);
                        }

                        if (CVI_VDEC_ReleaseFrame(stVdecCfg.s32ChnNum, &stFrameInfo) != CVI_SUCCESS)
                        {
                            printf("Warning: Failed to release frame\n");
                        }
                    }
                    else if (s32Ret != CVI_ERR_VDEC_BUF_EMPTY)
                    {
                        printf("Failed to get frame from VDEC, error: 0x%x\n", s32Ret);
                    }

                    avFrameCounter++;
                }
                av_packet_unref(&pkt);
            }

            VDEC_STREAM_S end_stream;
            memset(&end_stream, 0, sizeof(end_stream));
            end_stream.bEndOfStream = CVI_TRUE;
            CVI_VDEC_SendStream(stVdecCfg.s32ChnNum, &end_stream, -1);

            av_packet_unref(&pkt);
            avcodec_free_context(&pCodecCtx);
            avformat_close_input(&pFormatCtx);
            retry++;
            if(isEnd){
                break;
            }
        }
    }

    VDEC_DESTROY(&stVdecCfg);
    SYS_DESTROY();
    return s32Ret;
}