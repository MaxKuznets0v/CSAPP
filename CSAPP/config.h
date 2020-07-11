#pragma once

const int cNums = 2;
// hotkeys for each hack
enum class hKeys
{
	BHOP = VK_F8,
	RADAR_HACK = VK_F9
};

// hacks ids
enum hID
{
	BHOP = 0,
	RADAR_HACK
};

// cheats availability
static bool enabled[cNums];
