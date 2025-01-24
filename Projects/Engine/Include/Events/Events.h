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

	struct BuildCompleteEvent : public Event
	{
	public:
		BuildCompleteEvent(bool succeeded)
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

	struct SceneModifiedEvent : public Event
	{
		enum class Modification
		{
			None = 0,
			DeleteGameObject = 1,
			CreateGameObject = 2,
			AddComponent = 3,
			RemoveComponent = 4,
		};
	public:
		SceneModifiedEvent(Scene* scene, Modification mod)
		{
			Scene = scene;
			Modification = mod;
		}

	public:
		Scene* Scene;
		Modification Modification;
	};
}