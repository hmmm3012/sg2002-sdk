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
#include "sys_init.h"

CVI_VOID HandleSig(CVI_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (SIGINT == signo || SIGTERM == signo)
	{
		printf("Program termination abnormally\n");
	}
	exit(1);
}

int main(int argc, char **argv)
{
	CVI_S32 s32Ret = CVI_FAILURE;
	printf("Usage: ./myapp input_h264 model_path \n");

	signal(SIGINT, HandleSig);
	signal(SIGTERM, HandleSig);
}