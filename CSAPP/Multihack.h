#pragma once
#include "MemScan.h"
#include "Offets.h"
#include <iostream>
#include "config.h"

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