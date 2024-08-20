#include "Scene.h"
#include "AssetSerializer.h"
#include "GameObject.h"
#include "Camera.h"
#include "PropertiesComponent.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "ScriptComponent.h"
#include "ScriptingManager.h"

namespace Odyssey
{
	Scene::Scene()
	{
	}

	Scene::Scene(const std::filesystem::path& assetPath)
		: m_Path(assetPath)
	{
		LoadFromDisk(m_Path);
	}

	GameObject Scene::CreateGameObject()
	{
		// Create the backing entity
		const auto entity = m_Registry.create();

		// Construct a game object and add an ID component 
		GameObject gameObject = GameObject(this, entity);
		gameObject.AddComponent<PropertiesComponent>();

		m_GUIDToGameObject[gameObject.GetGUID()] = gameObject;

		return gameObject;
	}

	GameObject Scene::CreateEmptyEntity()
	{
		// Create the backing entity
		const auto entity = m_Registry.create();

		// Return a game object wrapper
		return GameObject(this, entity);
	}

	void Scene::DestroyGameObject(const GameObject& gameObject)
	{
		m_Registry.destroy(gameObject);
	}

	void Scene::Clear()
	{
		for (auto entity : m_Registry.view<PropertiesComponent>())
		{
			DestroyGameObject(GameObject(this, entity));
		}
		m_Registry.clear();
	}

	void Scene::OnStartRuntime()
	{
		for (auto entity : m_Registry.view<ScriptComponent>())
		{
			GameObject gameObject = GameObject(this, entity);
			GUID guid = gameObject.GetGUID();

			ScriptComponent& script = gameObject.GetComponent<ScriptComponent>();
			script.SetManagedHandle(ScriptingManager::Instantiate(guid, (uint64_t)guid));
		}
	}

	void Scene::OnStopRuntime()
	{
		for (auto entity : m_Registry.view<ScriptComponent>())
		{
			GameObject gameObject = GameObject(this, entity);
			GUID guid = gameObject.GetGUID();

			ScriptComponent& script = gameObject.GetComponent<ScriptComponent>();
			script.OnDestroy();
			script.ClearManagedHandle();
			ScriptingManager::DestroyInstance(guid);
		}
	}

	void Scene::Awake()
	{
		for (auto entity : m_Registry.view<Camera>())
		{
			GameObject gameObject = GameObject(this, entity);
			auto& camera = gameObject.GetComponent<Camera>();
			camera.Awake();
		}

		for (auto entity : m_Registry.view<ScriptComponent>())
		{
			GameObject gameObject = GameObject(this, entity);
			ScriptComponent& script = gameObject.GetComponent<ScriptComponent>();
			script.Awake();
		}
	}

	void Scene::Update()
	{
		for (auto entity : m_Registry.view<ScriptComponent>())
		{
			GameObject gameObject = GameObject(this, entity);
			auto& userScript = gameObject.GetComponent<ScriptComponent>();
			userScript.Update();
		}
	}

	void Scene::OnDestroy()
	{
		for (auto entity : m_Registry.view<ScriptComponent>())
		{
			GameObject gameObject = GameObject(this, entity);
			auto& userScript = gameObject.GetComponent<ScriptComponent>();
			userScript.OnDestroy();
		}
	}

	void Scene::SaveTo(const std::filesystem::path& savePath)
	{
		SaveToDisk(savePath);
	}

	void Scene::Save()
	{
		SaveToDisk(m_Path);
	}

	void Scene::Load()
	{
		LoadFromDisk(m_Path);
	}

	void Scene::SaveToDisk(const std::filesystem::path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		root.WriteData("Scene", m_Name);
		root.WriteData("GUID", m_GUID);

		SerializationNode gameObjectsNode = root.CreateSequenceNode("GameObjects");

		for (auto entity : m_Registry.view<PropertiesComponent>())
		{
			GameObject gameObject = GameObject(this, entity);
			PropertiesComponent properties = gameObject.GetComponent<PropertiesComponent>();
	
			if (properties.Serialize)
				gameObject.Serialize(gameObjectsNode);
		}

		serializer.WriteToDisk(assetPath);
	}

	void Scene::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);

		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			SerializationNode gameObjectsNode = root.GetNode("GameObjects");

			root.ReadData("Scene", m_Name);
			root.ReadData("GUID", m_GUID.Ref());

			assert(gameObjectsNode.IsSequence());
			assert(gameObjectsNode.HasChildren());

			for (size_t i = 0; i < gameObjectsNode.ChildCount(); i++)
			{
				GameObject gameObject = CreateEmptyEntity();
				SerializationNode child = gameObjectsNode.GetChild(i);
				gameObject.Deserialize(child);

				GUID guid = gameObject.GetGUID();
				m_GUIDToGameObject[guid] = gameObject;

				if (gameObject.HasComponent<ScriptComponent>())
				{
					ScriptComponent& scriptComponent = gameObject.GetComponent<ScriptComponent>();
					uint32_t scriptID = scriptComponent.GetScriptID();
				}
			}
		}
	}
}