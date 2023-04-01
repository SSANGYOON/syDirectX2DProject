#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN  


#include "EnginePch.h"
#include "Input.h"
#include "Timer.h"

#define INPUT GET_SINGLE(Input)
#define TIME GET_SINGLE(Timer)

#endif //PCH_H