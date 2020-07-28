#include "Multihack.h"
#include <iostream>
//#include "Offets.h"
#include "csgo.hpp"
using namespace hazedumper;
using namespace netvars;
using namespace signatures;

Multihack::Multihack() : process(ProcessHandler("csgo.exe"))
{
	if (!process.GetProcID())
	{
		std::cout << "Run CS:GO first!\n";
		return;
	}
	active = true;
	GetSize();
	//HDC csgoDC = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive"));
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

Multihack::~Multihack()
{
	active = false;
	StopAll();
}

void Multihack::StopAll()
{
	// killing all the threads
	for (int i = 0; i < cNums; ++i)
	{
		enabled[i] = false;
		cheatTreads[i].join();
	}
}

void Multihack::ESP()
{
	ClientUpdate();
	while (active)
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

	while (active)
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
	while (active)
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
			for (int i = 1; i < 64; ++i)
			{
				uintptr_t entity = process.ProcRead<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
				if (entity)
					process.ProcWrite<bool>(entity + m_bSpotted, true);
			}
		}
		Sleep(100);
	}
}

void Multihack::AimBot()
{
	ClientUpdate();
	std::thread findClosest;
	uintptr_t enemyToAim = 0;
	// flag that prevensts false activation
	bool first = true;

	while (active)
	{
		if (GetAsyncKeyState((int)hKeys::AIMBOT) & 1)
		{
			enabled[hID::AIMBOT] = !enabled[hID::AIMBOT];
			if (enabled[hID::AIMBOT])
			{
				std::cout << "Aimbot enabled\n";
				first = true;
				findClosest = std::thread(([this, &enemyToAim]()
				{
					while (enabled[hID::AIMBOT])
					{
						enemyToAim = ClosestEnemy();
						Sleep(10);
					}
				}));
			}
			else
			{
				std::cout << "Aimbot disabled\n";
				enemyToAim = 0;
				findClosest.join();
				//findClosest.~thread();
			}
		}
		if (enabled[hID::AIMBOT])
		{
			// getting weapon id
			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
			int id = WeaponID(lPlayer);

			// checking for shooting ability
			bool weaponInHand = true;
			if (id > 40 && id < 60 || id > 67 && id < 526)
				weaponInHand = false;

			if (enemyToAim && GetAsyncKeyState(VK_LBUTTON) && !first && weaponInHand)
			{
				Vector3 playerPos = process.ProcRead<Vector3>(lPlayer + m_vecOrigin);
				Vector3 entPos = process.ProcRead<Vector3>(enemyToAim + m_vecOrigin);

				Vector3 playerHead = getEntHead(lPlayer);
				Vector3 enemyHead = getEntHead(enemyToAim);
				Vector3 angles = getAngles(playerHead, enemyHead);

				// some silly check (not sure if that's needed) 
				if (abs(angles.x) > 360 || abs(angles.y) > 360)
					continue;
				
				Vector3 punchAngles = process.ProcRead<Vector3>(lPlayer + m_aimPunchAngle);
				angles = angles - punchAngles * 2;

				uintptr_t engine = process.GetModule("engine.dll");
				uintptr_t clientState = process.ProcRead<uintptr_t>(engine + dwClientState);

				// Smoothing thing but de-facto is garbage
				/*Vector3 curAngles = process.ProcRead<Vector3>(clientState + dwClientState_ViewAngles);
				Vector3 delta = angles - curAngles;*/

				// needs to avoid camera shaking after killing the enemy
				if (process.ProcRead<int>(enemyToAim + m_iHealth))
					process.ProcWrite<Vector3>(clientState + dwClientState_ViewAngles, angles);
			}
			if (first)
				first = !first;
		}
		Sleep(10);
	}
}

