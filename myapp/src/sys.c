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

#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_buffer.h"
#include "sys.h"
#include "config.h"

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

CVI_S32 SYS_INIT()
{

	COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;
	VB_CONFIG_S stVbConf;
	CVI_U32 u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

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

	return s32Ret;
}

CVI_VOID SYS_DESTROY(CVI_VOID)
{
	CVI_SYS_Exit();
	CVI_VB_Exit();
}