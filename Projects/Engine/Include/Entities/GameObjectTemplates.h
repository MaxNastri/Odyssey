#include "GameObject.h"
#pragma once

namespace Odyssey
{
	template<typename T, typename ...Args>
	inline T& GameObject::AddComponent(Args && ...params)
	{
		return m_Scene->m_Registry.emplace<T>(m_Entity, *this, std::forward<Args>(params)...);
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
		return m_Scene->m_Registry.remove<T>(m_Entity);
	}
}