#pragma once
#include <string>

namespace Odyssey
{
	enum class PlaymodeState
	{
		None = 0,
		EnterPlaymode = 1,
		ExitPlaymode = 2,
	};

	struct GUISelection
	{
	public:
		std::string GUID;
		std::string Type;
		std::filesystem::path FilePath;
		int32_t ID;
	};
}