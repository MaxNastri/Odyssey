#include "Scene.h"
#include "ComponentManager.h"
#include <fstream>

namespace Odyssey::Entities
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
		json jsonObject
		{
			{ "name", name }
		};

		for (GameObject& gameObject : gameObjects)
		{
			gameObject.Serialize(jsonObject);
		}

		std::fstream file(filename, std::ios_base::out);
		file << std::setw(4) << jsonObject << std::endl;
		file.close();
	}

	void Scene::Deserialize(const std::string& filename)
	{
		std::fstream file(filename, std::ios_base::in);
		json jsonObject;
		file >> jsonObject;

		name = jsonObject.at("name");

		for (auto& element : jsonObject)
		{
			if (element.is_object() && element.at("Type") == GameObject::Type)
			{
				GameObject go = CreateGameObject();
				go.Deserialize(element);
			}
		}
	}
}