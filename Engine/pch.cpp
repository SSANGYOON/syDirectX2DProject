﻿
#include "pch.h"

string wtos(wstring w)
{
	return string(w.begin(), w.end());
}

wstring stow(string s)
{
	return wstring(s.begin(), s.end());
}

// 미리 컴파일된 헤더를 사용하는 경우 컴파일이 성공하려면 이 소스 파일이 필요합니다.
