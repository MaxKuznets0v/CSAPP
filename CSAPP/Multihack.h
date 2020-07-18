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
	uintptr_t moduleBase;
	//HDC csgoDC;
	// multicheat activity
	bool active;
	// cheats availability
	bool enabled[cNums];
	std::thread cheatTreads[cNums];
	void ESP();
	// not used in this hack
	/*void HealthBar(uintptr_t, float);
	void DrawBar(RECT, float);*/
	void Bhop();
	void RadarHack();
	void AimBot();
	uintptr_t ClosestEnemy();
	Vector3 getAngles(Vector3, Vector3) const;
	void ClientUpdate();
	void Options();
	void StopAll();
	void DrawLine(float StartX, float StartY, float EndX, float End);
};