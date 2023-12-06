#include "ComponentManager.h"

namespace Odyssey
{
	void ComponentManager::RemoveUserScript(const uint32_t id, const std::string& managedName)
	{
		if (ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName))
		{
			userScriptArray->RemoveGameObject(id);
		}

		// Iterate through this game object's user scripts
		for (int i = 0; i < gameObjectToUserScriptIndex[id].size(); ++i)
		{
			auto& pair = gameObjectToUserScriptIndex[id][i];

			// Check if this pair matches the managed name of the script to remove
			if (pair.first == managedName)
			{
				// Remove it from the list of user scripts
				gameObjectToUserScriptIndex[id].erase(gameObjectToUserScriptIndex[id].begin() + i);

				// If we have no user scripts left, remove the game object's entry entirely
				if (gameObjectToUserScriptIndex[id].size() == 0)
				{
					gameObjectToUserScriptIndex.erase(id);
				}

				break;
			}
		}
	}

	UserScript* ComponentManager::GetUserScript(const uint32_t id, const std::string& managedName)
	{
		ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
		return userScriptArray->GetComponentData(id);
	}

	std::vector<std::pair<std::string, UserScript*>> ComponentManager::GetAllUserScripts(const uint32_t id)
	{
		std::vector<std::pair<std::string, UserScript*>> userScripts;

		if (gameObjectToUserScriptIndex.find(id) != gameObjectToUserScriptIndex.end())
		{
			std::vector<std::pair<std::string, unsigned int>> storedUserScripts = gameObjectToUserScriptIndex[id];

			for (const auto& [managedName, index] : storedUserScripts)
			{
				ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
				userScripts.push_back(std::make_pair(managedName, userScriptArray->GetComponentData(id)));
			}
		}

		return userScripts;
	}

	bool ComponentManager::HasUserScript(const uint32_t id, const std::string& managedName)
	{
		ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
		return userScriptArray->HasComponent(id);
	}

	ComponentArray<UserScript>* ComponentManager::GetUserScriptArray(const std::string& managedName)
	{
		if (userScriptArrays.find(managedName) == userScriptArrays.end())
		{
			userScriptArrays[managedName] = std::make_unique<ComponentArray<UserScript>>();
		}

		return userScriptArrays[managedName].get();
	}

	void ComponentManager::ExecuteOnGameObjectComponents(const uint32_t id, std::function<void(Component*)> func)
	{
		// For each component assigned to the game object
		for (const auto& [componentType, componentIndex] : gameObjectToComponentArrayIndex[id])
		{
			// Get the component array and run the component for this game object's Awake
			func(componentArrays[componentType]->componentData[componentIndex].get());
		}

		for (const auto& [className, componentIndex] : gameObjectToUserScriptIndex[id])
		{
			func(userScriptArrays[className]->componentData[componentIndex].get());
		}
	}

	void ComponentManager::RemoveGameObject(const uint32_t id)
	{
		// For each component assigned to the game object
		if (gameObjectToComponentArrayIndex.find(id) != gameObjectToComponentArrayIndex.end())
		{
			for (const auto& pair : gameObjectToComponentArrayIndex[id])
			{
				// Remove the game object from each component array
				componentArrays[pair.first]->RemoveGameObject(id);
			}

			gameObjectToComponentArrayIndex.erase(id);
		}

		if (gameObjectToUserScriptIndex.find(id) != gameObjectToUserScriptIndex.end())
		{
			for (const auto& [className, index] : gameObjectToUserScriptIndex[id])
			{
				ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(className);
				userScriptArray->RemoveGameObject(id);
			}

			gameObjectToUserScriptIndex.erase(id);
		}
	}
}