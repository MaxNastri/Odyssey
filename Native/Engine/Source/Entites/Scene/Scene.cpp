#include "Scene.h"
#include "ComponentManager.h"
#include <fstream>
#include <string>
#include "AssetSerializer.h"

namespace Odyssey
{
	Scene::Scene()
	{
		awakeFunc = [](Component* component) { component->Awake(); };
		updateFunc = [](Component* component) { component->Update(); };
		onDestroyFunc = [](Component* component) { component->OnDestroy(); };
		nextGameObjectID = 0;
	}

	Scene::Scene(const std::filesystem::path& assetPath)
		: Asset(assetPath)
	{
		awakeFunc = [](Component* component) { component->Awake(); };
		updateFunc = [](Component* component) { component->Update(); };
		onDestroyFunc = [](Component* component) { component->OnDestroy(); };
		nextGameObjectID = 0;

		LoadFromDisk(assetPath);
	}

	GameObject* Scene::CreateGameObject()
	{
		// Create a new game object
		GameObject* gameObject = CreateEmptyGameObject();

		// Automatically add a transform component for new game objects
		gameObject->AddComponent<Transform>();

		return gameObject;
	}

	void Scene::DestroyGameObject(GameObject* gameObject)
	{
		ComponentManager::RemoveGameObject(gameObject->id);
		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->id == gameObject->id)
			{
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}
	}

	void Scene::Clear()
	{
		for (auto& gameObject : gameObjects)
		{
			ComponentManager::RemoveGameObject(gameObject->id);
		}

		gameObjects.clear();
		gameObjectsByID.clear();
		nextGameObjectID = 0;
		m_MainCamera = nullptr;
	}

	GameObject* Scene::GetGameObject(int32_t id)
	{
		if (gameObjectsByID.find(id) != gameObjectsByID.end())
		{
			return gameObjectsByID[id].get();
		}

		Logger::LogError("[Scene] Cannot find game object " + std::to_string(id));
		return nullptr;
	}

	void Scene::Awake()
	{
		for (const auto& gameObject : gameObjects)
		{
			ComponentManager::ExecuteOnGameObjectComponents(gameObject->id, awakeFunc);
		}
	}

	void Scene::Update()
	{
		for (const auto& gameObject : gameObjects)
		{
			ComponentManager::ExecuteOnGameObjectComponents(gameObject->id, updateFunc);
		}
	}

	void Scene::OnDestroy()
	{
		for (const auto& gameObject : gameObjects)
		{
			ComponentManager::ExecuteOnGameObjectComponents(gameObject->id, onDestroyFunc);
		}
	}

	void Scene::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Scene::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	GameObject* Scene::CreateEmptyGameObject()
	{
		// Create a new game object
		uint32_t id = nextGameObjectID++;
		std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(id);

		gameObjects.push_back(gameObject);
		gameObjectsByID[id] = gameObject;

		return gameObject.get();
	}

	void Scene::SaveToDisk(const std::filesystem::path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the metadata first
		SerializeMetadata(serializer);

		SerializationNode gameObjectsNode = root.CreateSequenceNode("GameObjects");

		for (auto& gameObject : gameObjects)
		{
			gameObject->Serialize(gameObjectsNode);
		}

		serializer.WriteToDisk(assetPath);
	}

	void Scene::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);

		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			SerializationNode gameObjectsNode = root.GetNode("GameObjects");

			assert(gameObjectsNode.IsSequence());
			assert(gameObjectsNode.HasChildren());

			for (size_t i = 0; i < gameObjectsNode.ChildCount(); i++)
			{
				GameObject* gameObject = CreateEmptyGameObject();
				SerializationNode child = gameObjectsNode.GetChild(i);
				gameObject->Deserialize(child);
			}
		}
	}

	Camera* Scene::GetMainCamera()
	{
		if (m_MainCamera == nullptr)
			FindMainCamera();

		return m_MainCamera;
	}

	void Scene::FindMainCamera()
	{
		for (auto& gameObject : gameObjects)
		{
			if (Camera* camera = gameObject->GetComponent<Camera>())
			{
				if (camera->IsMainCamera())
					m_MainCamera = camera;
			}
		}
	}
}