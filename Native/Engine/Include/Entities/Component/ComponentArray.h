#pragma once
#include "Component.h"
#include "Logger.h"

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
		virtual Component* GetComponent(uint32_t gameObject) = 0;
		virtual void RemoveGameObject(uint32_t gameObject) = 0;

	protected:
		friend class ComponentManager;
		friend class ComponentRegistry;
		std::array<std::unique_ptr<Component>, MAX_GAME_OBJECTS> componentData;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		ComponentArray()
		{
			for (uint32_t i = 0; i < MAX_GAME_OBJECTS; i++)
			{
				availableIndices.push(i);
			}
		}
	public:
		template<typename... Args>
		uint32_t InsertData(uint32_t gameObject, Args&&... params)
		{
			if (gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end())
			{
				// Assign the index to the lookup and create the component
				uint32_t newIndex = availableIndices.front();
				availableIndices.pop();

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

		T* GetComponentData(uint32_t gameObject)
		{
			if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
			{
				uint32_t index = gameObjectToIndexMap[gameObject];
				return static_cast<T*>(componentData[index].get());
			}

			return nullptr;
		}

		virtual Component* GetComponent(uint32_t gameObject) override
		{
			if (gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end())
			{
				uint32_t index = gameObjectToIndexMap[gameObject];
				return componentData[index].get();
			}

			return nullptr;
		}

		bool HasComponent(uint32_t gameObject)
		{
			return gameObjectToIndexMap.find(gameObject) != gameObjectToIndexMap.end();
		}

		virtual void RemoveGameObject(unsigned int gameObject) override
		{
			if (availableIndices.size() == 0)
			{
				// TODO: Log error
				return;
			}

			// Make sure the game object has this component type
			if (gameObjectToIndexMap.find(gameObject) == gameObjectToIndexMap.end())
			{
				Logger::LogError("Cannot remove component from game object: " + std::to_string(gameObject));
				return;
			}

			// Get the index of the removed game object and our last element
			uint32_t removalIndex = gameObjectToIndexMap[gameObject];
			
			// Call on destroy for the removed component
			componentData[removalIndex]->OnDestroy();
			componentData[removalIndex].reset();
			
			availableIndices.push(removalIndex);

			// Remove the gameObject from the index lookup
			gameObjectToIndexMap.erase(gameObject);
			size--;
		}

	private:
		std::unordered_map<uint32_t, uint32_t> gameObjectToIndexMap;
		size_t size;
		std::queue<uint32_t> availableIndices;
	};
}