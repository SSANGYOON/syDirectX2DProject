#pragma once
#include <wrl.h>

class Transform;
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
	GRID,
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
	Transform* _target;
	DebugMeshType dtype;
	Vector3 position;
	Vector3 scale;
	float duration;
	float time;
};



