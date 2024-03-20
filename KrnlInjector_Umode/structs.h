#pragma once

#include <string>

#include "injectorStatusCodes.h"

#define SELECTBYPID				0x0L
#define SELECTBYNAME			0x1L

#define CONFIG_HIJACK_HANDLE	0x1LL

struct errorLog {

	INJSTATUS	code;
	std::string caller;

};


struct config {

	
	int selectionMode; // 0 = selectByPid; 1 = selectByName
	bool hijackHandle; 

};


struct selectedProcess
{

	char* processImage;
	char* pid;

};