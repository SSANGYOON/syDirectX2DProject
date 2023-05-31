#pragma once
#include "UUID.h"

namespace SY
{
	struct IDComponent
	{
		SY::UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID id)
			: ID(id) {}

	};
}

