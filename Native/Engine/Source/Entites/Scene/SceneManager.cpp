#include "SceneManager.h"
#include <EventSystem.h>
#include "Events.h"

namespace Odyssey
{
	void SceneManager::ListenForEvents()
	{
		EventSystem::Listen<OnBuildFinished>(SceneManager::BuildFinished);
		EventSystem::Listen<OnAssembliesReloaded>(SceneManager::AssembliesReloaded);
	}

	void SceneManager::LoadScene(const std::string& filename)
	{
		if (activeScene != -1)
		{
			scenes[activeScene]->OnDestroy();
			scenes[activeScene]->Clear();
			scenes[activeScene].reset();
		}

		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		scene->Deserialize(filename);
		scenes.push_back(scene);

		activeScene = (int)scenes.size() - 1;

		// TODO: Send a copy of the scene, so the GUI manager can use the game objects to reload the inspectors
		EventSystem::Dispatch<OnSceneLoaded>(scenes[activeScene].get());
	}

	void SceneManager::SaveActiveScene(const std::string& filename)
	{
		scenes[activeScene]->Serialize(filename);
	}

	Scene* SceneManager::GetActiveScene()
	{
		if (activeScene < scenes.size())
			return scenes[activeScene].get();

		return nullptr;
	}

	std::shared_ptr<Scene> SceneManager::GetActiveSceneRef()
	{
		if (activeScene < scenes.size())
			return scenes[activeScene];

		return nullptr;
	}

	void SceneManager::Awake()
	{
		if (activeScene < scenes.size())
			scenes[activeScene]->Awake();
	}

	void SceneManager::Update()
	{
		if (activeScene < scenes.size())
			scenes[activeScene]->Update();
	}

	void SceneManager::BuildFinished(OnBuildFinished* onBuildFinished)
	{
		if (activeScene < scenes.size())
		{
			if (onBuildFinished->success)
			{
				scenes[activeScene]->Serialize(tempSaveFilename);
				scenes[activeScene]->Clear();
			}
		}
	}

	void SceneManager::AssembliesReloaded(OnAssembliesReloaded* reloadedEvent)
	{
		if (activeScene < scenes.size())
		{
			scenes[activeScene]->Deserialize(tempSaveFilename);
			EventSystem::Dispatch<OnSceneLoaded>(scenes[activeScene].get());
		}
	}
}