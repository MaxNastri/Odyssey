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
#include "AnimatorInspector.h"
#include "LightInspector.h"
#include "ParticleEmitterInspector.h"

namespace Odyssey
{
	inline static std::unordered_map<std::string, std::function<void(GameObject&)>> s_AddComponentFuncs;
	inline static std::map<uint32_t, std::function<std::unique_ptr<Inspector>(GameObject&)>> s_CreateInspectorFuncs;

	template<typename ComponentType, typename InspectorType>
	void RegisterComponentType(uint32_t priority)
	{
		static_assert(std::is_base_of<Inspector, InspectorType>::value, "InspectorType is not a dervied class of Inspector.");

		if (!s_AddComponentFuncs.contains(ComponentType::ClassName))
		{
			s_AddComponentFuncs[ComponentType::ClassName] = [](GameObject& gameObject)
				{
					if (!gameObject.HasComponent<ComponentType>())
						gameObject.AddComponent<ComponentType>();
				};
		}

		if (!s_CreateInspectorFuncs.contains(priority))
		{
			s_CreateInspectorFuncs[priority] = [](GameObject& gameObject)
				{
					if (gameObject.HasComponent<ComponentType>())
						return std::make_unique<InspectorType>(gameObject);

					return std::unique_ptr<InspectorType>{};
				};
		}
	}

	GameObjectInspector::GameObjectInspector(GUID guid)
	{
		m_Target = SceneManager::GetActiveScene()->GetGameObject(guid);

		// Note: Priority parameter determines the display order
		RegisterComponentType<Transform, TransformInspector>(0);
		RegisterComponentType<Camera, CameraInspector>(1);
		RegisterComponentType<Light, LightInspector>(2);
		RegisterComponentType<MeshRenderer, MeshRendererInspector>(3);
		RegisterComponentType<Animator, AnimatorInspector>(4);
		RegisterComponentType<ParticleEmitter, ParticleEmitterInspector>(5);

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

			auto kv = std::views::keys(s_AddComponentFuncs);
			std::vector<std::string> possibleComponents{ kv.begin(), kv.end() };

			auto scriptMetadatas = ScriptingManager::GetAllScriptMetadatas();

			for (auto& metadata : scriptMetadatas)
			{
				possibleComponents.push_back(metadata.Name);
			}

			uint64_t selected = 0;

			for (size_t i = 0; i < possibleComponents.size(); i++)
			{
				const std::string& componentName = possibleComponents[i];
				if (ImGui::Selectable(componentName.c_str()))
				{
					selected = i;

					if (s_AddComponentFuncs.contains(componentName))
					{
						s_AddComponentFuncs[componentName](m_Target);
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
			[this](std::string_view name) { OnNameChanged(name); });

		for (auto& [className, createInspectorFunc] : s_CreateInspectorFuncs)
		{
			auto inspector = createInspectorFunc(m_Target);
			if (inspector)
				m_Inspectors.push_back(std::move(inspector));
		}
	}

	void GameObjectInspector::RefreshUserScripts()
	{
		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.UpdateFields();
		}
	}

	void GameObjectInspector::OnNameChanged(std::string_view name)
	{
		if (m_Target.HasComponent<PropertiesComponent>())
			m_Target.SetName(name);
	}
}