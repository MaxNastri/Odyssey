#pragma once
#include "GameObject.h"
#include "RawBuffer.hpp"

namespace Odyssey
{
	enum class InteractionType
	{
		None = 0,
		Hovered = 1,
		Selection = 2,
		ContextMenu = 3,
		DragAndDropTarget = 4,
		Opened = 5,
	};

	template<typename T>
	struct Interaction
	{
	public:
		InteractionType Type = InteractionType::None;
		T* Target;
		RawBuffer Data;

		void Set(InteractionType type, T* target)
		{
			Type = type;
			Target = target;
		}

		void Clear()
		{
			Type = InteractionType::None;
			Target = nullptr;
		}

		bool Empty() { return Type == InteractionType::None; }
	};
}