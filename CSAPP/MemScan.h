#pragma once

#include <Windows.h>

class ProcessHandler
{
public:
	ProcessHandler(const char*);
	~ProcessHandler();
	uintptr_t GetProcID() const;
	HANDLE GetHandle() const;
	uintptr_t GetModule(const char*) const;
	template <typename T>
	T ProcRead(uintptr_t) const;
	template <typename T>
	void ProcWrite(uintptr_t, T);
private:
	HANDLE hProc;
	uintptr_t procID;
};

template <typename T>
T ProcessHandler::ProcRead(uintptr_t procAddress) const
{
	// Reading from process memory
	T buff;
	ReadProcessMemory(hProc, (LPCVOID)procAddress, &buff, sizeof(T), NULL);
	return buff;
}

template <typename T>
void ProcessHandler::ProcWrite(uintptr_t procAddress, T val)
{
	// Writing to process memory
	WriteProcessMemory(hProc, (LPVOID)procAddress, &val, sizeof(T), NULL);
}