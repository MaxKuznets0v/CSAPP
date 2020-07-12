#pragma once
#include "MemScan.h"
#include "config.h"
#include <thread>


class Multihack
{
public:
	Multihack();
private:
	ProcessHandler process;
	DWORD moduleBase;
	// multicheat activity
	bool active;
	// cheats availability
	bool enabled[cNums];
	std::thread cheatTreads[cNums];
	void ESP();
	void Bhop();
	void RadarHack();
	void ClientUpdate();
	void Options();
	void StopAll();
};