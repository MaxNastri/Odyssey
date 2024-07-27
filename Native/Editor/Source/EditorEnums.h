#pragma once
#include <string>

namespace Odyssey
{
	struct GUISelection
	{
	public:
		std::string GUID;
		std::string Type;
		std::filesystem::path FilePath;
		int32_t ID;
	};
}