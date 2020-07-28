#pragma once
#include "MemScan.h"
#include "config.h"
#include <thread>


class Multihack
{
public:
	Multihack();
	~Multihack();
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
	void TriggerBot();
	void RecoilControl();
	void AntiFlash();
	void Crosshair();
	int WeaponID(uintptr_t);
	void DrawCrosshair(HDC&, HBRUSH) const;
	void NormalizeAngles(Vector3&);
	uintptr_t ClosestEnemy();
	Vector3 getAngles(Vector3, Vector3) const;
	void ClientUpdate();
	void Options();
	void StopAll();
	void LaunchThreads();
	void DrawLine(float StartX, float StartY, float EndX, float End);
	Vector3 getEntHead(uintptr_t) const;
	int GetLocalIndex() const;
	bool SpottedByMe(uintptr_t) const;
};