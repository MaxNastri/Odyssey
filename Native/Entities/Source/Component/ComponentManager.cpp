#include "ComponentManager.h"

namespace Odyssey::Entities
{
	void ComponentManager::RemoveUserScript(const GameObject& gameObject, const std::string& managedName)
	{
		if (ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName))
		{
			userScriptArray->RemoveGameObject(gameObject.id);
		}

		// Iterate through this game object's user scripts
		for (int i = 0; i < gameObjectToUserScriptIndex[gameObject.id].size(); ++i)
		{
			auto& pair = gameObjectToUserScriptIndex[gameObject.id][i];

			// Check if this pair matches the managed name of the script to remove
			if (pair.first == managedName)
			{
				// Remove it from the list of user scripts
				gameObjectToUserScriptIndex[gameObject.id].erase(gameObjectToUserScriptIndex[gameObject.id].begin() + i);

				// If we have no user scripts left, remove the game object's entry entirely
				if (gameObjectToUserScriptIndex[gameObject.id].size() == 0)
				{
					gameObjectToUserScriptIndex.erase(gameObject.id);
				}

				break;
			}
		}
	}

	UserScript* ComponentManager::GetUserScript(const GameObject& gameObject, const std::string& managedName)
	{
		ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
		return userScriptArray->GetComponentData(gameObject.id);
	}

	std::vector<std::pair<std::string, UserScript*>> ComponentManager::GetAllUserScripts(const GameObject& gameObject)
	{
		std::vector<std::pair<std::string, UserScript*>> userScripts;

		if (gameObjectToUserScriptIndex.find(gameObject.id) != gameObjectToUserScriptIndex.end())
		{
			std::vector<std::pair<std::string, unsigned int>> storedUserScripts = gameObjectToUserScriptIndex[gameObject.id];

			for (const auto& [managedName, index] : storedUserScripts)
			{
				ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
				userScripts.push_back(std::make_pair(managedName, userScriptArray->GetComponentData(gameObject.id)));
			}
		}

		return userScripts;
	}

	bool ComponentManager::HasUserScript(const GameObject& gameObject, const std::string& managedName)
	{
		ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
		return userScriptArray->HasComponent(gameObject.id);
	}

	ComponentArray<UserScript>* ComponentManager::GetUserScriptArray(const std::string& managedName)
	{
		if (userScriptArrays.find(managedName) == userScriptArrays.end())
		{
			userScriptArrays[managedName] = std::make_unique<ComponentArray<UserScript>>();
		}

		return userScriptArrays[managedName].get();
	}

	void ComponentManager::ExecuteOnGameObjectComponents(const GameObject& gameObject, std::function<void(Component*)> func)
	{
		// For each component assigned to the game object
		for (const auto& [componentType, componentIndex] : gameObjectToComponentArrayIndex[gameObject.id])
		{
			// Get the component array and run the component for this game object's Awake
			func(componentArrays[componentType]->componentData[componentIndex].get());
		}

		for (const auto& [className, componentIndex] : gameObjectToUserScriptIndex[gameObject.id])
		{
			func(userScriptArrays[className]->componentData[componentIndex].get());
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
				componentArrays[pair.first]->RemoveGameObject(gameObject.id);
			}

			gameObjectToComponentArrayIndex.erase(gameObject.id);
		}

		if (gameObjectToUserScriptIndex.find(gameObject.id) != gameObjectToUserScriptIndex.end())
		{
			for (const auto& [className, index] : gameObjectToUserScriptIndex[gameObject.id])
			{
				ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(className);
				userScriptArray->RemoveGameObject(gameObject.id);
			}

			gameObjectToUserScriptIndex.erase(gameObject.id);
		}
	}
}