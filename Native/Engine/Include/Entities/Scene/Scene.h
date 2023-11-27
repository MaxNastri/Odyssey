#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "GameObject.h"

namespace Odyssey
{
	// FWD Declarations
	class Camera;
	class Component;

	class Scene
	{
	public:
		Scene();
		RefHandle<GameObject> CreateGameObject();
		void DestroyGameObject(RefHandle<GameObject>& gameObject);
		void Clear();

	public:
		std::vector<RefHandle<GameObject>>& GetGameObjects() { return gameObjects; }
		RefHandle<GameObject> GetGameObject(uint32_t id);

	public:
		void Awake();
		void Update();
		void OnDestroy();
		void Serialize(const std::string& filename);
		void Deserialize(const std::string& filename);

	public:
		Camera* GetMainCamera();

	private:
		void FindMainCamera();

	private:
		friend class RenderScene;
		std::string name;
		std::vector<RefHandle<GameObject>> gameObjects;
		std::unordered_map<unsigned int, RefHandle<GameObject>> gameObjectsByID;
		uint32_t nextGameObjectID;
		Camera* m_MainCamera = nullptr;

	private:
		std::function<void(Component*)> awakeFunc;
		std::function<void(Component*)> updateFunc;
		std::function<void(Component*)> onDestroyFunc;
	};
}