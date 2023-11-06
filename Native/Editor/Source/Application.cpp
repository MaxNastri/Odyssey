#include "Application.h"
#include <Input.h>
#include <Log.h>
#include <ECS.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>
#include <UserScript.h>
#include <ComponentManager.h>

namespace Odyssey::Editor
{
	Application::Application(std::filesystem::path path)
	{
		ScriptingManager::Initialize(path);
		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		// Create the scene
		Entities::Scene scene;
		//Entities::GameObject go = scene.CreateGameObject();
		//Entities::Transform* transform = Entities::ComponentManager::AddComponent<Entities::Transform>(go);
		//Entities::UserScript* userScript = Entities::ComponentManager::AddComponent<Entities::UserScript>(go);
		//Coral::ManagedObject managedUserScript = Scripting::ScriptingManager::CreateManagedObject("Example.Managed.ExampleScript");
		//userScript->SetManagedInstance(managedUserScript);
		//
		//scene.Serialize("scene.json");
		scene.Deserialize("scene.json");

		Entities::GameObject go2 = scene.CreateGameObject();
		Entities::ComponentManager::AddComponent<Entities::UserScript>(go2, "Example.Managed.ExampleScript");

		while (running)
		{
			float elapsed = stopwatch.Elapsed();

			if (elapsed > MaxFPS)
			{
				stopwatch.Restart();

				// Do update
				if (window.Update())
				{
					Exit();
					return;
				}
				if (allowRecompile && Odyssey::Framework::Input::GetKeyPress(Odyssey::KeyCode::Space))
				{
					Odyssey::Framework::Log::Info("Recompiling...");
					ScriptingManager::Recompile();
					allowRecompile = false;
				}
				scene.Update();
			}
		}
	}

	void Application::Exit()
	{
		running = false;
	}
}