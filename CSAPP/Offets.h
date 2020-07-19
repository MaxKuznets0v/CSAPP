#pragma once


// csgo offsets (needed to be constantly updated)
// are not used due to inconvinience (changed by csgo.hpp)
namespace offsets
{
	uintptr_t dwEntityList = 0x4D523EC;
	uintptr_t dwViewMatrix = 0x4D43D34;
	uintptr_t m_bDormant = 0xED;
	uintptr_t dwLocalPlayer = 0xD3DBEC;
	uintptr_t m_iHealth = 0x100;
	uintptr_t m_iTeamNum = 0xF4;
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
		uintptr_t m_bIsDefusing = 0x3930;
	}
	namespace aimBot
	{
		uintptr_t m_dwBoneMatrix = 0x26A8;
		uintptr_t m_vecOrigin = 0x138;
		uintptr_t dwClientState_ViewAngles = 0x4D88;
	}
	namespace noRecoil
	{
		uintptr_t m_aimPunchAngle = 0x302C;
		//uintptr_t m_thirdPersonViewAngles = 0x31D8;
	}
}
