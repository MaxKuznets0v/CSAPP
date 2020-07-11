#pragma once
#include "MemScan.h"

class Multihack
{
public:
	Multihack();
private:
	ProcessHandler process;
	DWORD moduleBase;
	void Bhop();
	void RadarHack();
	void ClientUpdate();
	void Options();
};