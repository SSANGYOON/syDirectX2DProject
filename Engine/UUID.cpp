#include "pch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace SY {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		: m_UUID(GetRandomNumber())
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}

	uint64_t GetRandomNumber()
	{
		return s_UniformDistribution(s_Engine);
	}

}