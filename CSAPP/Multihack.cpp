#include "Multihack.h"
#include <iostream>
//#include "Offets.h"
#include "csgo.hpp"
using namespace hazedumper;
using namespace netvars;
using namespace signatures;

Multihack::Multihack() : process(ProcessHandler("csgo.exe"))
{
	active = true;
	//csgoDC = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive"));
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
	ClientUpdate();
	while (true)
	{
		if (GetAsyncKeyState((int)hKeys::ESP) & 1)
		{
			enabled[hID::ESP] = !enabled[hID::ESP];
			if (enabled[hID::ESP])
				std::cout << "ESP enabled\n";
			else
				std::cout << "ESP disabled\n";
		}
		if (enabled[hID::ESP])
		{
			uintptr_t glowManager = process.ProcRead<uintptr_t>(moduleBase + dwGlowObjectManager);

			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
			int team = process.ProcRead<int>(lPlayer + m_iTeamNum);
			
			// looping through entity list (64 since max number of players on a server is 64)
			for (int i = 1; i < 64; ++i)
			{
				uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
				int health = process.ProcRead<int>(entity + m_iHealth);
				int entityTeam = process.ProcRead<int>(entity + m_iTeamNum);
				// spectator
				int isDormant = process.ProcRead<int>(entity + m_bDormant);

				// skip if entity is not found or a spectator or dead
				if (entity && !isDormant && health > 0 && health < 101)
				{
					int glowIndex = process.ProcRead<int>(entity + m_iGlowIndex);
					Glow glowStat = process.ProcRead<Glow>(glowManager + (glowIndex * 0x38));
					if (entityTeam == team)
					{
						glowStat.blue = 1;
						glowStat.green = 0.5;
					}
					else
					{
						bool defusing = process.ProcRead<bool>(entity + m_bIsDefusing);
						if (defusing)
						{
							// defusing player will be white
							glowStat.red = 1;
							glowStat.green = 1;
							glowStat.blue = 1;
						}
						else
						{
							// glow color changes accordigly to player's health
							glowStat.red = health * -0.01 + 1;
							glowStat.green = health * 0.01;
						}
					}

					glowStat.alpha = 0.8;
					//glowStat.glowStyle = 1;
					glowStat.renderOccluded = true;
					glowStat.renderNonOccluded = false;
					process.ProcWrite<Glow>(glowManager + (glowIndex * 0x38), glowStat);
				}
			}
		}
		Sleep(1);
	}
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
			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwEntityList);

			// getting a state flag
			flag = process.ProcRead<int>(lPlayer + m_fFlags);

			// when chat or console is opened stop bhoping
			int mouseEnabled = process.ProcRead<int>(moduleBase + dwMouseEnable);

			int action = 0;
			if (GetAsyncKeyState(VK_SPACE) && flag & (1 << 0) && mouseEnabled & 1)
				action = 1;

			if (!firstLaunch)
				process.ProcWrite<int>(moduleBase + dwForceJump, action);
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
				uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
				if (entity)
					process.ProcWrite(entity + m_bSpotted, true);
			}
		}
		Sleep(100);
	}
}

void Multihack::AimBot()
{
	std::thread findClosest;
	while (true)
	{
		if (GetAsyncKeyState((int)hKeys::AIMBOT) & 1)
		{
			enabled[hID::AIMBOT] = !enabled[hID::AIMBOT];
			if (enabled[hID::AIMBOT])
				std::cout << "Aimbot enabled\n";
			else
			{
				std::cout << "Aimbot disabled\n";
				findClosest.~thread();
			}
		}
		if (enabled[hID::AIMBOT])
		{
			ClientUpdate();
			uintptr_t enemyToAim = 0;
			findClosest = std::thread(([this, &enemyToAim]()
			{
				while (true)
				{
					enemyToAim = ClosestEnemy();
					Sleep(1);
				}
			}));
			findClosest.detach();

			bool spotted = process.ProcRead<bool>(enemyToAim + m_bSpotted);

			if (spotted && enemyToAim && GetAsyncKeyState(VK_LBUTTON))
			{
				uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
				Vector3 playerPos = process.ProcRead<Vector3>(lPlayer + m_vecOrigin);
				Vector3 entPos = process.ProcRead<Vector3>(enemyToAim + m_vecOrigin);
				
				Vector3 angles = getAngles(playerPos, entPos);
				Vector3 punchAngles = process.ProcRead<Vector3>(lPlayer + m_aimPunchAngle);
				angles = angles - punchAngles * 2;

				uintptr_t engine = process.GetModule("engine.dll");
				uintptr_t clientState = process.ProcRead<uintptr_t>(engine + dwClientState);
				Vector3 curAngles = process.ProcRead<Vector3>(clientState + dwClientState_ViewAngles);
				Vector3 delta = angles - curAngles;
				process.ProcWrite<Vector3>(clientState + dwClientState_ViewAngles, angles + delta / 20);
			}
		}
	}
}

