#include "Application.h"
#include "FileManager.h"
#include "AssetManager.h"
#include "ScriptCompiler.h"
#include "ScriptingManager.h"
#include "GUIManager.h"
#include "SceneManager.h"
#include <VulkanRenderer.h>
#include "OdysseyTime.h"
#include "Random.h"

namespace Odyssey
{
	Application::Application()
	{
		// Its important we initialize scripting first due to a bug with VS2022
		// With native debugging enabled, our breakpoints wont work before we init scripting
		ScriptingManager::Initialize();
		Random::Initialize();

		// Track the manage project folder for any file changes
		FileManager::Initialize();
		FileManager::TrackFolder(Paths::Relative::ManagedProjectSource);

		// Create the renderer
		renderer = std::make_shared<VulkanRenderer>();

		AssetManager::CreateDatabase();

		// Start listening for events
		ScriptCompiler::ListenForEvents();
		SceneManager::ListenForEvents();
		GUIManager::Initialize();

		// Build the user assembly
		ScriptCompiler::BuildUserAssembly();

		SetupEditorGUI();
		CreateRenderPasses();

		// We're off an running
		running = true;
	}

	void Application::Run()
	{
		running = true;

		Time::Begin();
		SceneManager::Awake();

		while (running)
		{
			Time::Tick();
			float deltaTime = Time::DeltaTime();
			m_TimeSinceLastUpdate += deltaTime;

			if (m_TimeSinceLastUpdate > MaxFPS)
			{
				m_TimeSinceLastUpdate = 0.0f;

				// Process any changes made to the user's managed dll
				ScriptCompiler::Process();

				GUIManager::Update();
				SceneManager::Update();

				if (!renderer->Update())
				{
					running = false;
				}

				renderer->Render();
			}
		}
		renderer->Destroy();
	}

	void Application::Exit()
	{
		running = false;
	}

	void Application::SetupEditorGUI()
	{
		GUIManager::CreateInspectorWindow(nullptr);
		GUIManager::CreateSceneHierarchyWindow();
		GUIManager::CreateSceneViewWindow();
		GUIManager::CreateContentBrowserWindow();
		GUIManager::CreateRayTracingWindow();
	}

	void Application::CreateRenderPasses()
	{
		renderer->AddRenderPass(GUIManager::GetSceneViewWindow(0).GetRenderPass());
		renderer->AddRenderPass(GUIManager::GetRenderPass());
	}
}