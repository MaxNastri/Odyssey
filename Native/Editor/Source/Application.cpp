#include "Application.h"
#include <Input.h>
#include <Logger.h>
#include <FileManager.h>
#include <Paths.h>
#include <Graphics.h>
#include <VulkanRenderer.h>
#include "ScriptCompiler.h"
#include <ScriptingManager.h>
#include "SceneManager.h"
#include "GUIManager.h"

namespace Odyssey
{
	Application::Application()
	{
		Scripting::ScriptingManager::Initialize();
		FileManager::Initialize();
		FileManager::TrackFolder(Paths::Relative::ManagedProjectSource);
		ScriptCompiler::ListenForEvents();
		GUIManager::ListenForEvents();
		SceneManager::ListenForEvents();

		ScriptCompiler::BuildUserAssembly();

		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		// Create the scene
		SceneManager::LoadScene("scene.yaml");

		Scene scene = SceneManager::GetActiveScene();
		GameObject go = scene.GetGameObject(0);
		GUIManager::CreateInspectorWindow(go);

		while (running)
		{
			float elapsed = stopwatch.Elapsed();

			if (elapsed > MaxFPS)
			{
				stopwatch.Restart();
				ScriptCompiler::Process();

				SceneManager::Update();

				if (!r.Update())
				{
					running = false;
				}

				r.Render();
			}
		}
	}

	void Application::Exit()
	{
		running = false;
	}
}