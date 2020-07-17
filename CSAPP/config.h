#pragma once
#include <cmath> 

const int screenX = GetSystemMetrics(SM_CXSCREEN);
const int screenY = GetSystemMetrics(SM_CYSCREEN);
#define M_PI 3.14159265358979323846


const int cNums = 4;
// hotkeys for each hack
enum class hKeys
{
	ESP = VK_F6,
	BHOP = VK_F7,
	AIMBOT = VK_F8,
	RADAR_HACK = VK_F9
};

// hacks ids
enum hID
{
	BHOP = 0,
	RADAR_HACK,
	ESP,
	AIMBOT
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

struct Vector3
{
	float x, y, z;
	Vector3 operator-(Vector3 another) const
	{
		return { x - another.x, y - another.y, z - another.z };
	}
	float len3() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	float len2() const
	{
		return sqrt(x * x + y * y);
	}
	Vector3 operator*(float cons) const
	{
		return { x * cons, y * cons, z * cons };
	}
};

struct view_matrix_t {
	float* operator[](int index) 
	{
		return matrix[index];
	}

	float matrix[4][4];
};

static Vector3 WorldToScreen(const Vector3 pos, view_matrix_t matrix) 
{
	float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

	float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = screenX * .5f;
	float y = screenY * .5f;

	x += 0.5f * _x * screenX + 0.5f;
	y -= 0.5f * _y * screenY + 0.5f;

	return { x,y,w };
}

struct boneMatrix_t 
{
	byte pad3[12];
	float x;
	byte pad1[12];
	float y;
	byte pad2[12];
	float z;
};
