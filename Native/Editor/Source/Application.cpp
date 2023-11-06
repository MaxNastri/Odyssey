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
	Application::Application()
	{
		ScriptingManager::Initialize();
		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		// Create the scene
		Entities::Scene scene;
		scene.Deserialize("scene.json");

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
					allowRecompile = false;
					if (Scripting::ScriptingManager::RecompileUserAssemblies())
					{
						// Scene manager.tempsave
						scene.Serialize("tmpSave.json");
						scene.Clear();

						// Reload assemblies
						Scripting::ScriptingManager::ReloadUserAssemblies();

						// Load temp scene back into memory
						scene.Deserialize("tmpSave.json");
					}
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