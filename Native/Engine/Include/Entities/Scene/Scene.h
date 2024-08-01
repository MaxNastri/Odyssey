#pragma once
#include "Asset.h"
#include "ComponentRegistry.h"

namespace Odyssey
{
	// FWD Declarations
	class Camera;
	class Component;
	class GameObject;

	class Scene : public Asset
	{
	public:
		Scene();
		Scene(const std::filesystem::path& assetPath);
		GameObject* CreateGameObject();
		void DestroyGameObject(GameObject* gameObject);
		void Clear();

	public:
		std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return gameObjects; }
		GameObject* GetGameObject(int32_t id);
		ComponentRegistry* GetComponentRegistry() { return m_ComponentRegistry.get(); }

	public:
		void Awake();
		void Update();
		void OnDestroy();

	public:
		void SaveTo(const std::filesystem::path& savePath);
		void Save();
		void Load();

	private:
		GameObject* CreateEmptyGameObject();
		void SaveToDisk(const std::filesystem::path& assetPath);
		void LoadFromDisk(const std::filesystem::path& assetPath);

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
		std::unique_ptr<ComponentRegistry> m_ComponentRegistry;

	private:
		std::function<void(Component*)> awakeFunc;
		std::function<void(Component*)> updateFunc;
		std::function<void(Component*)> onDestroyFunc;
	};
}