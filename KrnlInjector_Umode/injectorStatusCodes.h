#pragma once

typedef long INJSTATUS;

// default status
#define INJ_CLIENT_NOT_INITIALIZED	   -0x1L

#define INJ_SUCCESS						0x0L

#define INJ_UNSUCCESSFULL				0x1L

// Errors from windows functions which log the error as last error
#define INJ_WIN_ERROR					0x2L

#define INJ_CREATE_DRIVER_DEVICE_ERROR	0x3L

#define INJ_MEMORY_ALLOCATION_ERROR		0x4L

#define INJ_FILE_NOT_ACCESSIBLE			0x5L

#define INJ_INVALID_FILE_TYPE			0x6L

#define INJ_PID_NOT_FOUND				0x7L

#define INJ_CLIENT_INITIALIZED			0x8L

#define INJ_NOT_INJECTED				0x9L

