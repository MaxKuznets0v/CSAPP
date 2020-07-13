#pragma once


// csgo offsets (needed to be constantly updated)
namespace offsets
{
	uintptr_t dwEntityList = 0x4D523EC;
	namespace bunnyHop
	{
		uintptr_t m_vecVelocity = 0x114; // not used
		uintptr_t dwMouseEnable = 0xD43790;
		uintptr_t dwForceJump = 0x51FC094;
		uintptr_t m_fFlags = 0x104;
	}
	namespace radar
	{
		uintptr_t m_bSpotted = 0x93D;
	}
	namespace esp
	{
		uintptr_t m_iGlowIndex = 0xA438;
		uintptr_t dwGlowObjectManager = 0x529A248;
		uintptr_t dwLocalPlayer = 0xD3DBEC;
		uintptr_t m_iHealth = 0x100;
		uintptr_t m_iTeamNum = 0xF4;
		uintptr_t m_bDormant = 0xED;
		uintptr_t m_bIsDefusing = 0x3930;
		// next two are nor used
		uintptr_t m_vecOrigin = 0x138;
		uintptr_t dwViewMatrix = 0x4D43D34;
	}
}
