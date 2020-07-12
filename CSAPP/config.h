#pragma once

const int cNums = 3;
// hotkeys for each hack
enum class hKeys
{
	ESP = VK_F7,
	BHOP = VK_F8,
	RADAR_HACK = VK_F9
};

// hacks ids
enum hID
{
	BHOP = 0,
	RADAR_HACK,
	ESP
};

// glow struct
struct Glow
{
	// just 4 bytes base (not used but needed to read memory properly)
	BYTE base[4];
	// rgb codes
	float red;
	float green;
	float blue;
	// intensity
	float alpha;
	// buffer 16 bytes (not used)
	BYTE buffer[16];
	// flags for occlusion and other info (not used)
	bool renderOccluded;
	bool renderNonOccluded;
	bool fullBloom;
	BYTE buffer1[5];
	int glowStyle;
};