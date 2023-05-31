#pragma once
#include "ConstantBuffer.h"

namespace SY {
	enum class LIGHT_TYPE
	{
		POINT,
		SPOT,
		DIRECTIONAL,
		AMBIENT
	};

	struct Light
	{
		LightInfo info;
		static void DrawImGui(Light& component);
	};
}