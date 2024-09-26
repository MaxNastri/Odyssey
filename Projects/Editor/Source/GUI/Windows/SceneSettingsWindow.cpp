#include "SceneSettingsWindow.h"
#include "EventSystem.h"
#include "Events.h"
#include "EditorEvents.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "imgui.h"

#include "GeometryUtil.h"
#include "AssetManager.h"
#include "Mesh.h"

namespace Odyssey
{
	SceneSettingsWindow::SceneSettingsWindow(size_t windowID)
		: DockableWindow("Scene Settings", windowID,
			glm::vec2(0, 0), glm::vec2(400, 450), glm::vec2(2, 2))
	{
		m_Scene = SceneManager::GetActiveScene();
		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });

		m_SkyboxDrawer = AssetFieldDrawer("Skybox", 0, "Cubemap", 
			[this](GUID skyboxGUID) { OnSkyboxChanged(skyboxGUID); });
	}

	void SceneSettingsWindow::Draw()
	{
		if (!Begin())
			return;

		m_SkyboxDrawer.Draw();

		End();
	}

	void SceneSettingsWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void SceneSettingsWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		m_Scene = event->loadedScene;
	}

	void SceneSettingsWindow::OnSkyboxChanged(GUID skyboxGUID)
	{
		if (m_Scene)
			m_Scene->GetEnvironmentSettings().Skybox = skyboxGUID;
	}
}