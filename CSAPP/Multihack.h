#pragma once
#include "MemScan.h"
#include "config.h"
#include <thread>
#include <mutex>


class Multihack
{
public:
	Multihack();
	~Multihack();
	bool Cheatable() const;
private:
	ProcessHandler process;
	uintptr_t moduleBase;
	std::mutex printLock;
	bool terminate;
	bool active;
	// cheats availability
	bool enabled[cNums];
	std::thread cheatTreads[cNums];
	int checkTime;

	// cheats
	void ESP();
	void Bhop();
	void RadarHack();
	void AimBot();
	void TriggerBot();
	void RecoilControl();
	void AntiFlash();
	void Crosshair();

	// methods used in cheats
	int WeaponID(uintptr_t);
	void DrawCrosshair(HDC&, HBRUSH) const;
	void NormalizeAngles(Vector3&);
	uintptr_t ClosestEnemy();
	Vector3 getAngles(Vector3, Vector3) const;
	void ClientUpdate();
	void Options();
	void StopAll();
	void LaunchThreads();
	Vector3 getEntHead(uintptr_t) const;
	int GetLocalIndex() const;
	bool SpottedByMe(uintptr_t) const;

	// Printing menu stuff
	void PrintMenu();
	void PrintCheatInfo(int) const;
};