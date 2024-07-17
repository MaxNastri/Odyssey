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
#include "ShaderCompiler.h"
#include "EditorContext.h"

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
		auto drawListener = EditorContext::GetInstance()->GetGUIManager()->GetDrawGUIListener();
		renderer->GetImGui()->SetDrawGUIListener(drawListener);
		AssetManager::CreateDatabase();

		// Start listening for events
		ScriptCompiler::ListenForEvents();
		SceneManager::ListenForEvents();

		// Build the user assembly
		ScriptCompiler::BuildUserAssembly();

		SetupEditorGUI();
		CreateRenderPasses();

		// We're off an running
		running = true;

		ShaderCompiler* compiler = EditorContext::GetInstance()->GetShaderCompiler();
		compiler->Compile("Frag", ShaderType::Fragment, "C:/Git/Odyssey/Native/Editor/Assets/Shaders/glsl/red.frag.glsl");
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

				EditorContext::GetInstance()->GetGUIManager()->Update();
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
		GUIManager* guiManager = EditorContext::GetInstance()->GetGUIManager();
		guiManager->CreateInspectorWindow(nullptr);
		guiManager->CreateSceneHierarchyWindow();
		guiManager->CreateSceneViewWindow();
		guiManager->CreateContentBrowserWindow();
	}

	void Application::CreateRenderPasses()
	{
		GUIManager* guiManager = EditorContext::GetInstance()->GetGUIManager();
		renderer->AddRenderPass(guiManager->GetSceneViewWindow(0).GetRenderPass());
		renderer->AddRenderPass(guiManager->GetRenderPass());
	}
}