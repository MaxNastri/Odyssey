#include "Scene.h"
#include "AssetSerializer.h"
#include "GameObject.h"
#include "Camera.h"
#include "PropertiesComponent.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "ScriptComponent.h"
#include "ScriptingManager.h"
#include "Animator.h"
#include "ParticleBatcher.h"
#include "ParticleEmitter.h"

namespace Odyssey
{
	Scene::Scene()
	{
	}

	Scene::Scene(const Path& assetPath)
		: m_Path(assetPath)
	{
		m_Registry.on_construct<ParticleEmitter>().connect<&Scene::OnParticleEmitterCreate>(this);
		m_Registry.on_destroy<ParticleEmitter>().connect<&Scene::OnParticleEmitterDestroy>(this);

		LoadFromDisk(m_Path);
	}

	GameObject Scene::CreateGameObject()
	{
		// Create the backing entity
		const auto entity = m_Registry.create();

		// Construct a game object and add an ID component 
		GameObject gameObject = GameObject(this, entity);
		PropertiesComponent& properties = gameObject.AddComponent<PropertiesComponent>(GUID::New());

		m_GUIDToGameObject[gameObject.GetGUID()] = gameObject;

		m_SceneGraph.AddEntity(gameObject);

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
		entt::entity entity = gameObject;
		m_SceneGraph.RemoveEntityAndChildren(gameObject);
		m_Registry.destroy(entity);
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

	void Scene::OnEditorUpdate()
	{
		for (auto entity : m_Registry.view<Animator>())
		{
			GameObject gameObject = GameObject(this, entity);
			Animator& animator = gameObject.GetComponent<Animator>();
			animator.OnEditorUpdate();
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

		for (auto entity : m_Registry.view<Animator>())
		{
			GameObject gameObject = GameObject(this, entity);
			Animator& animator = gameObject.GetComponent<Animator>();
			animator.Update();
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

	void Scene::SaveTo(const Path& savePath)
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

	void SerializeSceneNode(SerializationNode& serializationNode, Ref<SceneNode>& sceneNode)
	{
		GameObject gameObject = sceneNode->Entity;
		PropertiesComponent& properties = gameObject.GetComponent<PropertiesComponent>();

		if (properties.Serialize)
		{
			SerializationNode gameObjectNode = serializationNode.AppendChild();
			gameObjectNode.SetMap();
			gameObjectNode.WriteData("Sort Order", properties.SortOrder);
			gameObjectNode.WriteData("Prefab", 0);

			gameObject.Serialize(gameObjectNode);
		}

		for (Ref<SceneNode> childNode : sceneNode->Children)
			SerializeSceneNode(serializationNode, childNode);
	}

	void Scene::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		GUID skybox = m_EnvironmentSettings.Skybox ? m_EnvironmentSettings.Skybox->GetGUID() : GUID(0);

		root.WriteData("Scene", m_Name);
		root.WriteData("GUID", m_GUID.CRef());
		root.WriteData("Skybox", skybox.CRef());
		root.WriteData("Ambient Color", m_EnvironmentSettings.AmbientColor);
		root.WriteData("Scene Center", m_EnvironmentSettings.SceneCenter);
		root.WriteData("Scene Radius", m_EnvironmentSettings.SceneRadius);

		SerializationNode gameObjectsNode = root.CreateSequenceNode("GameObjects");

		// Get the scene root and make sure the graph is sorted
		Ref<SceneNode>& sceneRoot = m_SceneGraph.GetSceneRoot();
		sceneRoot->SortChildren(true);

		for (Ref<SceneNode> sceneNode : sceneRoot->Children)
			SerializeSceneNode(gameObjectsNode, sceneNode);

		m_SceneGraph.Serialize(this, root);

		serializer.WriteToDisk(assetPath);
	}

	void Scene::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);

		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			SerializationNode gameObjectsNode = root.GetNode("GameObjects");

			GUID skybox;
			root.ReadData("Scene", m_Name);
			root.ReadData("GUID", m_GUID.Ref());
			root.ReadData("Skybox", skybox.Ref());
			root.ReadData("Ambient Color", m_EnvironmentSettings.AmbientColor);
			root.ReadData("Scene Center", m_EnvironmentSettings.SceneCenter);
			root.ReadData("Scene Radius", m_EnvironmentSettings.SceneRadius);

			if (skybox)
				m_EnvironmentSettings.SetSkybox(skybox);

			assert(gameObjectsNode.IsSequence());
			assert(gameObjectsNode.HasChildren());

			for (size_t i = 0; i < gameObjectsNode.ChildCount(); i++)
			{
				SerializationNode gameObjectNode = gameObjectsNode.GetChild(i);
				assert(gameObjectNode.IsMap());

				GameObject gameObject = CreateEmptyEntity();
				PropertiesComponent& properties = gameObject.AddComponent<PropertiesComponent>();

				GUID prefab;
				gameObjectNode.ReadData("Sort Order", properties.SortOrder);
				gameObjectNode.ReadData("Prefab", prefab.Ref());

				if (!prefab)
					gameObject.Deserialize(gameObjectNode);

				GUID guid = gameObject.GetGUID();
				m_GUIDToGameObject[guid] = gameObject;
			}

			m_SceneGraph.Deserialize(this, root);
		}
	}

	void Scene::OnParticleEmitterCreate(entt::registry& registry, entt::entity entity)
	{
		GameObject gameObject(this, entity);
		ParticleEmitter& emitter = gameObject.GetComponent<ParticleEmitter>();
		ParticleBatcher::RegisterEmitter(&emitter);
	}

	void Scene::OnParticleEmitterDestroy(entt::registry& registry, entt::entity entity)
	{
		GameObject gameObject(this, entity);
		ParticleEmitter& emitter = gameObject.GetComponent<ParticleEmitter>();
		ParticleBatcher::DeregisterEmtter(&emitter);
	}
}