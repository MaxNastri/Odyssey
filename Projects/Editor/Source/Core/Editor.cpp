#include "Editor.h"
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
#include "DebugRenderer.h"
#include "SceneSettingsWindow.h"
#include "AssetRegistry.h"
#include "ParticleBatcher.h"
#include "Preferences.h"
#include "FileManager.h"

namespace Odyssey
{
	Editor::Editor()
	{
		// Its important we initialize scripting first due to a bug with VS2022
		// With native debugging enabled, our breakpoints wont work before we init scripting
		ScriptingManager::Initialize();
		Random::Initialize();
		FileManager::Init();

		// Register for event listeners
		m_BuildCompleteListener = EventSystem::Listen<BuildCompleteEvent>
			([this](BuildCompleteEvent* event) { OnBuildComplete(event); });
		m_PlaymodeStateListener = EventSystem::Listen<PlaymodeStateChangedEvent>
			([this](PlaymodeStateChangedEvent* event) { OnPlaymodeStateChanged(event); });

		Preferences::LoadPreferences("Resources/Editor.prefs");
		Preferences::SavePreferences();

		// Load the default project
		Project::LoadProject(Preferences::GetStartupProject());

		{
			// Create the asset database
			AssetManager::Settings settings;
			settings.AssetsDirectory = Project::GetActiveAssetsDirectory();
			settings.AdditionalRegistries = { Preferences::GetEditorRegistry() };
			settings.AssetExtensions = Preferences::GetAssetExtensions();
			settings.SourceAssetExtensionMap = Preferences::GetSourceExtensionsMap();
			AssetManager::CreateDatabase(settings);
		}

		// Create the renderer
		RendererConfig config = { .EnableIMGUI = true };
		Renderer::Init(config);
		Renderer::SetDrawGUIListener(GUIManager::DrawGUI);

		// Setup debug renderer
		DebugRenderer::Settings debugSettings;
		debugSettings.MaxVertices = 128000;
		DebugRenderer::Init(debugSettings);

		GUIManager::Initialize();

		{
			// Create the script compiler
			ScriptCompiler::Settings settings;
			settings.ApplicationPath = Globals::GetApplicationPath();
			settings.CacheDirectory = Project::GetActiveCacheDirectory();
			settings.UserScriptsDirectory = Project::GetActiveUserScriptsDirectory();
			settings.UserScriptsProject = Project::GetActiveUserScriptsProject();
			m_ScriptCompiler = std::make_unique<ScriptCompiler>(settings);
		}

		// Build the user assembly
		ScriptingManager::SetUserAssembliesPath(m_ScriptCompiler->GetUserAssemblyPath());
		ScriptingManager::LoadUserAssemblies();

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

				FileManager::Get().Dispatch();

				// Process any changes made to the user's managed dll
				m_ScriptCompiler->Process();
				DebugRenderer::Clear();
				GUIManager::Update();

				SceneManager::OnEditorUpdate();

				// Only update the scene if we are updating scripts (playmode)
				if (m_UpdateScripts)
					SceneManager::Update();

				running = Renderer::Update();
				Renderer::Render();
			}
		}

		Renderer::Destroy();
		ScriptingManager::Destroy();
	}

	void Editor::Exit()
	{
		running = false;
	}

	void Editor::SetupEditorGUI()
	{
		GUIManager::CreateInspectorWindow();
		GUIManager::CreateDockableWindow<GameViewWindow>();
		GUIManager::CreateDockableWindow<SceneViewWindow>();
		GUIManager::CreateDockableWindow<ContentBrowserWindow>();
		GUIManager::CreateDockableWindow<SceneHierarchyWindow>();
		GUIManager::CreateDockableWindow<SceneSettingsWindow>();
	}

	void Editor::OnPlaymodeStateChanged(PlaymodeStateChangedEvent* event)
	{
		switch (event->State)
		{
			case PlaymodeState::EnterPlaymode:
			{
				Scene* activeScene = SceneManager::GetActiveScene();
				activeScene->OnStartRuntime();
				activeScene->Awake();

				m_UpdateScripts = true;
				break;
			}
			case PlaymodeState::PausePlaymode:
			{
				m_UpdateScripts = false;
				break;
			}
			case PlaymodeState::ExitPlaymode:
			{
				Scene* activeScene = SceneManager::GetActiveScene();
				activeScene->OnStopRuntime();

				// Slight hack here, we just load the scene off disk again
				const Path& scenePath = activeScene->GetPath();
				SceneManager::LoadScene(scenePath);

				m_UpdateScripts = false;
				break;
			}
		}
	}
	void Editor::OnBuildComplete(BuildCompleteEvent* event)
	{
		if (event->success)
		{
			ScriptingManager::ReloadAssemblies();
		}
	}
}