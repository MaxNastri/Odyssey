#pragma once
#include "magic_enum.hpp"

namespace Odyssey::Enum
{

	template<typename T>
	inline std::string_view ToString(T value)
	{
		return magic_enum::enum_name(value);
	}

	template<typename T>
	inline T ToEnum(std::string_view value)
	{
		auto ret = magic_enum::enum_cast<T>(value, magic_enum::case_insensitive);
	}

	template<typename T>
	inline auto GetEntries()
	{
		return magic_enum::enum_entries<T>();
	}

	template<typename T>
	inline auto GetNameSequence()
	{
		return magic_enum::enum_names<T>();
	}
}