#ifndef __MISC_H__
#define __MISC_H__

// #include "cvi_common.h"

#define PAUSE()                                                             \
	do                                                                      \
	{                                                                       \
		printf("---------------press Enter key to exit!---------------\n"); \
		getchar();                                                          \
	} while (0)

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

// PAYLOAD_TYPE_E find_file_type(const char *filename, int filelen);

#endif /* __MISC_H__ */
