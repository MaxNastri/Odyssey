#pragma once
#include <array>
#include <unordered_map>
#include "Component.h"
#include <Logger.h>
#include <string>

#ifndef MAX_GAME_OBJECTS
#define MAX_GAME_OBJECTS 5000
#endif // !MAX_GAME_OBJECTS

namespace Odyssey
{
	// FWD Declarations
	class Component;
	class ComponentManager;

	class IComponentArray
	{
		virtual Component* GetComponent(unsigned int gameObject) = 0;
		virtual void RemoveGameObject(unsigned int gameObject) = 0;

	protected:
		friend class ComponentManager;
		std::array<std::unique_ptr<Component>, MAX_GAME_OBJECTS> componentData;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		template<typename... Args>
		unsigned int InsertData(unsigned int gameObject, Args&&... params)
		{
			if (gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end())
			{
				// Assign the index to the lookup and create the component
				unsigned int newIndex = size;
				gameObjectToIndexMap[gameObject] = newIndex;
				componentData[newIndex] = std::make_unique<T>(params...);
				++size;
				return newIndex;
			}
			else
			{
				Logger::LogError("Cannot insert data for game object: " + std::to_string(gameObject));
				return -1;
			}
		}

		T* GetComponentData(unsigned int gameObject)
		{
			if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
			{
				unsigned int index = gameObjectToIndexMap[gameObject];
				return static_cast<T*>(componentData[index].get());
			}

			return nullptr;
		}

		virtual Component* GetComponent(unsigned int gameObject) override
		{
			if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
			{
				unsigned int index = gameObjectToIndexMap[gameObject];
				return componentData[index].get();
			}

			return nullptr;
		}

		bool HasComponent(unsigned int gameObject)
		{
			return gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end();
		}

		virtual void RemoveGameObject(unsigned int gameObject) override
		{
			if (size == 0)
			{
				gameObjectToIndexMap.clear();
				size = 0;
				return;
			}

			// Make sure the game object has this component type
			if (gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end())
			{
				Logger::LogError("Cannot remove component from game object: " + std::to_string(gameObject));
				return;
			}

			// Get the index of the removed game object and our last element
			unsigned int removalIndex = gameObjectToIndexMap[gameObject];
			unsigned int lastIndex = size - 1;
			
			// Call on destroy for the removed component
			componentData[removalIndex]->OnDestroy();
			
			// Replace the removed component with the last element and reset the last index ptr
			componentData[removalIndex] = std::move(componentData[lastIndex]);
			componentData[lastIndex].reset();

			// Remove the gameObject from the index lookup
			gameObjectToIndexMap.erase(gameObject);

			// The last index is now the next viable slot
			size = lastIndex;
		}

	private:
		std::unordered_map<unsigned int, unsigned int> gameObjectToIndexMap;
		unsigned int size;
	};
}