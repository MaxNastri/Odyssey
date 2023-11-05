#pragma once
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <memory>
#include "ComponentArray.h"
#include "GameObject.h"

namespace Odyssey::Entities
{
	class ComponentManager
	{
	public:
		template<typename T, typename... Args>
		static T* AddComponent(const GameObject& gameObject, Args&&... params)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			componentArray->InsertData(gameObject.id, params...);
			return componentArray->GetComponentData(gameObject.id);
		}

		template<typename T>
		static void RemoveComponent(const GameObject& gameObject)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			if (componentArray)
			{
				componentArray->RemoveGameObject(gameObject.id);
			}
		}

		template<typename T>
		static T* GetComponent(const GameObject& gameObject)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->GetComponentData(gameObject.id);
		}

	private:
		template<typename T>
		static ComponentArray<T>* GetComponentArray()
		{
			std::type_index typeIndex = typeid(T);
			if (componentArrays.find(typeIndex) == componentArrays.end())
			{
				componentArrays[typeIndex] = new ComponentArray<T>();
			}
			return reinterpret_cast<ComponentArray<T>*>(componentArrays[typeIndex]);
		}
	private:
		static std::unordered_map<std::type_index, unsigned int> componentTypes;
		static std::unordered_map<std::type_index, IComponentArray*> componentArrays;
	};
}