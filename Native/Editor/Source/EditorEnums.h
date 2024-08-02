#pragma once
#include "GUID.h"

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
		GUID GUID;
		std::string Type;
		std::filesystem::path FilePath;
	};
}