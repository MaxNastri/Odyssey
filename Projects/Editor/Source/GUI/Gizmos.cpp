#include "Gizmos.h"
#include "Components.h"
#include "SceneManager.h"
#include "Events.h"

namespace Odyssey
{
#define REGISTER_GIZMO(Component) m_Gizmos[Component::ClassName].OnGizmoModified = [](bool enabled) { ToggleGizmo<Component>(enabled);}

	template<typename T>
	void ToggleGizmo(bool enabled)
	{
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			auto view = activeScene->GetAllEntitiesWith<T>();
			for (auto& entity : view)
			{
				GameObject gameObject = GameObject(activeScene, entity);
				T& component = gameObject.GetComponent<T>();
				component.SetDebugEnabled(enabled);
			}
		}
	}

	void Gizmos::Init()
	{
		m_Gizmos.clear();
		REGISTER_GIZMO(Animator);
		REGISTER_GIZMO(BoxCollider);
		REGISTER_GIZMO(SphereCollider);
		REGISTER_GIZMO(CapsuleCollider);
		REGISTER_GIZMO(CharacterController);
		REGISTER_GIZMO(FluidBody);

		m_OnSceneModified = EventSystem::Listen<SceneModifiedEvent>(OnSceneModified);
		m_OnSceneLoaded = EventSystem::Listen<SceneLoadedEvent>(OnSceneLoaded);
	}

	void Gizmos::OnSceneModified(SceneModifiedEvent* onEvent)
	{
		if (onEvent->Modification == SceneModifiedEvent::Modification::CreateGameObject ||
			onEvent->Modification == SceneModifiedEvent::Modification::AddComponent)
		{
			for (auto& [componentName, info] : m_Gizmos)
			{
				info.OnGizmoModified(info.Enabled);
			}
		}
	}
	void Gizmos::OnSceneLoaded(SceneLoadedEvent* onEvent)
	{
		for (auto& [componentName, info] : m_Gizmos)
		{
			info.OnGizmoModified(info.Enabled);
		}
	}
}