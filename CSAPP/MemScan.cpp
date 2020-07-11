#include "MemScan.h"
#include <TlHelp32.h>
#include <iostream>

ProcessHandler::ProcessHandler(const char* procName)
{
	hProc = NULL;
	procID = NULL;
	// creates a handle to all processes
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	// process struct 
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(procEntry);
	if (hSnap != INVALID_HANDLE_VALUE) // if handle is created properly
	{
		while (Process32Next(hSnap, &procEntry))
		{
			// searching for a process in a snapshot
			if (!strcmp((char*)procEntry.szExeFile, procName))
			{
				procID = procEntry.th32ProcessID;
				CloseHandle(hSnap);
				// creating handle for current process
				hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

				return;
			}
		}
		std::cout << "Can't find the process in a snapshot!\n";
		CloseHandle(hSnap);
	}
	else
		std::cout << "Can't take processes' snapshot!\n";
}

uintptr_t ProcessHandler::GetModule(const char* modName)
{
	// you should found process first
	if (procID == NULL)
		throw std::runtime_error("Process wasn't found!\n");

	// all the same as with process
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(modEntry);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		while (Module32Next(hSnap, &modEntry))
		{
			if (!strcmp((char*)modEntry.szModule, modName))
			{
				CloseHandle(hSnap);
				return (uintptr_t)modEntry.modBaseAddr;
			}
		}
		std::cout << "Can't find the module in a snapshot!\n";
		CloseHandle(hSnap);
		return uintptr_t();
	}
	else
	{
		std::cout << "Can't take modules' snapshot!\n";
		return uintptr_t();
	}
}

ProcessHandler::~ProcessHandler()
{
	// closing process handle
	CloseHandle(hProc);
}

DWORD ProcessHandler::GetProcID() const
{
	// returning process ID
	return procID;
}

HANDLE ProcessHandler::GetHandle() const
{
	return hProc;
}
