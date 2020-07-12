#include "Multihack.h"
#include <iostream>
#include "Offets.h"


Multihack::Multihack() : process(ProcessHandler("csgo.exe"))
{
	active = true;
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

void Multihack::StopAll()
{
	// killing all the threads
	for (int i = 0; i < cNums; ++i)
	{
		cheatTreads[i].~thread();
		enabled[i] = false;
	}
}

void Multihack::ESP()
{
	std::cout << "ESP enabled\n";
	ClientUpdate();
	while (GetKeyState((int)hKeys::RADAR_HACK) & 1)
	{
		// looping through entity list (64 since max number of players on a server is 64)
		for (int i = 0; i < 64; ++i)
		{
			DWORD entity = process.ProcRead<DWORD>(moduleBase + offsets::dwEntityList + i * 0x10);

		}
		Sleep(50);
	}

	enabled[hID::ESP] = false;
	std::cout << "ESP disabled\n";
}

void Multihack::Bhop()
{
	// needed to prevent a jump when hotkey is pressed
	bool firstLaunch = true; 
	ClientUpdate();

	while (true)
	{
		if (GetAsyncKeyState((int)hKeys::BHOP) & 1)
		{
			enabled[hID::BHOP] = !enabled[hID::BHOP];
			if (enabled[hID::BHOP])
			{
				std::cout << "Bhop enabled\n";
				firstLaunch = true;
			}
			else
				std::cout << "Bhop disabled\n";
		}
		if (enabled[hID::BHOP])
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
		}
		Sleep(20);
	}
}

void Multihack::RadarHack()
{
	ClientUpdate();
	while (true)
	{
		if (GetAsyncKeyState((int)hKeys::RADAR_HACK) & 1)
		{
			enabled[hID::RADAR_HACK] = !enabled[hID::RADAR_HACK];
			if (enabled[hID::RADAR_HACK])
				std::cout << "Radar hack enabled\n";
			else
				std::cout << "Radar hack disabled\n";
		}
		if (enabled[hID::RADAR_HACK])
		{
			// looping through entity list (64 since max number of players on a server is 64)
			for (int i = 0; i < 64; ++i)
			{
				DWORD entity = process.ProcRead<DWORD>(moduleBase + offsets::dwEntityList + i * 0x10);
				if (entity)
					process.ProcWrite(entity + offsets::radar::m_bSpotted, true);
			}
		}
		Sleep(100);
	}
}

void Multihack::ClientUpdate()
{
	moduleBase = process.GetModule("client.dll");
}

void Multihack::Options()
{
	cheatTreads[hID::BHOP] = std::thread(&Multihack::Bhop, this);
	cheatTreads[hID::RADAR_HACK] = std::thread(&Multihack::RadarHack, this);
	cheatTreads[hID::ESP] = std::thread(&Multihack::ESP, this);
	for (int i = 0; i < cNums; ++i)
		cheatTreads[i].detach();

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1)
		{
			active = !active;
			if (active)
				std::cout << "Multihack enabled\n";
			else
			{
				std::cout << "Multihack disabled\n";
				StopAll();
			}
		}
			
		Sleep(1000);
	}
}