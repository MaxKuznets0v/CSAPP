#pragma once
#include <cmath> 
#include <exception>

static int screenX;
static int screenY;

static void GetSize()
{
	RECT rect;
	HWND csgownd = FindWindowA(NULL, "Counter-Strike: Global Offensive");

	/*if (GetWindowRect(csgownd, &rect))
	{
		screenX = rect.right - rect.left;
		screenY = rect.bottom - rect.top;
	}
	else
	{
		screenX = -1;
		screenY = -1;
	}*/
	screenX = 1920;
	screenY = 1080;
}

//const int screenX = GetSystemMetrics(SM_CXSCREEN);
//const int screenY = GetSystemMetrics(SM_CYSCREEN);
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
	bool operator==(Vector3 another) const
	{
		return x == another.x && y == another.y && z == another.z;
	}
	Vector3 operator-(Vector3 another) const
	{
		return { x - another.x, y - another.y, z - another.z };
	}
	Vector3 operator+(Vector3 another) const
	{
		return { x + another.x, y + another.y, z + another.z };
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
	Vector3 operator/(float cons) const
	{
		if (cons)
			return { x / cons, y / cons, z / cons };
		else
			throw std::exception("Divided by zero!\n");
	}
};

struct view_matrix_t 
{
	float matrix[16];
};

static Vector3 WorldToScreen(const Vector3 pos, view_matrix_t matrix) 
{
	Vector3 res;
	float _x = matrix.matrix[0] * pos.x + matrix.matrix[1] * pos.y + matrix.matrix[2] * pos.z + matrix.matrix[3];
	float _y = matrix.matrix[4] * pos.x + matrix.matrix[5] * pos.y + matrix.matrix[6] * pos.z + matrix.matrix[7];
	res.z = matrix.matrix[12] * pos.x + matrix.matrix[13] * pos.y + matrix.matrix[14] * pos.z + matrix.matrix[15];

	if (res.z < 0.1f)
		return { -1, -1, -1 };
	_x /= res.z;
	_y /= res.z;

	res.x = screenX * .5f;
	res.y = screenY * .5f;

	res.x += 0.5f * _x * screenX + 0.5f;
	res.y -= 0.5f * _y * screenY + 0.5f;

	return res;
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
