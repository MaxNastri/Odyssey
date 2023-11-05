#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "GameObject.h"

namespace Odyssey::Entities
{
	class Scene
	{
	public:
		GameObject CreateGameObject();
		void DestroyGameObject(GameObject gameObject);
		
	public:
		void Awake();
		void Update();
		void OnDestroy();

	private:
		std::string name;
		std::vector<GameObject> gameObjects;
		std::unordered_map<unsigned int, GameObject> gameObjectsByID;
		unsigned int nextGameObjectID;
	};
}