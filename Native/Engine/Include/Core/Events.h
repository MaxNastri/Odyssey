#pragma once
#include "EventSystem.h"
#include "Globals.h"

namespace Odyssey
{
#pragma region Graphics

	struct OnGUIRenderEvent : public Event
	{

	};

#pragma endregion

#pragma region Scripting

	struct OnAssembliesReloaded : public Event
	{

	};

	struct OnBuildStart : public Event
	{

	};

	struct OnBuildFinished : public Event
	{
	public:
		OnBuildFinished(bool succeeded)
		{
			success = succeeded;
		}

		bool success = false;
	};

#pragma endregion

	class Scene;

	struct SceneLoadedEvent : public Event
	{
	public:
		SceneLoadedEvent(Scene* scene)
		{
			loadedScene = scene;
		}

		Scene* loadedScene;
	};
}