void Multihack::TriggerBot()
{
	ClientUpdate();
	int checkTime = 10;
	bool wasEnabled = false;

	while (active)
	{
		if (GetAsyncKeyState((int)hKeys::TRIGGER) & 1)
		{
			enabled[hID::TRIGGER] = !enabled[hID::TRIGGER];
			if (enabled[hID::TRIGGER])
			{
				std::cout << "Trigger bot enabled\n";
				if (enabled[hID::RECOIL])
					wasEnabled = true;
				enabled[hID::RECOIL] = true;
			}
			else
			{
				std::cout << "Trigger bot disabled\n";
				if (!wasEnabled)
					enabled[hID::RECOIL] = false;
			}
		}
		if (enabled[hID::TRIGGER])
		{
			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
			int crosshairID = process.ProcRead<int>(lPlayer + m_iCrosshairId);
			uintptr_t crosshairEntity = process.ProcRead<uintptr_t>(moduleBase + dwEntityList + (crosshairID - 1) * 0x10);
			int entityHealth = process.ProcRead<int>(crosshairEntity + m_iHealth);
			int crosshairTeam = process.ProcRead<int>(crosshairEntity + m_iTeamNum);
			int localTeam = process.ProcRead<int>(lPlayer + m_iTeamNum);

			int id = WeaponID(lPlayer);

			// weapon validation
			bool validWeapon = true;
			if (id > 40 && id < 60 || id > 67 && id < 526)
				validWeapon = false;

			// cheking for sniper rifle scope
			if (id == 40 || id == 38 || id == 9 || id == 262155 || id == 11)
			{
				bool isScoped = process.ProcRead<bool>(lPlayer + m_bIsScoped);
				if (!isScoped)
					validWeapon = false;
			}

			// for background team is 0 **Actual shooting**
			if (localTeam && entityHealth > 0 && entityHealth < 101 && localTeam != crosshairTeam && 
				crosshairID < 64 && validWeapon)
			{
				process.ProcWrite<int>(moduleBase + dwForceAttack, 5);
				Sleep(1);
				process.ProcWrite<int>(moduleBase + dwForceAttack, 4);
			}
		}

		// changing hack delay
		if (GetAsyncKeyState(VK_DOWN) & 1 && checkTime > 5)
			checkTime -= 5;
		if (GetAsyncKeyState(VK_UP) & 1)
			checkTime += 5;

		Sleep(checkTime);
	}
}

void Multihack::AntiFlash()
{
	ClientUpdate();
	// making sure that antiflash disabled
	uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
	process.ProcWrite<float>(lPlayer + m_flFlashMaxAlpha, 255);

	while (active)
	{
		if (GetAsyncKeyState((int)hKeys::FLASH) & 1)
		{
			enabled[hID::FLASH] = !enabled[hID::FLASH];

			if (enabled[hID::FLASH])
			{
				process.ProcWrite<float>(lPlayer + m_flFlashMaxAlpha, 60);
				std::cout << "Antiflash enabled\n";
			}
			else
			{
				process.ProcWrite<float>(lPlayer + m_flFlashMaxAlpha, 255);
				std::cout << "Antiflash disabled\n";
			}
		}
		Sleep(10);
	}
}

void Multihack::RecoilControl()
{
	ClientUpdate(); 
	uintptr_t engine = process.GetModule("engine.dll");
	Vector3 prevPunch{ 0, 0, 0 };

	while (active)
	{
		if (GetAsyncKeyState((int)hKeys::RECOIL) & 1)
		{
			enabled[hID::RECOIL] = !enabled[hID::RECOIL];
			if (enabled[hID::RECOIL])
				std::cout << "Recoil control enabled\n";
			else
				std::cout << "Recoil control disabled\n";
		}
		if (enabled[hID::RECOIL])
		{
			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
			int shots = process.ProcRead<int>(lPlayer + m_iShotsFired);

			if (shots < 3/*!GetAsyncKeyState(VK_LBUTTON)*/)
			{
				prevPunch = std::move(Vector3(0, 0, 0));
			}
			else
			{
				uintptr_t clientState = process.ProcRead<uintptr_t>(engine + dwClientState);
				Vector3 curAngles = process.ProcRead<Vector3>(clientState + dwClientState_ViewAngles);
				Vector3 punchAngles = process.ProcRead<Vector3>(lPlayer + m_aimPunchAngle);

				curAngles = curAngles + prevPunch - punchAngles * 2;
				NormalizeAngles(curAngles);
				process.ProcWrite<Vector3>(clientState + dwClientState_ViewAngles, curAngles);
				prevPunch = punchAngles * 2;
			}
		}
		Sleep(10);
	}
}

int Multihack::WeaponID(uintptr_t player)
{
	uintptr_t weaponAddress = process.ProcRead<uintptr_t>(player + m_hActiveWeapon) & 0xFFF;
	int m_iBase = process.ProcRead<int>(moduleBase + dwEntityList + (weaponAddress - 1) * 0x10);
	return process.ProcRead<int>(m_iBase + m_iItemDefinitionIndex);
}

