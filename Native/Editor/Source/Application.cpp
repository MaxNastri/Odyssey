#include "Application.h"
#include <Input.h>
#include <Logger.h>
#include <FileManager.h>
#include <Paths.h>
#include <VulkanRenderer.h>
#include "ScriptCompiler.h"
#include <ScriptingManager.h>
#include "SceneManager.h"
#include "GUIManager.h"
#include <ComponentManager.h>
#include <Camera.h>
#include <MeshRenderer.h>
#include "Mesh.h"
#include "AssetManager.h"
#include "Shader.h"
#include "Material.h"

namespace Odyssey
{
	Application::Application()
	{
		// Initialize our managers
		ScriptingManager::Initialize();
		FileManager::Initialize();
		FileManager::TrackFolder(Paths::Relative::ManagedProjectSource);
		AssetManager::CreateDatabase();

		// Create the renderer
		renderer = std::make_shared<VulkanRenderer>();

		// Start listening for events
		ScriptCompiler::ListenForEvents();
		GUIManager::Initialize();
		SceneManager::ListenForEvents();

		// Build the user assembly
		ScriptCompiler::BuildUserAssembly();

		ConstructVisuals();
		SetupEditorGUI();
		CreateRenderPasses();

		// We're off an running
		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		SceneManager::Awake();

		while (running)
		{
			float elapsed = stopwatch.Elapsed();

			if (elapsed > MaxFPS)
			{
				stopwatch.Restart();
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

	void Application::ConstructVisuals()
	{
	}

	void Application::SetupEditorGUI()
	{
		GUIManager::CreateInspectorWindow(nullptr);
		GUIManager::CreateSceneHierarchyWindow();
		GUIManager::CreateSceneViewWindow();
		GUIManager::CreateContentBrowserWindow();
	}

	void Application::CreateRenderPasses()
	{
		renderer->AddRenderPass(GUIManager::GetSceneViewWindow(0).GetRenderPass());
		renderer->AddRenderPass(GUIManager::GetRenderPass());
	}
}