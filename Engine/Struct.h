#pragma once
#include <wrl.h>

struct WindowInfo
{
	HWND hwnd;
	unsigned int width;
	unsigned int height;
	bool windowed;
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
	PLAYER,
	MONSTER,
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
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
	float duration;
	float time;
};



