#pragma once

#pragma warning(disable: 4996)
#pragma warning(disable: 4267)
#pragma warning(disable: 4005)

#include <Windows.h>
#include <ntstatus.h>
#include <iostream>
#include <string>
#include <vector>
#include <psapi.h>
#include <chrono>
#include <WinDNS.h>

#include "structs.h"
#include "injectorStatusCodes.h"
#include "imgui/imgui.h"

#define WBINPATH_SIZE			256
#define BINPATH_SIZE			256
#define TARGETPROCID_SIZE		256
#define TARGETPROCIMAGE_SIZE	256

class Injector
{
	
public:

	bool									testmode			= true;
	bool									timerRunning		= false;
	static inline wchar_t*					wBinPath;				// "\\??\\C:\\Users\\sxcca\\Desktop\\cs2internal_v1.0.dll"
	static inline char*						binPath;
	static inline selectedProcess			targetProcess;
	HANDLE									driverDevice		= nullptr;
	LPVOID									lpInBuffer			= nullptr;
	LPVOID									lpOutBuffer			= nullptr;
	DWORD									nInBufferSize		= 0;
	DWORD									nOutBufferSize		= 0;
	LPDWORD									lpBytesReturned		= nullptr;
	LPOVERLAPPED							lpOverlapped		= nullptr;
	NTSTATUS								lastNtStatus		= STATUS_SUCCESS;
	INJSTATUS								initStatus			= INJ_CLIENT_NOT_INITIALIZED;
	INJSTATUS								lastInjectionStatus = INJ_NOT_INJECTED;
	std::vector <errorLog>					loggedErrors		= {};
	config									configuration;
	std::chrono::steady_clock::time_point	injectionStatusTimer;


	Injector();
	~Injector();

	bool				initialize();
	bool				inject();
	bool				validateBinPath();
	bool				validateTargetPid();
	bool				exitInjector();
	bool				connectToDriver();
	bool				loadConfig();    
	void				logError(INJSTATUS errorType, std::string caller);
	void				errorAbort();
	static int			getProcIdByImageName(ImGuiInputTextCallbackData* data);
	static int			testCallback(ImGuiInputTextCallbackData* data);
	static int			getProcessNameByPid(ImGuiInputTextCallbackData* data);
	long				getLastErrorCode();
	std::string			getLastErrorAsString();
	static uintptr_t	getModuleBaseAddress(DWORD procId, const wchar_t* modName);
	QWORD				getConfigAsQWORD();

};