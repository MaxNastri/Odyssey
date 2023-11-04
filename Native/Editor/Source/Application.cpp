#include "Application.h"
#include <Input.h>
#include <Log.h>

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
				if (Odyssey::Framework::Input::GetKeyPress(Odyssey::KeyCode::Space))
				{
					Odyssey::Framework::Log::Info("Space bar pressed");
				}
				scriptingManager.UpdateScripts();
			}
		}
	}

	void Application::Exit()
	{
		running = false;
	}
}