#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN  


#include "EnginePch.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "Input.h"
#include "Timer.h"

#define INPUT GET_SINGLE(Input)
#define TIME GET_SINGLE(Timer)
#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define KEY_PRESSED(KEY_TYPE) (INPUT->GetKeyState(KEY_TYPE) == KEY_STATE::PRESS)

extern string wtos(wstring w);

extern wstring stow(string s);

#endif //PCH_H