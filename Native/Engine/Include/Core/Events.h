#pragma once
#include "EventSystem.h"
#include "FileWatcherHandle.h"

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

	struct OnUserFilesModified : public Event
	{
	public:
		OnUserFilesModified(const NotificationSet& notificationSet)
		{
			changedFileSet = notificationSet;
		}

		NotificationSet changedFileSet;
	};

#pragma endregion

	class Scene;

	struct OnSceneLoaded : public Event
	{
	public:
		OnSceneLoaded(Scene* scene)
		{
			loadedScene = scene;
		}

		Scene* loadedScene;
	};
}