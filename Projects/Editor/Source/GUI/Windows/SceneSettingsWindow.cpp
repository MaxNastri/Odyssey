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
		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });

		GUID skyboxGUID = m_Scene ? m_Scene->GetEnvironmentSettings().Skybox->GetGUID() : GUID(0);
		m_SkyboxDrawer = AssetFieldDrawer("Skybox", skyboxGUID, Cubemap::Type);

		float3 ambientColor = m_Scene ? m_Scene->GetEnvironmentSettings().AmbientColor : float3(0.0f);
		m_AmbientColorPicker = ColorPicker("Ambient Color", ambientColor);

		float3 sceneCenter = m_Scene ? m_Scene->GetEnvironmentSettings().SceneCenter : float3(0.0f);
		m_SceneCenterDrawer = Vector3Drawer("Scene Center", sceneCenter, float3(0.0f), false);

		float sceneRadius = m_Scene ? m_Scene->GetEnvironmentSettings().SceneRadius : 0.0f;
		m_SceneRadiusDrawer = FloatDrawer("Scene Radius", sceneRadius);
	}

	bool SceneSettingsWindow::Draw()
	{
		bool modified = false;

		if (!Begin())
			return modified;

		if (m_Scene)
		{
			if (m_SkyboxDrawer.Draw())
			{
				if (m_Scene)
					m_Scene->GetEnvironmentSettings().SetSkybox(m_SkyboxDrawer.GetGUID());
			}

			if (m_AmbientColorPicker.Draw())
			{
				if (m_Scene)
					m_Scene->GetEnvironmentSettings().AmbientColor = m_AmbientColorPicker.GetColor3();
			}

			if (m_SceneCenterDrawer.Draw())
			{
				if (m_Scene)
					m_Scene->GetEnvironmentSettings().SceneCenter = m_SceneCenterDrawer.GetValue();
			}

			if (m_SceneRadiusDrawer.Draw())
			{
				if (m_Scene)
					m_Scene->GetEnvironmentSettings().SceneRadius = m_SceneRadiusDrawer.GetValue();
			}
		}

		End();
		return modified;
	}

	void SceneSettingsWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void SceneSettingsWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		m_Scene = event->loadedScene;

		m_SkyboxDrawer.SetGUID(m_Scene->GetEnvironmentSettings().Skybox->GetGUID());
		m_AmbientColorPicker.SetColor(m_Scene->GetEnvironmentSettings().AmbientColor);
	}
}