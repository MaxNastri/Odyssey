#pragma once
#include <EventSystem.h>
#include <FileWatcherHandle.h>
#include <Scene.h>

namespace Odyssey
{
	struct OnUserFilesModified : public Event
	{
	public:
		OnUserFilesModified(const NotificationSet& notificationSet)
		{
			changedFileSet = notificationSet;
		}

		NotificationSet changedFileSet;
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