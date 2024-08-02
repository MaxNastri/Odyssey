#pragma once
#include "Asset.h"
#include "entt.hpp"
#include "GameObject.h"

namespace Odyssey
{
	class Camera;
	class Component;
	class GameObject;

	class Scene : public Asset
	{
	public:
		Scene();
		Scene(const std::filesystem::path& assetPath);

	public:
		GameObject CreateGameObject();
		GameObject GetGameObject(const std::string& guid) { return m_GUIDToGameObject[guid]; }

		void DestroyGameObject(const GameObject& gameObject);
		void Clear();

	public:
		void Awake();
		void Update();
		void OnDestroy();

	public:
		void SaveTo(const std::filesystem::path& savePath);
		void Save();
		void Load();

	private:
		void SaveToDisk(const std::filesystem::path& assetPath);
		void LoadFromDisk(const std::filesystem::path& assetPath);

	public:
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		friend class RenderScene;
		friend class GameObject;
		Camera* m_MainCamera = nullptr;
		entt::registry m_Registry;
		std::map<std::string, GameObject> m_GUIDToGameObject;
	};
}

#include "GameObjectTemplates.h"