#include "GameObjectInspector.h"
#include "MeshRendererInspector.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ScriptComponent.h"
#include "imgui.h"
#include "PropertiesComponent.h"
#include "ScriptingManager.h"
#include "Animator.h"
#include "AnimatorInspector.h"
#include "LightInspector.h"

namespace Odyssey
{
	GameObjectInspector::GameObjectInspector(GUID guid)
	{
		m_Target = SceneManager::GetActiveScene()->GetGameObject(guid);
		CreateInspectors();
	}

	void GameObjectInspector::Draw()
	{
		// Don't draw unless we have a target
		if (!m_Target)
			return;

		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
			m_NameDrawer.Draw();

		ImGui::Spacing();

		for (auto& inspector : m_Inspectors)
		{
			inspector->Draw();
		}

		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.Draw();
		}

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component Popup");

		if (ImGui::BeginPopup("Add Component Popup"))
		{
			ImGui::SeparatorText("Components");

			std::vector<std::string> possibleComponents
			{
				"Animator",
				"Camera",
				"Light",
				"Mesh Renderer",
				"Transform",
			};

			auto scriptMetadatas = ScriptingManager::GetAllScriptMetadatas();

			for (auto& metadata : scriptMetadatas)
			{
				possibleComponents.push_back(metadata.Name);
			}

			uint64_t selected = 0;

			for (size_t i = 0; i < possibleComponents.size(); i++)
			{
				if (ImGui::Selectable(possibleComponents[i].c_str()))
				{
					selected = i;
					
					if (selected == 0)
					{
						if (!m_Target.HasComponent<Animator>())
							m_Target.AddComponent<Animator>();
					}
					else if (selected == 1)
					{
						if (!m_Target.HasComponent<Camera>())
							m_Target.AddComponent<Camera>();
					}
					else if (selected == 2)
					{
						if (!m_Target.HasComponent<Light>())
							m_Target.AddComponent<Light>();
					}
					else if (selected == 3)
					{
						if (!m_Target.HasComponent<MeshRenderer>())
							m_Target.AddComponent<MeshRenderer>();
					}
					else if (selected == 4)
					{
						if (!m_Target.HasComponent<Transform>())
							m_Target.AddComponent<Transform>();
					}
					else
					{
						if (!m_Target.HasComponent<ScriptComponent>())
							m_Target.AddComponent<ScriptComponent>();

						ScriptComponent& script = m_Target.GetComponent<ScriptComponent>();
						size_t scriptIndex = i - 4;
						script.SetScriptID(scriptMetadatas[scriptIndex].ScriptID);
					}

					CreateInspectors();
				}
			}

			ImGui::EndPopup();
		}
	}

	void GameObjectInspector::CreateInspectors()
	{
		m_Inspectors.clear();
		userScriptInspectors.clear();

		m_NameDrawer = StringDrawer("Name", m_Target.GetName(),
			[this](const std::string& name) { OnNameChanged(name); });

		if (m_Target.HasComponent<Animator>())
			m_Inspectors.push_back(std::make_unique<AnimatorInspector>(m_Target));

		if (m_Target.HasComponent<Transform>())
			m_Inspectors.push_back(std::make_unique<TransformInspector>(m_Target));

		if (m_Target.HasComponent<Camera>())
			m_Inspectors.push_back(std::make_unique<CameraInspector>(m_Target));

		if (m_Target.HasComponent<MeshRenderer>())
			m_Inspectors.push_back(std::make_unique<MeshRendererInspector>(m_Target));

		if (m_Target.HasComponent<ScriptComponent>())
			m_Inspectors.push_back(std::make_unique<UserScriptInspector>(m_Target));

		if (m_Target.HasComponent<Light>())
			m_Inspectors.push_back(std::make_unique<LightInspector>(m_Target));
	}

	void GameObjectInspector::RefreshUserScripts()
	{
		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.UpdateFields();
		}
	}

	void GameObjectInspector::OnNameChanged(const std::string& name)
	{
		if (m_Target.HasComponent<PropertiesComponent>())
			m_Target.SetName(name);
	}
}