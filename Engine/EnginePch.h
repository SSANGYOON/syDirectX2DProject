#pragma once

// std::byte 사용하지 않음
#define _HAS_STD_BYTE 0

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <bitset>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include <wrl.h>
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

extern unique_ptr<class Engine> GEngine;

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define CONTEXT				GEngine->GetDevice()->GetContext()

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
#define CBSLOT_GRID 2
#define CBSLOT_SPRITE 3


struct Vertex
{
	Vector4 pos;
	Vector4 color;
	Vector2 uv;
};