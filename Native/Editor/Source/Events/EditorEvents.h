#pragma once
#include <EventSystem.h>
#include <FileWatcherHandle.h>
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
}