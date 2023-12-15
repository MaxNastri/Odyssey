#include "Scene.h"
#include "ComponentManager.h"
#include <fstream>
#include <string>
#include <Yaml.h>

namespace Odyssey
{
	Scene::Scene()
	{
		awakeFunc = [](Component* component) { component->Awake(); };
		updateFunc = [](Component* component) { component->Update(); };
		onDestroyFunc = [](Component* component) { component->OnDestroy(); };
		nextGameObjectID = 0;
	}

	Scene::Scene(const std::string& assetPath)
	{
		awakeFunc = [](Component* component) { component->Awake(); };
		updateFunc = [](Component* component) { component->Update(); };
		onDestroyFunc = [](Component* component) { component->OnDestroy(); };
		nextGameObjectID = 0;

		Load(assetPath);
	}

	GameObject* Scene::CreateGameObject()
	{
		// Create a new game object
		uint32_t id = nextGameObjectID++;
		std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(id);

		gameObjects.push_back(gameObject);
		gameObjectsByID[id] = gameObject;
		return gameObject.get();
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

	GameObject* Scene::GetGameObject(uint32_t id)
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
		SaveTo(m_AssetPath);
	}

	void Scene::SaveTo(const std::string& assetPath)
	{
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		// Serialize the base asset data
		root["m_GUID"] << m_GUID;
		root["m_Name"] << m_Name;
		root["m_AssetPath"] << assetPath;
		root["m_Type"] << m_Type;

		ryml::NodeRef gameObjectsNode = root["GameObjects"];
		gameObjectsNode |= ryml::SEQ;

		for (auto& gameObject : gameObjects)
		{
			gameObject->Serialize(gameObjectsNode);
		}

		FILE* file2 = fopen(assetPath.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Scene::Load(const std::string& assetPath)
	{
		if (std::ifstream ifs{ assetPath })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef root = tree.rootref();

			root["Name"] >> m_Name;

			ryml::NodeRef gameObjectsNode = root["GameObjects"];

			assert(gameObjectsNode.is_seq());
			assert(gameObjectsNode.has_children());

			for (size_t i = 0; i < gameObjectsNode.num_children(); i++)
			{
				GameObject* gameObject = CreateGameObject();
				ryml::NodeRef child = gameObjectsNode.child(i);
				gameObject->Deserialize(child);
			}
		}
		else
			std::cout << "Cannot open file\n";

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
			if (Camera* camera = ComponentManager::GetComponent<Camera>(gameObject->id))
			{
				if (camera->IsMainCamera())
					m_MainCamera = camera;
			}
		}
	}
}