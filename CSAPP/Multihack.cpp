#include "Multihack.h"
#include <thread>
#include <iostream>
#include "Offets.h"
#include "config.h"

Multihack::Multihack() : process(ProcessHandler("csgo.exe")) 
{
	std::thread updater([this]()
	{
		// updating client.dll file (in case game changes)
		while (true)
		{
			ClientUpdate();
			Sleep(2000);
		}
	});
	updater.detach();
	Options();
}

void Multihack::Bhop()
{
	// needed to prevent a jump when hotkey is pressed
	bool firstLaunch = true;
	std::cout << "Bhop enabled\n";
	ClientUpdate();
	while (!GetAsyncKeyState((int)hKeys::BHOP))
	{
		int flag = 0;
		
		// getting local player address (first in the entity list)
		uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + offsets::dwEntityList);

		// getting a state flag
		flag = process.ProcRead<int>(lPlayer + offsets::bunnyHop::m_fFlags);

		// when chat or console is opened stop bhoping
		int mouseEnabled = process.ProcRead<int>(moduleBase + offsets::bunnyHop::dwMouseEnable);

		int action = 0;
		if (GetAsyncKeyState(VK_SPACE) && flag & (1 << 0) && mouseEnabled & 1)
			action = 1;
		if (!firstLaunch)
			process.ProcWrite<int>(moduleBase + offsets::bunnyHop::dwForceJump, action);
		else
			firstLaunch = false;
		Sleep(20);
	}
	enabled[hID::BHOP] = false;
	std::cout << "Bhop disabled\n";
}

void Multihack::RadarHack()
{
	std::cout << "Radar hack enabled\n";
	ClientUpdate();
	while (!GetAsyncKeyState((int)hKeys::RADAR_HACK))
	{
		// looping through entity list (64 since max number of players on a server is 64)
		for (int i = 0; i < 64; ++i)
		{
			DWORD entity = process.ProcRead<DWORD>(moduleBase + offsets::dwEntityList + i * 0x10);
			if (entity)
				process.ProcWrite(entity + offsets::radar::m_bSpotted, true);
		}
		Sleep(50);
	}
	enabled[hID::RADAR_HACK] = false;
	std::cout << "Radar hack disabled\n";
}

void Multihack::ClientUpdate()
{
	moduleBase = process.GetModule("client.dll");
}

void Multihack::Options()
{
	std::thread bhopper;
	std::thread radarh;
	while (!GetAsyncKeyState(VK_END))
	{
		if (GetAsyncKeyState((int)hKeys::BHOP) && !enabled[hID::BHOP])
		{
			enabled[hID::BHOP] = true;
			bhopper = move(std::thread(&Multihack::Bhop, this));
			bhopper.detach();
		}
		else if (GetAsyncKeyState((int)hKeys::RADAR_HACK) && !enabled[hID::RADAR_HACK])
		{
			enabled[hID::RADAR_HACK] = true;
			radarh = move(std::thread(&Multihack::RadarHack, this));
			radarh.detach();
		}
		Sleep(1000);
	}
}