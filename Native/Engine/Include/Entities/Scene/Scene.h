#pragma once
#include "Asset.h"
#include "entt.hpp"
#include "GameObject.h"
#include "GUID.h"

namespace Odyssey
{
	class Camera;
	class Component;
	class GameObject;

	class Scene
	{
	public:
		Scene();
		Scene(const std::filesystem::path& assetPath);

	public:
		GameObject CreateGameObject();
		GameObject GetGameObject(GUID guid) { return m_GUIDToGameObject[guid]; }

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

	public:
		GUID GetGUID() { return m_GUID; }

	private:
		void SaveToDisk(const std::filesystem::path& assetPath);
		void LoadFromDisk(const std::filesystem::path& assetPath);

	public:
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private: // Serialized
		GUID m_GUID;
		std::string m_Name;
		Path m_Path;

	private:
		friend class RenderScene;
		friend class GameObject;
		Camera* m_MainCamera = nullptr;
		entt::registry m_Registry;
		std::map<GUID, GameObject> m_GUIDToGameObject;
	};
}

#include "GameObjectTemplates.h"