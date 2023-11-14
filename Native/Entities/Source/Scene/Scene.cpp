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
		name = "Scene";
		nextGameObjectID = 0;
	}

	GameObject Scene::CreateGameObject()
	{
		GameObject gameObject = GameObject(nextGameObjectID++);
		gameObjects.push_back(gameObject);
		gameObjectsByID[gameObject.id] = gameObject;
		return gameObject;
	}

	void Scene::DestroyGameObject(GameObject gameObject)
	{
		ComponentManager::RemoveGameObject(gameObject);
		gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject));
	}
	void Scene::Clear()
	{
		for (GameObject& gameObject : gameObjects)
		{
			DestroyGameObject(gameObject);
		}

		gameObjects.clear();
		gameObjectsByID.clear();
		nextGameObjectID = 0;
	}

	GameObject Scene::GetGameObject(unsigned int id)
	{
		if (gameObjectsByID.find(id) != gameObjectsByID.end())
		{
			return gameObjectsByID[id];
		}
		Logger::LogError("[Scene] Cannot find game object " + std::to_string(id));
		return NULL;
	}

	void Scene::Awake()
	{
		for (const GameObject& gameObject : gameObjects)
		{
			ComponentManager::ExecuteOnGameObjectComponents(gameObject, awakeFunc);
		}
	}

	void Scene::Update()
	{
		for (const GameObject& gameObject : gameObjects)
		{
			ComponentManager::ExecuteOnGameObjectComponents(gameObject, updateFunc);
		}
	}

	void Scene::OnDestroy()
	{
		for (const GameObject& gameObject : gameObjects)
		{
			ComponentManager::ExecuteOnGameObjectComponents(gameObject, onDestroyFunc);
		}
	}

	void Scene::Serialize(const std::string& filename)
	{
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["Name"] << name;

		ryml::NodeRef gameObjectsNode = root["GameObjects"];
		gameObjectsNode |= ryml::SEQ;
		
		for (GameObject& gameObject : gameObjects)
		{
			gameObject.Serialize(gameObjectsNode);
		}

		FILE* file2 = fopen(filename.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Scene::Deserialize(const std::string& filename)
	{
		if (std::ifstream ifs{ filename })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef root = tree.rootref();
			
			root["Name"] >> name;

			ryml::NodeRef gameObjectsNode = root["GameObjects"];

			assert(gameObjectsNode.is_seq());
			assert(gameObjectsNode.has_children());

			for (size_t i = 0; i < gameObjectsNode.num_children(); i++)
			{
				GameObject gameObject = CreateGameObject();
				ryml::NodeRef child = gameObjectsNode.child(i);
				gameObject.Deserialize(child);
			}
		}
		else
			std::cout << "Cannot open file\n";

	}
}