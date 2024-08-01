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
#include "Project.h"
#include "Renderer.h"

namespace Odyssey
{
	Editor::Editor()
	{
		// Its important we initialize scripting first due to a bug with VS2022
		// With native debugging enabled, our breakpoints wont work before we init scripting
		ScriptingManager::Initialize();
		Random::Initialize();
		FileTracker::Init();

		// Register for event listeners
		m_BuildCompleteListener = EventSystem::Listen<BuildCompleteEvent>
			([this](BuildCompleteEvent* event) { OnBuildComplete(event); });
		m_PlaymodeStateListener = EventSystem::Listen<PlaymodeStateChangedEvent>
			([this](PlaymodeStateChangedEvent* event) { OnPlaymodeStateChanged(event); });
		
		// Load the default project
		Project::LoadProject("C:/Git/Odyssey/Managed/ExampleProject");

		// Create the renderer
		RendererConfig config;
		config.EnableIMGUI = true;
		Renderer::Init(config);
		Renderer::SetDrawGUIListener(GUIManager::DrawGUI);
		GUIManager::Initialize();

		AssetManager::CreateDatabase(Project::GetActiveAssetsDirectory(), Project::GetActiveCacheDirectory());

		// Build the user assembly
		ScriptCompiler::Settings settings;
		settings.ApplicationPath = Globals::GetApplicationPath();
		settings.CacheDirectory = Project::GetActiveCacheDirectory();
		settings.UserScriptsDirectory = Project::GetActiveUserScriptsDirectory();
		settings.UserScriptsProject = Project::GetActiveUserScriptsProject();
		m_ScriptCompiler = std::make_unique<ScriptCompiler>(settings);

		ScriptingManager::SetUserAssembliesPath(m_ScriptCompiler->GetUserAssemblyPath());
		m_ScriptCompiler->BuildUserAssembly();

		SetupEditorGUI();

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

				running = Renderer::Update();
				Renderer::Render();
			}
		}

		Renderer::Destroy();
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

	void Editor::OnPlaymodeStateChanged(PlaymodeStateChangedEvent* event)
	{
		switch (event->State)
		{
		case PlaymodeState::EnterPlaymode:
		{
			Scene* activeScene = SceneManager::GetActiveScene();
			Path tempPath = Project::GetActiveTempDirectory() / TEMP_SCENE_FILE;
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
			Path tempPath = Project::GetActiveTempDirectory() / TEMP_SCENE_FILE;
			SceneManager::LoadScene(tempPath.string());
			m_UpdateScripts = false;
			break;
		}
		default:
			break;
		}
	}
	void Editor::OnBuildComplete(BuildCompleteEvent* event)
	{
		if (event->success)
		{
			ScriptingManager::ReloadUserAssemblies();
		}
	}
}