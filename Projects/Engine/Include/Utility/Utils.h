#pragma once
#include "Log.h"
#include <string>

namespace Odyssey
{
	inline void Hash(std::size_t& seed, const uint64_t& val)
	{
		seed ^= std::hash<uint64_t>{}(val)+0x9e3779b97f4a7c15LLU + (seed << 12) + (seed >> 4);
	}

	inline std::size_t HashCombine(uint64_t a, uint64_t b)
	{
		std::size_t seed = 0;
		Hash(seed, a);
		Hash(seed, b);
		return seed;
	}

	static constexpr uint32_t GenerateFNVHash(std::string_view str)
	{
		constexpr uint32_t FNV_PRIME = 16777619u;
		constexpr uint32_t OFFSET_BASIS = 2166136261u;

		const size_t length = str.length();
		const char* data = str.data();

		uint32_t hash = OFFSET_BASIS;
		for (size_t i = 0; i < length; ++i)
		{
			hash ^= *data++;
			hash *= FNV_PRIME;
		}
		hash ^= '\0';
		hash *= FNV_PRIME;

		return hash;
	}

	inline std::string ConvertWideToUtf8(const std::wstring& wstr)
	{
		std::string str;
		size_t size;

		str.resize(wstr.length());
		wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
		return str;
	}

	inline std::wstring ConvertUtf8ToWide(const std::string& str)
	{
		std::wstring wstr;
		size_t size;

		wstr.resize(str.length());
		mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
		return wstr;
	}
}