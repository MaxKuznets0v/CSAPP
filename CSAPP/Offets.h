#pragma once


// csgo offsets (needed to be constantly updated)
namespace offsets
{
	const unsigned int dwEntityList = 0x4D523EC;
	namespace bunnyHop
	{
		const unsigned int m_vecVelocity = 0x114; // not used
		const unsigned int dwMouseEnable = 0xD43790;
		const unsigned int dwForceJump = 0x51FC094;
		const unsigned int m_fFlags = 0x104;
	}
	namespace radar
	{
		const unsigned int m_bSpotted = 0x93D;
	}
	namespace esp
	{
		const unsigned int m_iGlowIndex = 0xA438;
		const unsigned int dwGlowObjectManager = 0x529A248;
		const unsigned int dwLocalPlayer = 0xD3DBEC;
		const unsigned int m_iHealth = 0x100;
		const unsigned int m_iTeamNum = 0xF4;
	}
}
