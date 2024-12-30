#pragma once
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "String.hpp"
#include "glm.h"
#include "Log.h"
#include "OdysseyTime.h"
#include "AssetManager.h"
#include "Mesh.h"
#include "Animator.h"
#include "Input.h"
#include "SpriteRenderer.h"

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
			Log::Error(std::format("No C# component class found for {}!", componentType));
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

#pragma region Animator

	bool Animator_IsEnabled(uint64_t gameObjectGUID)
	{
		GameObject gameObject = GetGameObject(gameObjectGUID);

		if (Animator* animator = gameObject.TryGetComponent<Animator>())
			return animator->IsEnabled();

		return false;
	}

	void Animator_SetFloat(uint64_t gameObjectGUID, Coral::String propertyName, float value)
	{
		std::string property = propertyName;
		GameObject gameObject = GetGameObject(gameObjectGUID);

		if (Animator* animator = gameObject.TryGetComponent<Animator>())
			animator->SetFloat(propertyName, value);
	}

	void Animator_SetBool(uint64_t gameObjectGUID, Coral::String propertyName, bool value)
	{
		std::string property = propertyName;
		GameObject gameObject = GetGameObject(gameObjectGUID);

		if (Animator* animator = gameObject.TryGetComponent<Animator>())
			animator->SetBool(propertyName, value);
	}

	void Animator_SetInt(uint64_t gameObjectGUID, Coral::String propertyName, int32_t value)
	{
		std::string property = propertyName;
		GameObject gameObject = GetGameObject(gameObjectGUID);

		if (Animator* animator = gameObject.TryGetComponent<Animator>())
			animator->SetInt(propertyName, value);
	}

	void Animator_SetTrigger(uint64_t gameObjectGUID, Coral::String propertyName)
	{
		std::string property = propertyName;
		GameObject gameObject = GetGameObject(gameObjectGUID);

		if (Animator* animator = gameObject.TryGetComponent<Animator>())
			animator->SetTrigger(propertyName);
	}

#pragma endregion

#pragma region GameObject

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
		std::string nameStr = name;
		gameObject.SetName(nameStr);
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

#pragma endregion

#pragma region Transform
	
	void Transform_GetPosition(uint64_t guid, glm::vec3* position)
	{
		if (!guid)
		{
			Log::Error("[InternalCalls] Invalid GUID detected for Transform_GetPosition.");
			return;
		}

		GameObject gameObject = GetGameObject(guid);

		if (Transform* transform = gameObject.TryGetComponent<Transform>())
			*position = transform->GetPosition();
	}

	void Transform_SetPosition(uint64_t guid, glm::vec3 position)
	{
		if (!guid)
		{
			Log::Error("[InternalCalls] Invalid GUID detected for Transform_GetPosition.");
			return;
		}

		GameObject gameObject = GetGameObject(guid);

		if (Transform* transform = gameObject.TryGetComponent<Transform>())
			transform->SetPosition(position);
	}

	void Transform_GetEulerAngles(uint64_t guid, glm::vec3* rotation)
	{
		GameObject gameObject = GetGameObject(guid);

		if (Transform* transform = gameObject.TryGetComponent<Transform>())
			*rotation = transform->GetEulerRotation();
	}

	void Transform_SetEulerAngles(uint64_t guid, glm::vec3 rotation)
	{
		GameObject gameObject = GetGameObject(guid);

		if (Transform* transform = gameObject.TryGetComponent<Transform>())
			transform->SetRotation(rotation);
	}

	void Transform_GetScale(uint64_t guid, glm::vec3* scale)
	{
		GameObject gameObject = GetGameObject(guid);

		if (Transform* transform = gameObject.TryGetComponent<Transform>())
			*scale = transform->GetScale();
	}

	void Transform_SetScale(uint64_t guid, glm::vec3 scale)
	{
		GameObject gameObject = GetGameObject(guid);

		if (Transform* transform = gameObject.TryGetComponent<Transform>())
			transform->SetScale(scale);
	}

#pragma endregion

#pragma region Mesh

	uint32_t Mesh_GetVertexCount(uint64_t guid)
	{
		if (Ref<Mesh> mesh = AssetManager::LoadAsset<Mesh>(GUID(guid)))
			return mesh->GetVertexCount();

		return 0;
	}

#pragma endregion

#pragma region Mesh Renderer

	uint64_t MeshRenderer_GetMesh(uint64_t entityGUID)
	{
		GameObject gameObject = GetGameObject(entityGUID);

		if (MeshRenderer* renderer = gameObject.TryGetComponent<MeshRenderer>())
			renderer->GetMesh();

		return GUID();
	}

	void MeshRenderer_SetMesh(uint64_t entityGUID, uint64_t meshGUID)
	{
		GameObject gameObject = GetGameObject(entityGUID);

		if (MeshRenderer* renderer = gameObject.TryGetComponent<MeshRenderer>())
			renderer->SetMesh(GUID(meshGUID));
	}

#pragma endregion

#pragma region Sprite Renderer

	void SpriteRenderer_SetFill(uint64_t entityGUID, float2 fill)
	{
		GameObject gameObject = GetGameObject(entityGUID);

		if (SpriteRenderer* spriteRenderer = gameObject.TryGetComponent<SpriteRenderer>())
			spriteRenderer->SetFill(fill);
	}

	void SpriteRenderer_GetFill(uint64_t entityGUID, float2* fill)
	{
		GameObject gameObject = GetGameObject(entityGUID);

		if (SpriteRenderer* spriteRenderer = gameObject.TryGetComponent<SpriteRenderer>())
			*fill = spriteRenderer->GetFill();
	}

	void SpriteRenderer_SetBaseColor(uint64_t entityGUID, float4 color)
	{
		GameObject gameObject = GetGameObject(entityGUID);

		if (SpriteRenderer* spriteRenderer = gameObject.TryGetComponent<SpriteRenderer>())
			spriteRenderer->SetBaseColor(color);
	}

	void SpriteRenderer_GetBaseColor(uint64_t entityGUID, float4* color)
	{
		GameObject gameObject = GetGameObject(entityGUID);

		if (SpriteRenderer* spriteRenderer = gameObject.TryGetComponent<SpriteRenderer>())
			*color = spriteRenderer->GetBaseColor();
	}

#pragma endregion

#pragma region Input

	bool Input_GetKeyPress(uint32_t key)
	{
		return Input::GetKeyPress((KeyCode)key);
	}

	bool Input_GetKeyDown(uint32_t key)
	{
		return Input::GetKeyDown((KeyCode)key);
	}

	bool Input_GetKeyUp(uint32_t key)
	{
		return Input::GetKeyUp((KeyCode)key);
	}

	bool Input_GetMouseButtonDown(uint32_t button)
	{
		return Input::GetMouseButtonDown((MouseButton)button);
	}

	float Input_GetMouseAxisHorizontal()
	{
		return Input::GetMouseAxisHorizontal();
	}

	float Input_GetMouseAxisVertical()
	{
		return Input::GetMouseAxisVertical();
	}

	float2 Input_GetMousePosition()
	{
		return Input::GetMousePosition();
	}

#pragma endregion

#pragma region Time

	float Time_GetDeltaTime()
	{
		return Time::DeltaTime();
	}

#pragma endregion
}