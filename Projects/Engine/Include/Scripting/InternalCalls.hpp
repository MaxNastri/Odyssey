#pragma once
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "String.hpp"
#include "Logger.h"

namespace Odyssey
{
	std::map<Coral::TypeId, std::function<void(GameObject&)>> s_AddComponentFuncs;
	std::map<Coral::TypeId, std::function<bool(GameObject&)>> s_HasComponentFuncs;
	std::map<Coral::TypeId, std::function<bool(GameObject&)>> s_RemoveComponentFuncs;

	template<typename TComponent>
	static void RegisterComponentType(Coral::ManagedAssembly& frameworkAssembly, const std::string& componentType)
	{
		Coral::Type& type = frameworkAssembly.GetType(componentType);

		if (type)
		{
			Coral::TypeId typeID = type.GetTypeId();
			s_AddComponentFuncs[typeID] = [](GameObject& gameObject) { gameObject.AddComponent<TComponent>(); };
			s_HasComponentFuncs[typeID] = [](GameObject& gameObject) { return gameObject.HasComponent<TComponent>(); };
			s_RemoveComponentFuncs[typeID] = [](GameObject& gameObject) { return gameObject.RemoveComponent<TComponent>(); };
		}
		else
		{
			Logger::LogError(std::format("No C# component class found for {}!", componentType));
		}
	}
}

namespace Odyssey::InternalCalls
{
	static GameObject GetGameObject(uint64_t guid)
	{
		Scene* activeScene = SceneManager::GetActiveScene();
		GameObject gameObject = activeScene->GetGameObject(guid);
		return gameObject;
	}

	uint64_t GameObject_Create()
	{
		Scene* activeScene = SceneManager::GetActiveScene();
		GameObject gameObject = activeScene->CreateGameObject();
		return gameObject.GetGUID();
	}

	Coral::String GameObject_GetName(uint64_t guid)
	{
		GameObject gameObject = GetGameObject(guid);
		return Coral::String::New(gameObject.GetName());
	}

	void GameObject_SetName(uint64_t guid, Coral::String name)
	{
		GameObject gameObject = GetGameObject(guid);
		gameObject.SetName(name);
	}

	void GameObject_AddComponent(uint64_t guid, Coral::ReflectionType componentType)
	{
		GameObject gameObject = GetGameObject(guid);
		
		if (Coral::Type& type = componentType)
		{
			// Find the HasComponent function for this component type and check that it is valid
			if (auto hasComponentFunc = s_HasComponentFuncs.find(type.GetTypeId()); hasComponentFunc != s_HasComponentFuncs.end())
			{
				// Check if the game object has this componentype already
				if (hasComponentFunc->second(gameObject))
					return;
			}

			// Find the AddComponent function for this component type and check that it is valid
			if (auto addComponentFunc = s_AddComponentFuncs.find(type.GetTypeId()); addComponentFunc != s_AddComponentFuncs.end())
			{
				// Execute the add component function on the game object
				addComponentFunc->second(gameObject);
			}
		}
	}

	bool GameObject_HasComponent(uint64_t guid, Coral::ReflectionType componentType)
	{
		GameObject gameObject = GetGameObject(guid);

		if (Coral::Type& type = componentType)
		{
			// Find the HasComponent function for this component type and check that it is valid
			if (auto hasComponentFunc = s_HasComponentFuncs.find(type.GetTypeId()); hasComponentFunc != s_HasComponentFuncs.end())
			{
				// Execute the HasComponent function on the game object
				return hasComponentFunc->second(gameObject);
			}
		}

		return false;
	}

	bool GameObject_RemoveComponent(uint64_t guid, Coral::ReflectionType componentType)
	{
		GameObject gameObject = GetGameObject(guid);

		if (Coral::Type& type = componentType)
		{
			// Find the RemoveComponent function for this component type and check that it is valid
			if (auto removeComponentFunc = s_RemoveComponentFuncs.find(type.GetTypeId()); removeComponentFunc != s_RemoveComponentFuncs.end())
			{
				// Execute the RemoveComponent function on the game object
				return removeComponentFunc->second(gameObject);
			}
		}

		return false;
	}
}