#include "ComponentRegistry.h"
#include "ComponentArray.h"

namespace Odyssey
{
	void ComponentRegistry::RemoveUserScript(const int32_t id, const std::string& managedName)
	{
		if (ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName))
		{
			userScriptArray->RemoveGameObject(id);
		}

		// Iterate through this game object's user scripts
		for (int i = 0; i < gameObjectToUserScriptIndex[id].size(); ++i)
		{
			UserScriptArrayEntry& entry = gameObjectToUserScriptIndex[id][i];

			// Check if this pair matches the managed name of the script to remove
			if (entry.Type == managedName)
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

	UserScript* ComponentRegistry::GetUserScript(const int32_t id, const std::string& managedName)
	{
		ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
		return userScriptArray->GetComponentData(id);
	}

	std::vector<UserScript*> ComponentRegistry::GetAllUserScripts(const int32_t id)
	{
		std::vector<UserScript*> userScripts;

		if (gameObjectToUserScriptIndex.find(id) != gameObjectToUserScriptIndex.end())
		{
			std::vector<UserScriptArrayEntry> storedUserScripts = gameObjectToUserScriptIndex[id];

			for (const auto& [managedName, index] : storedUserScripts)
			{
				ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
				userScripts.push_back(userScriptArray->GetComponentData(id));
			}
		}

		return userScripts;
	}

	bool ComponentRegistry::HasUserScript(const int32_t id, const std::string& managedName)
	{
		ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
		return userScriptArray->HasComponent(id);
	}

	ComponentArray<UserScript>* ComponentRegistry::GetUserScriptArray(const std::string& managedName)
	{
		if (userScriptArrays.find(managedName) == userScriptArrays.end())
		{
			userScriptArrays[managedName] = std::make_unique<ComponentArray<UserScript>>();
		}

		return userScriptArrays[managedName].get();
	}

	void ComponentRegistry::ExecuteOnGameObjectComponents(const int32_t id, std::function<void(Component*)> func)
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

	void ComponentRegistry::RemoveGameObject(const int32_t id)
	{
		// For each component assigned to the game object
		if (gameObjectToComponentArrayIndex.find(id) != gameObjectToComponentArrayIndex.end())
		{
			for (const auto& componentArrayIndex : gameObjectToComponentArrayIndex[id])
			{
				// Remove the game object from each component array
				componentArrays[componentArrayIndex.Type]->RemoveGameObject(id);
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