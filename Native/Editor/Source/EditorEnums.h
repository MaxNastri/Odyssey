#pragma once
#include <string>

namespace Odyssey
{
	enum class PlaymodeState
	{
		None = 0,
		EnterPlaymode = 1,
		PausePlaymode = 2,
		ExitPlaymode = 3,
	};

	struct GUISelection
	{
	public:
		std::string GUID;
		std::string Type;
		std::filesystem::path FilePath;
	};
}