void Multihack::DrawCrosshair(HDC& hDC, HBRUSH brush) const
{
	//GetSize();
	//Rectangle(hDC, screenX / 2 - 4, screenY / 2 - 4, screenX / 2 + 4, screenY / 2 + 4);
	RECT crosshair{ screenX / 2 - 4,  screenY / 2 - 4, screenX / 2 + 4, screenY / 2 + 4 };
	FillRect(hDC, &crosshair, brush);
}

void Multihack::Crosshair()
{
	HDC csgoDC;
	HBRUSH brush = HBRUSH();
	bool exist = false;

	while (active)
	{
		if (GetAsyncKeyState((int)hKeys::CROSSHAIR) & 1)
		{
			enabled[hID::CROSSHAIR] = !enabled[hID::CROSSHAIR];

			if (enabled[hID::CROSSHAIR])
			{
				brush = CreateSolidBrush(RGB(0, 255, 0));
				csgoDC = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive"));
				std::cout << "Auto crosshair enabled\n";
			}
			else
			{
				std::cout << "Auto crosshair disabled\n";
				DeleteObject(brush);
			}
		}
		if (enabled[hID::CROSSHAIR])
		{
			uintptr_t lPlayer = process.ProcRead<uintptr_t>(moduleBase + dwLocalPlayer);
			int id = WeaponID(lPlayer);
			bool draw = false;

			// cheking for sniper rifle scope
			if (id == 40 || id == 38 || id == 9 || id == 262155 || id == 11)
			{
				bool isScoped = process.ProcRead<bool>(lPlayer + m_bIsScoped);
				if (!isScoped)
					draw = true;
			}

			if (draw)
				DrawCrosshair(csgoDC, brush);
		}

		Sleep(1);
	}
}

void Multihack::NormalizeAngles(Vector3& vec)
{
	while (vec.y > 180)
		vec.y -= 360;

	while (vec.y < -180)
		vec.y += 360;

	if (vec.x > 89.0f)
		vec.x = 89.0f;

	if (vec.x < -89.0f)
		vec.x = -89.0f;
}

uintptr_t Multihack::ClosestEnemy()
{
	if (screenX == screenY == -1)
	{
		return 0;
	}
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
		bool spotted = SpottedByMe(entity);
		
		if (entity && spotted && entityTeam != team && !isDormant && health > 0 && health < 101)
		{
			// number 8 states for head bone id
			Vector3 head = getEntHead(entity);
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

Vector3 Multihack::getEntHead(uintptr_t entity) const
{
	uintptr_t boneBase = process.ProcRead<uintptr_t>(entity + m_dwBoneMatrix);
	boneMatrix_t boneHead = process.ProcRead<boneMatrix_t>(boneBase + sizeof(boneHead) * 8);
	return { boneHead.x, boneHead.y, boneHead.z };
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
	// be sure that sinus is not greater than 1 by a absolute value
	if (abs(delta.z / len) > 1)
	{
		// could be any number greater than 360
		angles.x = 361;
		return angles;
	}
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
	LaunchThreads();

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1)
		{
			active = !active;
			if (active)
			{
				std::cout << "Multihack enabled\n";
				LaunchThreads();
			}
			else
			{
				std::cout << "Multihack disabled\n";
				StopAll();
			}
		}
			
		Sleep(1000);
	}
}

void Multihack::LaunchThreads()
{
	cheatTreads[hID::BHOP] = std::thread(&Multihack::Bhop, this);
	cheatTreads[hID::RADAR_HACK] = std::thread(&Multihack::RadarHack, this);
	cheatTreads[hID::ESP] = std::thread(&Multihack::ESP, this);
	cheatTreads[hID::AIMBOT] = std::thread(&Multihack::AimBot, this);
	cheatTreads[hID::RECOIL] = std::thread(&Multihack::RecoilControl, this);
	cheatTreads[hID::FLASH] = std::thread(&Multihack::AntiFlash, this);
	cheatTreads[hID::TRIGGER] = std::thread(&Multihack::TriggerBot, this);
	//cheatTreads[hID::CROSSHAIR] = std::thread(&Multihack::Crosshair, this);
}

bool Multihack::SpottedByMe(uintptr_t player) const
{
	// checks whether enemy player was spotted by local player
	int localindex = GetLocalIndex();
	long dwMask = process.ProcRead<long>(player + m_bSpottedByMask);
	return bool(dwMask & (1 << localindex));
}

int Multihack::GetLocalIndex() const
{
	// returns local player index
	uintptr_t engine = process.GetModule("engine.dll");
	uintptr_t clientState = process.ProcRead<uintptr_t>(engine + dwClientState);
	return process.ProcRead<int>(clientState + dwClientState_GetLocalPlayer);
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