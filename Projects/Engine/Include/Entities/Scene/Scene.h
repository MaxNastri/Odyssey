#pragma once
#include "Asset.h"
#include "entt.hpp"
#include "GameObject.h"
#include "GUID.h"
#include "SceneGraph.h"
#include "EnvironmentSettings.h"

namespace Odyssey
{
	class Camera;
	class Component;
	class GameObject;

	class Scene
	{
	public:
		Scene();
		Scene(const Path& assetPath);

	public:
		GameObject CreateGameObject();
		GameObject CreateEmptyEntity();
		GameObject GetGameObject(GUID guid) { return m_GUIDToGameObject[guid]; }
		void AddGameObject(GUID guid, GameObject gameObject);

		void DestroyGameObject(const GameObject& gameObject);
		void Clear();

	public:
		void OnStartRuntime();
		void OnStopRuntime();
		void Awake();
		void OnEditorUpdate();
		void Update();
		void OnDestroy();

	public:
		void SaveTo(const Path& savePath);
		void Save();
		void Load();

	public:
		GUID GetGUID() { return m_GUID; }
		const Path& GetPath() { return m_Path; }
		SceneGraph& GetSceneGraph() { return m_SceneGraph; }
		EnvironmentSettings& GetEnvironmentSettings() { return m_EnvironmentSettings; }

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		void OnParticleEmitterCreate(entt::registry& registry, entt::entity entity);
		void OnParticleEmitterDestroy(entt::registry& registry, entt::entity entity);

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
		EnvironmentSettings m_EnvironmentSettings;

	private:
		friend class RenderScene;
		friend class GameObject;
		Camera* m_MainCamera = nullptr;
		entt::registry m_Registry;
		std::map<GUID, GameObject> m_GUIDToGameObject;
		SceneGraph m_SceneGraph;
	};
}

#include "GameObjectTemplates.h"