uintptr_t Multihack::ClosestEnemy()
{
	uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
	int team = process.ProcRead<int>(lPlayer + m_iTeamNum);
	uintptr_t closest = 0;
	double lowestDist = 999999;

	for (int i = 1; i < 64; ++i)
	{
		uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
		int health = process.ProcRead<int>(entity + m_iHealth);
		int entityTeam = process.ProcRead<int>(entity + m_iTeamNum);
		// spectator
		int isDormant = process.ProcRead<int>(entity + m_bDormant);
		
		//double lowestDist = max(screenX, screenY);
		
		if (entity && entityTeam != team && !isDormant && health > 0 && health < 101)
		{
			// number 8 states for head bone id
			uintptr_t boneBase = process.ProcRead<uintptr_t>(entity + m_dwBoneMatrix);
			boneMatrix_t boneHead = process.ProcRead<boneMatrix_t>(boneBase + sizeof(boneHead) * 8);
			Vector3 head{ boneHead.x, boneHead.y, boneHead.z };
			view_matrix_t mat = process.ProcRead<view_matrix_t>(moduleBase + dwViewMatrix);
			head = WorldToScreen(head, mat);
			if (head == Vector3{ -1, -1, -1 })
				continue;
	
			double delta = sqrt(pow(screenX / 2 - head.x, 2) + pow(screenY / 2 - head.y, 2));

			if (delta < lowestDist)
			{
				lowestDist = delta;
				closest = entity;
			}
		}
	}
	return closest;
}

void Multihack::DrawLine(float StartX, float StartY, float EndX, float EndY){ //This function is optional for debugging.
	int a, b = 0;
	HPEN hOPen;
	HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	HDC hdc = GetDC(hwnd);
	HPEN hNPen = CreatePen(PS_SOLID, 2, 0x0000FF /*red*/);
	hOPen = (HPEN)SelectObject(hdc, hNPen);
	MoveToEx(hdc, StartX, StartY, NULL); //start of line
	a = LineTo(hdc, EndX, EndY); //end of line
	DeleteObject(SelectObject(hdc, hOPen));
}

Vector3 Multihack::getAngles(Vector3 cur, Vector3 dest) const
{
	Vector3 angles;
	angles.z = 0;

	Vector3 delta = cur - dest;
	double len = delta.len2();

	// convert from radian to angles multiplying by 180 / PI
	angles.x = asinf(delta.z / len) * (180 / M_PI);
	angles.y = atanf(delta.y / delta.x) * (180.f / M_PI);

	if (delta.x >= 0)
		angles.y += 180;

	return angles;
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
	cheatTreads[hID::AIMBOT] = std::thread(&Multihack::AimBot, this);
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

//void Multihack::HealthBar(uintptr_t entity, float health)
//{
//	// getting veiw matrix for view transformation
//	view_matrix_t veiwMat = process.ProcRead<view_matrix_t>(moduleBase + offsets::esp::dwViewMatrix);
//	// getting player position
//	Vector3 pos = process.ProcRead<Vector3>(entity + offsets::esp::m_vecOrigin);
//	Vector3 head{ pos.x, pos.y, pos.z + 75.f };
//	// transforming 3D global coordinates into 2D screen coordinates
//	Vector3 screenpos = WorldToScreen(pos, veiwMat);
//	Vector3 screenhead = WorldToScreen(head, veiwMat);
//	float height = screenhead.y - screenpos.y;
//	float width = height / 2.4f;;
//	RECT bar{ screenhead.x - width / 2 - 10, screenhead.y, screenpos.x - width / 2 - 5, screenpos.y };
//	DrawBar(bar, health);
//}
//
//void Multihack::DrawBar(RECT bar, float health)
//{
//	HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
//	FillRect(csgoDC, &bar, brush);
//	DeleteObject(brush);
//}