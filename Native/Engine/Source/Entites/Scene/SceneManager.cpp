#include "SceneManager.h"
#include "Scene.h"
#include "AssetManager.h"
#include "EventSystem.h"
#include "Events.h"

namespace Odyssey
{
	void SceneManager::Initialize()
	{
	}

	void SceneManager::LoadScene(const std::string& filename)
	{
		if (activeScene != -1)
		{
			scenes[activeScene].Get()->OnDestroy();
			scenes[activeScene].Get()->Clear();

			AssetManager::UnloadScene(scenes[activeScene]);
		}

		scenes.push_back(AssetManager::LoadScene(filename));

		activeScene = (int)scenes.size() - 1;

		// TODO: Send a copy of the scene, so the GUI manager can use the game objects to reload the inspectors
		EventSystem::Dispatch<SceneLoadedEvent>(scenes[activeScene].Get());
	}

	void SceneManager::SaveActiveScene()
	{
		if (activeScene < scenes.size())
			scenes[activeScene].Get()->Save();
	}

	Scene* SceneManager::GetActiveScene()
	{
		if (activeScene < scenes.size())
			return scenes[activeScene].Get();

		return nullptr;
	}

	void SceneManager::Awake()
	{
		if (activeScene < scenes.size())
			scenes[activeScene].Get()->Awake();
	}

	void SceneManager::Update()
	{
		if (activeScene < scenes.size())
			scenes[activeScene].Get()->Update();
	}

	void SceneManager::BuildFinished(BuildCompleteEvent* onBuildFinished)
	{
		if (activeScene < scenes.size())
		{
			if (onBuildFinished->success)
			{
				//scenes[activeScene].Get()->SaveTo(tempSaveFilename);
				//scenes[activeScene].Get()->Clear();
			}
		}
	}

	void SceneManager::AssembliesReloaded(OnAssembliesReloaded* reloadedEvent)
	{
		if (activeScene < scenes.size())
		{
			//scenes[activeScene].Get()->Load(tempSaveFilename);
			EventSystem::Dispatch<SceneLoadedEvent>(scenes[activeScene].Get());
		}
	}
}