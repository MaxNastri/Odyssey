#include "SceneManager.h"
#include "Scene.h"
#include "AssetManager.h"
#include "EventSystem.h"
#include "Events.h"

namespace Odyssey
{
	void SceneManager::LoadScene(const Path& filename)
	{
		if (activeScene != -1)
		{
			scenes[activeScene]->OnDestroy();
			scenes[activeScene]->Clear();

		}

		scenes.push_back(std::make_shared<Scene>(filename));

		activeScene = (int)scenes.size() - 1;

		// TODO: Send a copy of the scene, so the GUI manager can use the game objects to reload the inspectors
		EventSystem::Dispatch<SceneLoadedEvent>(scenes[activeScene].get());
	}

	void SceneManager::SaveActiveScene()
	{
		if (activeScene < scenes.size())
			scenes[activeScene]->Save();
	}

	void SceneManager::SaveActiveScene(const Path& path)
	{
		if (activeScene < scenes.size())
		{
			scenes[activeScene]->SaveTo(path);
			LoadScene(path.string());
		}
	}

	Scene* SceneManager::GetActiveScene()
	{
		if (activeScene < scenes.size())
			return scenes[activeScene].get();

		return nullptr;
	}

	void SceneManager::Awake()
	{
		if (activeScene < scenes.size())
			scenes[activeScene]->Awake();
	}

	void SceneManager::OnEditorUpdate()
	{
		if (activeScene < scenes.size())
			scenes[activeScene]->OnEditorUpdate();
	}

	void SceneManager::Update()
	{
		if (activeScene < scenes.size())
			scenes[activeScene]->Update();
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
			EventSystem::Dispatch<SceneLoadedEvent>(scenes[activeScene].get());
		}
	}
}