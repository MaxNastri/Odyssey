#include "SceneSettingsWindow.h"
#include "EventSystem.h"
#include "Events.h"
#include "EditorEvents.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "imgui.h"
#include "Cubemap.h"

namespace Odyssey
{
	SceneSettingsWindow::SceneSettingsWindow(size_t windowID)
		: DockableWindow("Scene Settings", windowID,
			glm::vec2(0, 0), glm::vec2(400, 450), glm::vec2(2, 2))
	{
		m_Scene = SceneManager::GetActiveScene();
		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>(
			[this](SceneLoadedEvent* event) { OnSceneLoaded(event); });

		GUID skyboxGUID = m_Scene ? m_Scene->GetEnvironmentSettings().Skybox : GUID(0);
		m_SkyboxDrawer = AssetFieldDrawer("Skybox", skyboxGUID, Cubemap::Type,
			[this](GUID skyboxGUID) { OnSkyboxChanged(skyboxGUID); });

		glm::vec3 ambientColor = m_Scene ? m_Scene->GetEnvironmentSettings().AmbientColor : glm::vec3(0.0f);
		m_AmbientColorPicker = ColorPicker("Ambient Color", ambientColor,
			[this](glm::vec3 color) { OnAmbientColorChanged(color); });
	}

	void SceneSettingsWindow::Draw()
	{
		if (!Begin())
			return;

		if (m_Scene)
		{
			m_SkyboxDrawer.Draw();
			m_AmbientColorPicker.Draw();
		}

		End();
	}

	void SceneSettingsWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void SceneSettingsWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		m_Scene = event->loadedScene;
		m_SkyboxDrawer.SetGUID(m_Scene->GetEnvironmentSettings().Skybox);
		m_AmbientColorPicker.SetColor(m_Scene->GetEnvironmentSettings().AmbientColor);
	}

	void SceneSettingsWindow::OnSkyboxChanged(GUID skyboxGUID)
	{
		if (m_Scene)
			m_Scene->GetEnvironmentSettings().Skybox = skyboxGUID;
	}
	void SceneSettingsWindow::OnAmbientColorChanged(glm::vec3 color)
	{
		if (m_Scene)
			m_Scene->GetEnvironmentSettings().AmbientColor = color;
	}
}