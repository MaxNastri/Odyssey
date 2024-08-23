#include "FileID.h"
#include <random>

namespace Odyssey
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 engine(s_RandomDevice());
	static std::uniform_int_distribution<uint32_t> s_UniformDistribution;

	FileID::FileID()
		: m_FileID(s_UniformDistribution(engine))
	{
	}
	FileID::FileID(uint32_t fileID)
		: m_FileID(fileID)
	{
	}
	FileID::FileID(const std::string& fileID)
		: m_FileID(std::stoul(fileID))
	{
	}
	FileID::FileID(const FileID& other)
		: m_FileID(other.m_FileID)
	{
	}
}