#pragma once
#include <wrl.h>

struct WindowInfo
{
	HWND hwnd;
	unsigned int width;
	unsigned int height;
	bool windowed;
	bool VSync;
};

enum class ShaderStage : UINT8
{
	VS,
	GS,
	DS,
	HS,
	PS,
	CS
};

enum class LAYER_TYPE : UINT8
{
	CAMERA,
	BACKGROUND,
	FIXEDOBJECT,
	MONSTER,
	PLAYER,
	PLAYER_WEAPON,
	UI,
	END
};

enum class DebugMeshType
{
	RECT,
	CIRCLE,
	END
};

struct DebugAttribute
{
	DebugMeshType dtype;
	Matrix transform;
	float duration;
	float time;
};



