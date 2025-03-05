#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "cvi_vdec.h"
#include "cvi_vpss.h"
#include "cvi_venc.h"

#include "vdec.h"
#include "sys.h"
#include "misc.h"

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

// int main(int argc, char *argv[])
int main()
{
    // if (argc != 2)
    // {
    //     Print_Usage(argv[0]);
    //     return -1;
    // }

    // CVI_VOID *s_h264file[MAX_VDEC_NUM];
    // s_h264file[0] = argv[1]; // Input H.264 file

    CVI_S32 s32Ret = CVI_SUCCESS;
    signal(SIGINT, VDECVO_HandleSig);
    signal(SIGTERM, VDECVO_HandleSig);

    // s32Ret = VDEC_BIND_VPSS_BIND_VENC(1, s_h264file);

    // Initialize SYSTEM
    s32Ret = SYS_INIT();
    if (s32Ret != CVI_SUCCESS)
    {
        printf("SYS_INIT failed with %#x\n", s32Ret);
        return -1;
    }

    // Initialize VDEC
    VDEC_CONFIG_S stVdecCfg = {0};
    s32Ret = VDEC_INIT(1, &stVdecCfg);
    if (s32Ret != CVI_SUCCESS)
    {
        printf("VDEC_INIT failed with %#x\n", s32Ret);
        return -1;
    }

    int retry = 0;
    while (1)
    {
        AVDictionary *opts = NULL; 
        av_dict_set(&opts, "rw_timeout", "3000000", 0);
        av_dict_set(&opts, "stimeout", "3000000", 0);
        av_dict_set(&opts, "rtsp_transport", "tcp", 0);

        int avFrameCounter = 0;
        VIDEO_FRAME_INFO_S stFrameInfo;
        VDEC_STREAM_S stStream;

        while (1)
        {
            AVFormatContext *pFormatCtx = avformat_alloc_context();
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

            int videoStreamIndex = -1;
            for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
            {
                if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    videoStreamIndex = i;
                    break;
                }
            }
            //
            // AVRational frameRate = av_guess_frame_rate(pFormatCtx, pFormatCtx->streams[videoStreamIndex], nullptr);

            // int fps = (int)(frameRate.num / frameRate.den);

            AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
            if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStreamIndex]->codecpar) < 0)
            {
                avformat_close_input(&pFormatCtx);
                avcodec_close(pCodecCtx);
                retry++;
                continue;
            }

            const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
            if (pCodec == NULL)
            {
                avformat_close_input(&pFormatCtx);
                avcodec_close(pCodecCtx);
                retry++;
                continue;
            }
            if (avcodec_open2(pCodecCtx, pCodec, &opts) < 0)
            {
                avcodec_close(pCodecCtx);
                avformat_close_input(&pFormatCtx);
                retry++;
                continue;
            }
            // auto start_time = std::chrono::high_resolution_clock::now();
            AVPacket pkt;
            // int width = pCodecCtx->width;
            // int height = pCodecCtx->height;
            // auto conversion = sws_getContext(width, height, AV_PIX_FMT_YUV420P,
            //                                  width, height, AV_PIX_FMT_BGR24,
            //                                  SWS_BILINEAR, nullptr, nullptr, nullptr);
            while (av_read_frame(pFormatCtx, &pkt) >= 0)
            {
                if (pkt.stream_index == videoStreamIndex)
                {
                    // Prepare VDEC stream
                    memset(&stStream, 0, sizeof(stStream));
                    stStream.u64PTS = pkt.pts;
                    stStream.pu8Addr = pkt.data;
                    stStream.u32Len = pkt.size;
                    stStream.bEndOfFrame = CVI_TRUE;
                    stStream.bEndOfStream = CVI_FALSE;

                    // Send stream with retry
                    int retry_count = 0;
                    while (retry_count < 3)
                    {
                        s32Ret = CVI_VDEC_SendStream(0, &stStream, 1000);
                        if (s32Ret == CVI_SUCCESS)
                        {
                            break;
                        }
                        else if (s32Ret == CVI_ERR_VDEC_BUSY)
                        {
                            usleep(10000);
                            retry_count++;
                            continue;
                        }
                        else
                        {
                            printf("Error sending stream to VDEC: 0x%x\n", s32Ret);
                            goto CLEANUP;
                        }
                    }

                    // Try to get decoded frame
                    s32Ret = CVI_VDEC_GetFrame(0, &stFrameInfo, 100);
                    if (s32Ret == CVI_SUCCESS)
                    {
                        printf("Got frame %d: %dx%d\n",
                               avFrameCounter,
                               stFrameInfo.stVFrame.u32Width,
                               stFrameInfo.stVFrame.u32Height);

                        CVI_VDEC_ReleaseFrame(0, &stFrameInfo);
                        avFrameCounter++;
                    }
                }
                av_packet_unref(&pkt);
            }

        CLEANUP:
            // Signal end of stream
            memset(&stStream, 0, sizeof(VDEC_STREAM_S));
            stStream.bEndOfStream = CVI_TRUE;
            CVI_VDEC_SendStream(0, &stStream, -1);

            // Cleanup FFmpeg
            av_packet_unref(&pkt);
            avcodec_free_context(&pCodecCtx);
            avformat_close_input(&pFormatCtx);

            if (s32Ret != CVI_SUCCESS)
            {
                sleep(1);
            }
            retry++;
        }
    }
    // Destroy VDEC
    VDEC_DESTROY(&stVdecCfg);
    // Destroy SYSTEM
    SYS_DESTROY();
    return s32Ret;
}