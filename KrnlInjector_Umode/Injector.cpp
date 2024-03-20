
#include "Injector.h"
#include "helper.h"


#define IOCTL_ManualMap CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)


Injector::Injector()
{



}


Injector::~Injector()
{

	exitInjector();


}


bool Injector::connectToDriver()
{
	this->driverDevice = CreateFileW(
		L"\\\\.\\krnlinjectorlink",
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM,
		0
	);

	if (this->driverDevice == INVALID_HANDLE_VALUE)
	{
		logError(INJ_CREATE_DRIVER_DEVICE_ERROR, "initialize");
		return false;
	}

	return true;
}


bool Injector::initialize()
{

	bool status = true;

	Injector::wBinPath							= new wchar_t[WBINPATH_SIZE];
	Injector::binPath							= new char[BINPATH_SIZE];
	Injector::targetProcess.pid					= new char[TARGETPROCID_SIZE];
	Injector::targetProcess.processImage		= new char[TARGETPROCIMAGE_SIZE];

	memset(Injector::wBinPath, '\0', WBINPATH_SIZE);
	memset(Injector::binPath, '\0', BINPATH_SIZE);
	memset(Injector::targetProcess.pid, '\0', TARGETPROCID_SIZE);
	memset(Injector::targetProcess.processImage, '\0', TARGETPROCIMAGE_SIZE);

	this->lpInBuffer							= new BYTE[512];
	this->nInBufferSize							= 512;
	
	this->lpOutBuffer							= new BYTE[512];
	this->nOutBufferSize						= 512;

	this->lpOverlapped							= LPOVERLAPPED();
	this->lpBytesReturned						= 0;

	loadConfig();

	this->initStatus							= INJ_SUCCESS;

	return status;
}


bool Injector::loadConfig()
{

	this->configuration.hijackHandle = false;
	this->configuration.selectionMode = SELECTBYNAME;


	return true;
}


QWORD Injector::getConfigAsQWORD()
{
	QWORD config = 0;

	if (this->configuration.hijackHandle)
		config |= CONFIG_HIJACK_HANDLE;

	return config;

}


/*

	this function changes:
		- the (bool) Injector::lastInjectionStatus 
		- logs Error if DeviceIoControl fails


	IMPLEMENT:
		- Send a nother 64bit integer which has the purpose to represent the injection methods

*/

bool Injector::inject() {

	// validate binPath
	 
	if (!validateBinPath())
	{
		this->lastInjectionStatus = INJ_UNSUCCESSFULL;
		return false;
	}
	

	// validate targetPid

	if (!validateTargetPid())
	{
		this->lastInjectionStatus = INJ_UNSUCCESSFULL;
		return false;
	}

	QWORD statusCodes = getConfigAsQWORD();

	char null_terminator = '\0';
	this->nInBufferSize = strlen(Injector::binPath) + 2 + strlen(Injector::targetProcess.pid);
	strcpy((char*)this->lpInBuffer, Injector::binPath);
	strcpy((char*)((char*)this->lpInBuffer + strlen(Injector::binPath)), &null_terminator);
	strcpy((char*)((char*)this->lpInBuffer + strlen(Injector::binPath) + 1), Injector::targetProcess.pid);
	strcpy((char*)((char*)this->lpInBuffer + strlen(Injector::binPath) + 1 + strlen(Injector::targetProcess.pid)), &null_terminator);
	
	*((QWORD*)((BYTE*)this->lpInBuffer + this->nInBufferSize)) = statusCodes;

	this->nInBufferSize += 8;

	if (!connectToDriver())
	{
		this->lastInjectionStatus = INJ_UNSUCCESSFULL;
		return false;
	}

	

	if (!DeviceIoControl(
		this->driverDevice,
		IOCTL_ManualMap,
		this->lpInBuffer,
		this->nInBufferSize, 
		this->lpOutBuffer, 
		this->nOutBufferSize,
		this->lpBytesReturned, 
		this->lpOverlapped))
	{
		logError(INJ_WIN_ERROR, "inject");
		this->lastInjectionStatus = INJ_UNSUCCESSFULL;
		return false;
	}

	this->lastInjectionStatus = INJ_SUCCESS;

	return true;

}



bool Injector::validateBinPath() {

	bool status = true;

	DWORD attributes = GetFileAttributesA(Injector::binPath);

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		logError(INJ_FILE_NOT_ACCESSIBLE, "validateBinPath");
		return false;
	}

	std::string string_binPath(Injector::binPath);

	// Check if it's a regular file and ends with ".dll"
	if (!(attributes & FILE_ATTRIBUTE_DIRECTORY) &&
		string_binPath.length() >= 4 &&
		string_binPath.substr(string_binPath.length() - 4) == ".dll")
	{
		return true;
	}

	logError(INJ_INVALID_FILE_TYPE, "validateBinPath");
	return false;

}



bool Injector::validateTargetPid()
{

	DWORD pid = 0;
	try
	{
		pid = std::stoi(Injector::targetProcess.pid);
	}
	catch (std::out_of_range)
	{
		logError(INJ_PID_NOT_FOUND, "validateTargetPid");
		return false;
	}
	catch (std::invalid_argument)
	{
		logError(INJ_PID_NOT_FOUND, "validateTargetPid");
		return false;
	}

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

	if (processHandle != NULL)
	{
		CloseHandle(processHandle);
		return true;
	}

	logError(INJ_PID_NOT_FOUND, "validateTargetPid");
	return false;

}




