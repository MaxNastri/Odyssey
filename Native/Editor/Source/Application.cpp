#include "Application.h"
#include <Input.h>
#include <Log.h>
#include <ECS.h>

namespace Odyssey::Editor
{
	Application::Application(std::filesystem::path path)
	{
		scriptingManager.Initialize(path);
		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		Entities::ECS::Create();
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
					scriptingManager.Recompile();
				}
				scriptingManager.UpdateScripts();
				Entities::ECS::Update();
			}
		}
	}

	void Application::Exit()
	{
		running = false;
	}
}