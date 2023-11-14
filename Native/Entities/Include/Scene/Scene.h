#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "GameObject.h"

namespace Odyssey
{
	// FWD Declarations
	class Component;

	class Scene
	{
	public:
		Scene();
		GameObject CreateGameObject();
		void DestroyGameObject(GameObject gameObject);
		void Clear();
		GameObject GetGameObject(unsigned int id);

	public:
		void Awake();
		void Update();
		void OnDestroy();
		void Serialize(const std::string& filename);
		void Deserialize(const std::string& filename);

	private:
		std::string name;
		std::vector<GameObject> gameObjects;
		std::unordered_map<unsigned int, GameObject> gameObjectsByID;
		unsigned int nextGameObjectID;

	private:
		std::function<void(Component*)> awakeFunc;
		std::function<void(Component*)> updateFunc;
		std::function<void(Component*)> onDestroyFunc;
	};
}