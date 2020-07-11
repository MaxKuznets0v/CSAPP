#pragma once


// csgo offsets (needed to be constantly updated)
namespace offsets
{
	const unsigned int dwEntityList = 0x4D523EC;
	namespace bunnyHop
	{
		const unsigned int m_vecVelocity = 0x114;
		const unsigned int dwMouseEnable = 0xD43790;
		const unsigned int dwForceJump = 0x51FC094;
		const unsigned int m_fFlags = 0x104;
	}
	namespace radar
	{
		const unsigned int m_bSpotted = 0x93D;
	}
}
