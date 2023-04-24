#ifndef PCH_H
#define PCH_H

#define _HAS_STD_BYTE 0
#define NOMINMAX
#include "framework.h"
#include "EnginePch.h"
#include "Engine.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imguizmo.h"
#pragma comment(lib, "Engine.lib")

#ifdef _DEBUG
#pragma comment (lib, "yaml-cpp/Debug/yaml-cppd.lib")
#pragma comment (lib, "box2d/Debug/box2d.lib")
#else
#pragma comment (lib, "yaml-cpp/Release/yaml-cpp.lib")
#pragma comment (lib, "box2d/Release/box2d.lib")
#endif

#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define KEY_PRESSED(KEY_TYPE) (INPUT->GetKeyState(KEY_TYPE) == KEY_STATE::PRESS)
#endif //PCH_H