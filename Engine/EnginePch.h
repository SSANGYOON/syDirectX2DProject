#pragma once

#define _HAS_STD_BYTE 0
#define NOMINMAX

#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <bitset>
#include <iostream>
#include <fstream>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include <wrl.h>
#include <DirectXMath.h>
#include "SimpleMath.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

using namespace DirectX::SimpleMath;
#include "Struct.h"

#include "yaml-cpp/yaml.h"

extern unique_ptr<class Engine> GEngine;

#define DEVICE				GEngine->GetDevice()
#define CONTEXT				GEngine->GetContext()
#define SWAPCHAIN			GEngine->GetSwapChain()

#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()

#define CB_GETBINDSLOT(name) __CBUFFERBINDSLOT__##name##__
#define CBUFFER(name, slot) static const int CB_GETBINDSLOT(name) = slot; struct alignas(16) name

#define CBSLOT_TRANSFORM 0
#define CBSLOT_MATERIAL 1
#define CBSLOT_SPRITE 2
#define CBSLOT_LIGHT 3
#define CBSLOT_PANEL 4
#define CBSLOT_SLIDER 5
#define CBSLOT_SLOT 6

struct Vertex
{
	Vector4 pos;
	Vector4 color;
	Vector2 uv;
};
