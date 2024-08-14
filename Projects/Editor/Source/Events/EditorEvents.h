#pragma once
#include <EventSystem.h>
#include <Scene.h>
#include "EditorEnums.h"

namespace Odyssey
{
	struct PlaymodeStateChangedEvent : public Event
	{
	public:
		PlaymodeStateChangedEvent(PlaymodeState state)
		{
			State = state;
		}

		PlaymodeState State;
	};

	struct GUISelectionChangedEvent : public Event
	{
	public:
		GUISelectionChangedEvent(const GUISelection& selection)
		{
			Selection = selection;
		}

		GUISelection Selection;
	};
}