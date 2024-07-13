#pragma once
#include <string>

namespace Odyssey
{
	struct GUISelection
	{
		enum class SelectionType
		{
			None = 0,
			GameObject = 1,
			Material = 2,
			Mesh = 3,
			Shader = 4,
		};
	public:
		SelectionType Type;
		std::string guid;
		int32_t ID;
	};
}