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
#include "ScriptInspector.h"
#include "Events.h"

namespace Odyssey
{
	inline static bool s_ComponentsRegistered = false;
	inline static std::unordered_map<std::string, std::function<void(GameObject&)>> s_AddComponentFuncs;
	inline static std::map<uint32_t, std::function<std::unique_ptr<Inspector>(GameObject&)>> s_CreateInspectorFuncs;

	template<typename ComponentType, typename InspectorType>
	void RegisterComponentType(uint32_t priority)
	{
		static_assert(std::is_base_of<Inspector, InspectorType>::value, "InspectorType is not a dervied class of Inspector.");

		// Register an add component function
		if (!s_AddComponentFuncs.contains(ComponentType::ClassName))
		{
			s_AddComponentFuncs[ComponentType::ClassName] = [](GameObject& gameObject)
				{
					if (!gameObject.HasComponent<ComponentType>())
						gameObject.AddComponent<ComponentType>();
				};

			// No duplicate priorities
			if (s_CreateInspectorFuncs.contains(priority))
			{
				Log::Warning("[GameObjectInspector] Duplicate priority (" + std::to_string(priority) + " detected for " + ComponentType::ClassName);
			}

			s_CreateInspectorFuncs[priority] = [](GameObject& gameObject)
				{
					if (gameObject.HasComponent<ComponentType>())
						return std::make_unique<InspectorType>(gameObject);

					return std::unique_ptr<InspectorType>{};
				};
		}
	}

	// Helper function for when we want to register at lowest current priority
	template<typename ComponentType, typename InspectorType>
	void RegisterComponentType()
	{
		static uint32_t lowPri = 0;

		if (!s_CreateInspectorFuncs.empty())
		{
			lowPri = std::prev(s_CreateInspectorFuncs.end())->first + 1;
		}

		RegisterComponentType<ComponentType, InspectorType>(lowPri);
	}

	GameObjectInspector::GameObjectInspector(GUID guid)
	{
		m_TargetGUID = guid;
		m_Target = SceneManager::GetActiveScene()->GetGameObject(guid);

		// Note: Priority parameter determines the display order
		if (!s_ComponentsRegistered)
		{
			RegisterComponentType<Transform, TransformInspector>();
			RegisterComponentType<Camera, CameraInspector>();
			RegisterComponentType<Light, LightInspector>();
			RegisterComponentType<MeshRenderer, MeshRendererInspector>();
			RegisterComponentType<Animator, AnimatorInspector>();
			RegisterComponentType<ParticleEmitter, ParticleEmitterInspector>();
			RegisterComponentType<ScriptComponent, ScriptInspector>();
			s_ComponentsRegistered = true;
		}

		auto onSceneModified = [this](SceneModifiedEvent* eventData) { OnSceneModified(eventData); };
		EventSystem::Listen<SceneModifiedEvent>(onSceneModified);

		CreateInspectors();
	}

	bool GameObjectInspector::Draw()
	{
		bool modified = false;

		// Don't draw unless we have a target
		if (!m_Target.IsValid())
			return modified;

		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
			m_NameDrawer.Draw();

		ImGui::Spacing();

		for (auto& inspector : m_Inspectors)
		{
			modified |= inspector->Draw();
		}

		for (auto& userScriptInspector : userScriptInspectors)
		{
			modified |= userScriptInspector.Draw();
		}

		ImGui::Spacing();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component Popup");

		if (ImGui::BeginPopup("Add Component Popup"))
		{
			auto kv = std::views::keys(s_AddComponentFuncs);
			std::vector<std::string> possibleComponents{ kv.begin(), kv.end() };

			uint64_t selected = 0;

			for (size_t i = 0; i < possibleComponents.size(); i++)
			{
				const std::string& componentName = possibleComponents[i];
				if (ImGui::Selectable(componentName.c_str()))
				{
					selected = i;
					s_AddComponentFuncs[componentName](m_Target);
					CreateInspectors();
					modified = true;
				}
			}

			// No component was selected
			if (selected == 0)
			{
				auto& scriptMetadatas = ScriptingManager::GetAllScriptMetadatas();

				for (auto& [scriptID, metadata] : scriptMetadatas)
				{
					if (ImGui::Selectable(metadata.Name.c_str()))
					{
						if (!m_Target.HasComponent<ScriptComponent>())
							m_Target.AddComponent<ScriptComponent>();

						ScriptComponent& script = m_Target.GetComponent<ScriptComponent>();
						script.SetScriptID(metadata.ScriptID);
						CreateInspectors();
						modified = true;
					}
				}
			}

			ImGui::EndPopup();
		}

		return modified;
	}

	void GameObjectInspector::CreateInspectors()
	{
		m_Inspectors.clear();
		userScriptInspectors.clear();

		m_NameDrawer = StringDrawer("Name", m_Target.GetName(), false,
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
	void GameObjectInspector::OnSceneModified(SceneModifiedEvent* eventData)
	{
		m_Target = eventData->Scene->GetGameObject(m_TargetGUID);

		if (!m_Target.IsValid())
		{
			m_Inspectors.clear();
			userScriptInspectors.clear();
		}
	}
}