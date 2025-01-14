#include "GameObject.h"

#pragma once

namespace Odyssey
{
	template<typename T, typename ...Args>
	inline T& GameObject::AddComponent(Args && ...params)
	{
		T& component = m_Scene->AddComponent<T>(*this, std::forward<Args>(params)...);
		return component;
	}

	template<typename T>
	inline T& GameObject::GetComponent()
	{
		return m_Scene->m_Registry.get<T>(m_Entity);
	}

	template<typename T>
	inline const T& GameObject::GetComponent() const
	{
		return m_Scene->m_Registry.get<T>(m_Entity);
	}

	template<typename T>
	inline T* GameObject::TryGetComponent()
	{
		return m_Scene->m_Registry.try_get<T>(m_Entity);
	}

	template<typename T>
	inline const T* GameObject::TryGetComponent() const
	{
		return m_Scene->m_Registry.try_get<T>(m_Entity);
	}

	template<typename... T>
	inline bool GameObject::HasComponent()
	{
		return m_Scene->m_Registry.any_of<T...>(m_Entity);
	}

	template<typename T>
	inline bool GameObject::RemoveComponent()
	{
		bool remove = m_Scene->m_Registry.remove<T>(m_Entity);
		EventSystem::Dispatch<SceneModifiedEvent>(m_Scene);
		return remove;
	}
}