bool Injector::exitInjector()
{

	bool status = true;

	// clean up

	CloseHandle(this->driverDevice);

	return status;

}


long Injector::getLastErrorCode()
{

	return this->loggedErrors.at(this->loggedErrors.size() - 1).code;
}



std::string Injector::getLastErrorAsString() {

	switch (this->loggedErrors.at(this->loggedErrors.size() - 1).code)
	{
	case INJ_UNSUCCESSFULL:
		return "Operation was unsuccessful (0x1 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";

	case INJ_WIN_ERROR:
		return "Windows error (0x2 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";

	case INJ_CREATE_DRIVER_DEVICE_ERROR:
		return "Could not create a handle to the driver device (0x3 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";

	case INJ_MEMORY_ALLOCATION_ERROR:
		return "Failed to allocate memory (0x4 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";

	case INJ_FILE_NOT_ACCESSIBLE:
		return "Could not find/access file given in path (0x5 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";

	case INJ_INVALID_FILE_TYPE:
		return "Given file is not a dll (0x6 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";

	case INJ_PID_NOT_FOUND:
		return "Could not find running program with given pid/image (0x7 " + this->loggedErrors.at(this->loggedErrors.size() - 1).caller + ")";
		
	default:
		return "Unknown error ()";
	}

}



void Injector::errorAbort() {




}


/*

When an error occured in a non Injector-function, the Injector function has to call this with the specific INJ-error from the non-Injector fucntion

*/

void Injector::logError(INJSTATUS errorType, std::string caller)
{
	// Retrieve the system error message for the last-error code

	if (errorType == INJ_WIN_ERROR)
	{
		LPCSTR lpszFunction = "main";

		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();


		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		// Display the error message and exit the process

		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"),
			lpszFunction, dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
	}


	this->loggedErrors.push_back( { errorType, caller } );


	// maybe also log in a log file ???

}



uintptr_t Injector::getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	//initialize to zero for error checking
	uintptr_t modBaseAddr = 0;

	//get a handle to a snapshot of all modules
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	//check if it's valid
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		//this struct holds the actual module information
		MODULEENTRY32 modEntry;

		//this is required for the function to work
		modEntry.dwSize = sizeof(modEntry);

		//If a module exists, get it's entry
		if (Module32First(hSnap, &modEntry))
		{
			//loop through the modules
			do
			{
				//compare the module name against ours
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					//copy the base address and break out of the loop
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

				//each iteration we grab the next module entry
			} while (Module32Next(hSnap, &modEntry));
		}
	}

	//free the handle
	CloseHandle(hSnap);
	return modBaseAddr;
}


int Injector::getProcIdByImageName(ImGuiInputTextCallbackData* data)
{

	wchar_t* procName = helper::convertToWideChar(data->Buf);


	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));

		}
	}
	CloseHandle(hSnap);
	
	// Overwrite the buffer for targetProcId
	if (procId != 0)
	{
		memset(Injector::targetProcess.pid, '\0', TARGETPROCID_SIZE);
		std::string procId_string = std::to_string(procId);
		const char* procId_c_str = procId_string.c_str();

		strcpy_s(Injector::targetProcess.pid, TARGETPROCID_SIZE, procId_c_str);

		return INJ_SUCCESS;
	}
	else
	{
		memset(Injector::targetProcess.pid, '\0', TARGETPROCID_SIZE);
	}
	
	return INJ_UNSUCCESSFULL;

}



int Injector::testCallback(ImGuiInputTextCallbackData* data)
{
	std::cout << "TEST\n";
	return 0;
}


int Injector::getProcessNameByPid(ImGuiInputTextCallbackData* data)
{

	const char* procId = (char*)data->Buf;

	std::wstring imageName = L"";

	std::string procId_string(procId);

	if (!helper::isNumber(procId_string) || procId_string == "")
	{
		memset(Injector::targetProcess.processImage, '\0', TARGETPROCIMAGE_SIZE);
		return INJ_UNSUCCESSFULL;
	}
	
	DWORD pid = 0;

	try
	{
		pid = std::stoi(procId_string);
	}
	catch (std::out_of_range)
	{
		return INJ_UNSUCCESSFULL;
	}
	catch (std::invalid_argument)
	{
		return INJ_UNSUCCESSFULL;
	}


	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (hProcess != NULL)
	{
		wchar_t buffer[MAX_PATH];
		if (GetModuleFileNameExW(hProcess, NULL, buffer, MAX_PATH))
		{
			CloseHandle(hProcess);
			std::wstring fullPath(buffer);
			size_t lastSlash = fullPath.find_last_of(L"\\");
			if (lastSlash != std::wstring::npos)
			{
				imageName = fullPath.substr(lastSlash + 1);
			}
		}
		else
			CloseHandle(hProcess);
	}

	// Overwrite procImage
	if (imageName != L"")
	{
		memset(Injector::targetProcess.processImage, '\0', TARGETPROCIMAGE_SIZE);
		std::string imageName_str = helper::wstringToString(imageName);
		const char* imageName_cstr = imageName_str.c_str();
		strcpy_s(Injector::targetProcess.processImage, TARGETPROCIMAGE_SIZE, imageName_cstr);

		return INJ_SUCCESS;
	}
	else
	{
		memset(Injector::targetProcess.processImage, '\0', TARGETPROCIMAGE_SIZE);
	}


	return INJ_UNSUCCESSFULL;
}