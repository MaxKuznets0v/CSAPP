#pragma once

#include <Windows.h>

class ProcessHandler
{
public:
	ProcessHandler(const char*);
	~ProcessHandler();
	DWORD GetProcID() const;
	HANDLE GetHandle() const;
	uintptr_t GetModule(const char*);
	template <typename T>
	T ProcRead(DWORD);
	template <typename T>
	void ProcWrite(DWORD, T);
private:
	HANDLE hProc;
	DWORD procID;
};

template <typename T>
T ProcessHandler::ProcRead(DWORD procAddress)
{
	// Reading from process memory
	T buff;
	ReadProcessMemory(hProc, (LPCVOID)procAddress, &buff, sizeof(T), NULL);
	return buff;
}

template <typename T>
void ProcessHandler::ProcWrite(DWORD procAddress, T val)
{
	// Writing to process memory
	WriteProcessMemory(hProc, (LPVOID)procAddress, &val, sizeof(T), NULL);
}