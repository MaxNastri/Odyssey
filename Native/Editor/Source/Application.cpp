#include "Application.h"
#include <Input.h>
#include <Log.h>
#include <Scene.h>
#include <FileManager.h>
#include <Paths.h>
#include <Graphics.h>
#include <VulkanRenderer.h>

namespace Odyssey::Editor
{
	Application::Application()
	{
		ScriptingManager::Initialize();
		FileManager::Initialize();
		FileManager::TrackFolder(Paths::Relative::ManagedProjectSource);
		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		// Create the scene
		Entities::Scene scene;
		scene.Deserialize("scene.json");

		Entities::GameObject go = scene.GetGameObject(0);
		std::shared_ptr<InspectorWindow> inspector = std::make_shared<InspectorWindow>(go);
		guiElements.push_back(inspector);

		while (running)
		{
			float elapsed = stopwatch.Elapsed();

			if (elapsed > MaxFPS)
			{
				stopwatch.Restart();

				if (allowRecompile && Odyssey::Framework::Input::GetKeyPress(Odyssey::KeyCode::Space))
				{
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

						inspector->RefreshUserScripts();
					}
				}
				scene.Update();

				if (!r.Update())
				{
					running = false;
				}

				r.Render(guiElements);
			}
		}
	}

	void Application::Exit()
	{
		running = false;
	}
}