#include "ComponentManager.h"

namespace Odyssey::Entities
{
	std::unordered_map<std::type_index, IComponentArray*> ComponentManager::componentArrays;
	std::unordered_map<unsigned int, std::vector<std::pair<std::type_index, unsigned int>>> ComponentManager::gameObjectToComponentArrayIndex;

	void ComponentManager::Awake(const std::vector<GameObject>& gameObjects)
	{
		// For each game object
		for (GameObject gameObject : gameObjects)
		{
			// For each component assigned to the game object
			for (const auto& pair : gameObjectToComponentArrayIndex[gameObject.id])
			{
				// Get the component array and run the component for this game object's Awake
				IComponentArray* componentArray = componentArrays[pair.first];
				componentArray->componentData[pair.second]->Awake();
			}
		}
	}

	void ComponentManager::Update(const std::vector<GameObject>& gameObjects)
	{
		// For each game object
		for (GameObject gameObject : gameObjects)
		{
			// For each component assigned to the game object
			for (const auto& pair : gameObjectToComponentArrayIndex[gameObject.id])
			{
				// Get the component array and run the component for this game object's Update
				IComponentArray* componentArray = componentArrays[pair.first];
				componentArray->componentData[pair.second]->Update();
			}
		}
	}

	void ComponentManager::OnDestroy(const std::vector<GameObject>& gameObjects)
	{
		// For each game object
		for (GameObject gameObject : gameObjects)
		{
			// For each component assigned to the game object
			for (const auto& pair : gameObjectToComponentArrayIndex[gameObject.id])
			{
				// Get the component array and run the component for this game object's OnDestroy
				IComponentArray* componentArray = componentArrays[pair.first];
				componentArray->componentData[pair.second]->OnDestroy();
			}
		}
	}
	void ComponentManager::RemoveGameObject(const GameObject& gameObject)
	{
		// For each component assigned to the game object
		if (gameObjectToComponentArrayIndex.find(gameObject.id) != gameObjectToComponentArrayIndex.end())
		{
			for (const auto& pair : gameObjectToComponentArrayIndex[gameObject.id])
			{
				// Remove the game object from each component array
				IComponentArray* componentArray = componentArrays[pair.first];
				componentArray->RemoveGameObject(gameObject.id);
			}

			gameObjectToComponentArrayIndex.erase(gameObject.id);
		}
	}
}