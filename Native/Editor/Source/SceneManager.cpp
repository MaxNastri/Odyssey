#include "SceneManager.h"
#include <EventSystem.h>
#include "EditorEvents.h"
#include <ScriptingEvents.h>

namespace Odyssey::Editor
{
    std::vector<Entities::Scene> SceneManager::scenes;
    int SceneManager::activeScene = 0;
    void SceneManager::ListenForEvents()
    {
        Framework::EventSystem::Listen<OnBuildFinished>(SceneManager::BuildFinished);
        Framework::EventSystem::Listen<Scripting::OnAssembliesReloaded>(SceneManager::AssembliesReloaded);
    }

    void SceneManager::LoadScene(const std::string& filename)
    {
        Entities::Scene scene;
        scene.Deserialize(filename);

        scenes.push_back(scene);
        activeScene = scenes.size() - 1;

        // TODO: Send a copy of the scene, so the GUI manager can use the game objects to reload the inspectors
        Framework::EventSystem::Dispatch<OnSceneLoaded>(&scenes[activeScene]);

    }

    void SceneManager::SaveActiveScene(const std::string& filename)
    {
        scenes[activeScene].Serialize(filename);
    }

    Entities::Scene& SceneManager::GetActiveScene()
    {
        // TODO: insert return statement here
        return scenes[activeScene];
    }

    void SceneManager::Update()
    {
        scenes[activeScene].Update();
    }

    void SceneManager::BuildFinished(OnBuildFinished* onBuildFinished)
    {
        if (onBuildFinished->success)
        {
            scenes[activeScene].Serialize(tempSaveFilename);
            scenes[activeScene].Clear();
        }
    }

    void SceneManager::AssembliesReloaded(Scripting::OnAssembliesReloaded* reloadedEvent)
    {
        scenes[activeScene].Deserialize(tempSaveFilename);
        Framework::EventSystem::Dispatch<OnSceneLoaded>(&scenes[activeScene]);
    }
}