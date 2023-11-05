#include "Scene.h"
#include "ComponentManager.h"

namespace Odyssey::Entities
{
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

	void Scene::Awake()
	{
		ComponentManager::Awake(gameObjects);
	}

	void Scene::Update()
	{
		ComponentManager::Update(gameObjects);
	}

	void Scene::OnDestroy()
	{
		ComponentManager::OnDestroy(gameObjects);
	}
}