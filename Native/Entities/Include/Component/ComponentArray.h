#pragma once
#include <array>
#include <unordered_map>
#include "Component.h"
#include <Log.h>
#include <string>

#ifndef MAX_GAME_OBJECTS
#define MAX_GAME_OBJECTS 5000
#endif // !MAX_GAME_OBJECTS

namespace Odyssey::Entities
{
	// FWD Declarations
	class Component;

	class IComponentArray
	{
		virtual Component* GetComponent(unsigned int gameObject) = 0;
		virtual void RemoveGameObject(unsigned int gameObject) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		template<typename... Args>
		void InsertData(unsigned int gameObject, Args&&... params)
		{
			if (gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end())
			{
				// Assign the index to the lookup and create the component
				unsigned int newIndex = size;
				gameObjectToIndexMap[gameObject] = newIndex;
				componentArray[newIndex] = new T(params...);
				++size;
			}
			else
			{
				Framework::Log::Error("Cannot insert data for game object: " + std::to_string(gameObject));
			}
		}

		T* GetComponentData(unsigned int gameObject)
		{
			if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
			{
				unsigned int index = gameObjectToIndexMap[gameObject];
				return reinterpret_cast<T*>(componentArray[index]);
			}

			Framework::Log::Error("Cannot Get Component Data for GameObject: " + std::to_string(gameObject));
			return nullptr;
		}

		virtual Component* GetComponent(unsigned int gameObject) override
		{
			if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
			{
				unsigned int index = gameObjectToIndexMap[gameObject];
				return componentArray[index];
			}

			Framework::Log::Error("Cannot Get Component for GameObject: " + std::to_string(gameObject));
			return nullptr;
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
				Framework::Log::Error("Cannot remove component from game object: " + std::to_string(gameObject));
				return;
			}

			// Get the index of the removed game object and our last element
			unsigned int removalIndex = gameObjectToIndexMap[gameObject];
			unsigned int lastIndex = size - 1;
			
			// Get both components from the array
			Component* removalComponent = componentArray[removalIndex];
			Component* lastComponent = componentArray[lastIndex];

			// Swap the last element into the removal index and move the removal to the end
			componentArray[removalIndex] = lastComponent;
			componentArray[lastIndex] = removalComponent;

			// Remove the gameObject from the index lookup
			gameObjectToIndexMap.erase(gameObject);

			// Destroy the removed component
			removalComponent->OnDestroy();

			// The last index is now the next viable slot
			size = lastIndex;
		}

	private:
		std::array<Component*, MAX_GAME_OBJECTS> componentArray;
		std::unordered_map<unsigned int, unsigned int> gameObjectToIndexMap;
		unsigned int size;
	};
}