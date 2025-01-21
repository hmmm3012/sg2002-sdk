#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "cvi_comm_venc.h"
#include "cvi_venc.h"
#include "venc_init.h"

static CVI_S32 SAMPLE_COMM_VENC_GetDataType(PAYLOAD_TYPE_E enType, VENC_PACK_S *ppack)
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

CVI_S32 SAMPLE_COMM_VENC_SaveStream(PAYLOAD_TYPE_E enType,
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

        dataType = SAMPLE_COMM_VENC_GetDataType(enType, ppack);
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