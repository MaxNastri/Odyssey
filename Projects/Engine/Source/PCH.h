#pragma once
#include <array>
#include <assert.h>
#include <bitset>
#include <cstring>
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <map>
#include <omp.h>
#include <queue>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef _WIN32
	#define NOMINMAX
	#include <Windows.h>
#endif

#include "Utils.h"
#include "Globals.h"
#include "Ref.h"
#include "Enum.h"
#include "Enums.h"

#include "glm.h"
using namespace glm;

#include "Jolt/Jolt.h"
using namespace JPH;

using Path = std::filesystem::path;

namespace Odyssey
{
	template<typename T>
	inline static bool Contains(const std::vector<T>& vector, const T& search)
	{
		return std::find(vector.begin(), vector.end(), search) != vector.end();
	}

	inline static std::string ToLower(const std::string& str)
	{
		std::string copy = str;
		std::transform(copy.begin(), copy.end(), copy.begin(),
			[](unsigned char c) { return std::tolower(c); });

		return copy;
	}
}
