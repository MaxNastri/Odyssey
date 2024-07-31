#include "Editor.h"
#include "FileTracker.h"
#include "AssetManager.h"
#include "ScriptingManager.h"
#include "GUIManager.h"
#include "SceneManager.h"
#include "VulkanRenderer.h"
#include "OdysseyTime.h"
#include "Random.h"
#include "ShaderCompiler.h"
#include "ProjectManager.h"
#include "Globals.h"

namespace Odyssey
{
	Editor::Editor()
	{
		// Its important we initialize scripting first due to a bug with VS2022
		// With native debugging enabled, our breakpoints wont work before we init scripting
		ScriptingManager::Initialize();
		Random::Initialize();

		ProjectManager::LoadProject("C:/Git/Odyssey/Managed/ExampleProject");

		// Track the manage project folder for any file changes
		FileTracker::Init();

		// Create the renderer
		renderer = std::make_shared<VulkanRenderer>();
		renderer->GetImGui()->SetDrawGUIListener(GUIManager::DrawGUI);
		GUIManager::Initialize();

		AssetManager::CreateDatabase(ProjectManager::GetAssetsDirectory(), ProjectManager::GetCacheDirectory());

		// Start listening for events
		SceneManager::Initialize();

		// Build the user assembly
		m_ScriptCompiler = std::make_unique<ScriptCompiler>();
		ScriptingManager::SetUserAssembliesPath(m_ScriptCompiler->GetUserAssemblyPath());
		m_ScriptCompiler->BuildUserAssembly();
		ScriptingManager::LoadUserAssemblies();

		SetupEditorGUI();
		CreateRenderPasses();

		auto listener = [this](PlaymodeStateChangedEvent* event) { OnPlaymodeStateChanged(event); };
		EventSystem::Listen<PlaymodeStateChangedEvent>(listener);

		// We're off an running
		running = true;
	}

	void Editor::Run()
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
				m_ScriptCompiler->Process();

				GUIManager::Update();

				// Only update the scene if we are updating scripts (playmode)
				if (m_UpdateScripts)
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

	void Editor::Exit()
	{
		running = false;
	}

	void Editor::SetupEditorGUI()
	{
		GUIManager::CreateInspectorWindow(nullptr);
		GUIManager::CreateSceneHierarchyWindow();
		GUIManager::CreateSceneViewWindow();
		GUIManager::CreateGameViewWindow();
		GUIManager::CreateContentBrowserWindow();
	}

	void Editor::CreateRenderPasses()
	{
		renderer->AddRenderPass(GUIManager::GetSceneViewWindow(0)->GetRenderPass());
		renderer->AddRenderPass(GUIManager::GetGameViewWindow(0)->GetRenderPass());
		renderer->AddRenderPass(GUIManager::GetRenderPass());
	}

	void Editor::OnPlaymodeStateChanged(PlaymodeStateChangedEvent* event)
	{
		switch (event->State)
		{
		case PlaymodeState::EnterPlaymode:
		{
			Scene* activeScene = SceneManager::GetActiveScene();
			auto tempPath = ProjectManager::GetTempDirectory() / TEMP_SCENE_FILE;
			activeScene->SaveTo(tempPath);
			m_UpdateScripts = false;
			break;
		}
		case PlaymodeState::PausePlaymode:
		{
			m_UpdateScripts = true;
			break;
		}
		case PlaymodeState::ExitPlaymode:
		{
			auto tempPath = ProjectManager::GetTempDirectory() / TEMP_SCENE_FILE;
			SceneManager::LoadScene(tempPath.string());
			m_UpdateScripts = false;
			break;
		}
		default:
			break;
		}
	}
}