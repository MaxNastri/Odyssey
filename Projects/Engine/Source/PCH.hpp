#pragma once
#include <array>
#include <assert.h>
#include <bitset>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <map>
#include <queue>
#include <ranges>
#include <set>
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

#include "coreclr_delegates.h"
#include "hostfxr.h"
#include "glm.h"
#include "Utils.h"
#include "Globals.h"

using Path = std::filesystem::path;