#include "Multihack.h"
#include <iostream>
#include "Offets.h"

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
			uintptr_t glowManager = process.ProcRead<uintptr_t>(moduleBase + offsets::esp::dwGlowObjectManager);

			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + offsets::dwLocalPlayer);
			int team = process.ProcRead<int>(lPlayer + offsets::m_iTeamNum);
			
			// looping through entity list (64 since max number of players on a server is 64)
			for (int i = 1; i < 64; ++i)
			{
				uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + offsets::dwEntityList + i * 0x10);
				int health = process.ProcRead<int>(entity + offsets::m_iHealth);
				int entityTeam = process.ProcRead<int>(entity + offsets::m_iTeamNum);
				// spectator
				int isDormaint = process.ProcRead<int>(entity + offsets::m_bDormant);

				// skip if entity is not found or a spectator or dead
				if (entity && !isDormaint && health > 0 && health < 101)
				{
					int glowIndex = process.ProcRead<int>(entity + offsets::esp::m_iGlowIndex);
					Glow glowStat = process.ProcRead<Glow>(glowManager + (glowIndex * 0x38));
					if (entityTeam == team)
					{
						glowStat.blue = 1;
						glowStat.green = 0.5;
					}
					else
					{
						bool defusing = process.ProcRead<bool>(entity + offsets::esp::m_bIsDefusing);
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
				uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + offsets::dwEntityList + i * 0x10);
				if (entity)
					process.ProcWrite(entity + offsets::radar::m_bSpotted, true);
			}
		}
		Sleep(100);
	}
}

void Multihack::AimBot()
{
	ClientUpdate();
	uintptr_t enemyToAim;
	std::thread findClosest([this, &enemyToAim]() 
	{
		while (true)
		{
			enemyToAim = ClosestEnemy();
			Sleep(1);
		}
	});
	findClosest.detach();

	while (true)
	{
		if (GetAsyncKeyState((int)hKeys::AIMBOT) & 1)
		{
			enabled[hID::AIMBOT] = !enabled[hID::AIMBOT];
			if (enabled[hID::AIMBOT])
				std::cout << "Aimbot enabled\n";
			else
				std::cout << "Aimbot disabled\n";
		}
		if (enabled[hID::AIMBOT])
		{
			bool spotted = process.ProcRead<bool>(enemyToAim + offsets::radar::m_bSpotted);

			if (spotted)
			{
				uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + offsets::dwLocalPlayer);
				Vector3 playerPos = process.ProcRead<Vector3>(lPlayer + offsets::aimBot::m_vecOrigin);
				Vector3 entPos = process.ProcRead<Vector3>(enemyToAim + offsets::aimBot::m_vecOrigin);
				Vector3 angles = getAngles(playerPos, entPos);

				Vector3 punchAngles = process.ProcRead<Vector3>(enemyToAim + offsets::noRecoil::m_aimPunchAngle);
				angles = angles - punchAngles * 2;
				process.ProcWrite<Vector3>(moduleBase + offsets::aimBot::dwClientState_ViewAngles, angles);
			}
		}
	}
}

uintptr_t Multihack::ClosestEnemy() const
{
	uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + offsets::dwLocalPlayer);
	int team = process.ProcRead<int>(lPlayer + offsets::m_iTeamNum);
	uintptr_t closest;

	for (int i = 1; i < 64; ++i)
	{
		uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + offsets::dwEntityList + i * 0x10);
		int health = process.ProcRead<int>(entity + offsets::m_iHealth);
		int entityTeam = process.ProcRead<int>(entity + offsets::m_iTeamNum);
		// spectator
		int isDormaint = process.ProcRead<int>(entity + offsets::m_bDormant);
		
		double lowestDist = max(screenX, screenY);
		if (entity && !isDormaint && health > 0 && health < 101)
		{
			// number 8 states for head bone id
			boneMatrix_t boneHead = process.ProcRead<boneMatrix_t>(entity + offsets::aimBot::m_dwBoneMatrix + sizeof(boneHead) * 8);
			view_matrix_t mat = process.ProcRead<view_matrix_t>(moduleBase + offsets::dwViewMatrix);
			Vector3 head{ boneHead.x, boneHead.y, boneHead.z };
			head = WorldToScreen(head, mat);
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

Vector3 Multihack::getAngles(Vector3 cur, Vector3 dest) const
{
	Vector3 angles;

	Vector3 delta = cur - dest;
	double len = delta.len2();

	// convert from radian to angles multiplying by 180 / PI
	angles.y = atan(delta.y / delta.x) * 180 / M_PI;
	angles.x = acos(delta.z / len) * 180 / M_PI;

	if (angles.x >= 0)
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