#include "GUID.h"
#include <random>

namespace Odyssey
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	GUID::GUID()
		: m_GUID(0)
	{
	}

	GUID::GUID(uint64_t guid)
		: m_GUID(guid)
	{
	}

	GUID::GUID(const std::string& guid)
		: m_GUID(std::stoull(guid))
	{
	}

	GUID::GUID(const GUID& other)
		: m_GUID(other.m_GUID)
	{
	}
	GUID GUID::New()
	{
		return GUID(s_UniformDistribution(engine));
	}
}