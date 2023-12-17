#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "GameObject.h"
#include "Asset.h"

namespace Odyssey
{
	// FWD Declarations
	class Camera;
	class Component;

	class Scene : public Asset
	{
	public:
		Scene();
		Scene(const std::string& assetPath);
		GameObject* CreateGameObject();
		void DestroyGameObject(GameObject* gameObject);
		void Clear();

	public:
		std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return gameObjects; }
		GameObject* GetGameObject(int32_t id);

	public:
		void Awake();
		void Update();
		void OnDestroy();
		void Save();
		void SaveTo(const std::string& assetPath);
		void Load(const std::string& assetPath);

	public:
		Camera* GetMainCamera();

	private:
		void FindMainCamera();

	private:
		friend class RenderScene;
		std::vector<std::shared_ptr<GameObject>> gameObjects;
		std::unordered_map<int32_t, std::shared_ptr<GameObject>> gameObjectsByID;
		int32_t nextGameObjectID;
		Camera* m_MainCamera = nullptr;

	private:
		std::function<void(Component*)> awakeFunc;
		std::function<void(Component*)> updateFunc;
		std::function<void(Component*)> onDestroyFunc;
	};
}