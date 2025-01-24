#include "Scene.h"
#include "AssetSerializer.h"
#include "GameObject.h"
#include "ScriptingManager.h"
#include "ParticleBatcher.h"
#include "EventSystem.h"
#include "Events.h"
#include "Components.h"

namespace Odyssey
{
#define EXECUTE_ON_COMPONENTS(ComponentType, Func) \
for (auto entity : m_Registry.view<ComponentType>()) GameObject(this, entity).GetComponent<ComponentType>().Func();

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

		EventSystem::Dispatch<SceneModifiedEvent>(this, SceneModifiedEvent::Modification::CreateGameObject);

		return gameObject;
	}

	GameObject Scene::CreateEmptyEntity()
	{
		// Create the backing entity
		const auto entity = m_Registry.create();

		EventSystem::Dispatch<SceneModifiedEvent>(this, SceneModifiedEvent::Modification::CreateGameObject);

		// Return a game object wrapper
		return GameObject(this, entity);
	}

	void Scene::AddGameObject(GUID guid, GameObject gameObject)
	{
		m_GUIDToGameObject[guid] = gameObject;
	}

	void Scene::DestroyGameObject(const GameObject& gameObject)
	{
		std::vector<GameObject> toDestroy = m_SceneGraph.GetAllChildren(gameObject);
		m_SceneGraph.RemoveEntityAndChildren(gameObject);

		m_Registry.destroy(gameObject);

		for (auto& entity : toDestroy)
			m_Registry.destroy(entity);

		EventSystem::Dispatch<SceneModifiedEvent>(this, SceneModifiedEvent::Modification::DeleteGameObject);
	}

	void Scene::Clear()
	{
		for (auto entity : m_Registry.view<PropertiesComponent>())
		{
			DestroyGameObject(GameObject(this, entity));
		}
		m_Registry.clear();

		EventSystem::Dispatch<SceneModifiedEvent>(this, SceneModifiedEvent::Modification::DeleteGameObject);
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
		EXECUTE_ON_COMPONENTS(ScriptComponent, OnDestroy);
		EXECUTE_ON_COMPONENTS(RigidBody, OnDestroy);
		EXECUTE_ON_COMPONENTS(CharacterController, OnDestroy);
	}

	void Scene::Awake()
	{
		m_State = SceneState::Awake;

		EXECUTE_ON_COMPONENTS(Camera, Awake);
		EXECUTE_ON_COMPONENTS(ScriptComponent, Awake);
		EXECUTE_ON_COMPONENTS(RigidBody, Awake);
		EXECUTE_ON_COMPONENTS(CharacterController, Awake);
	}

	void Scene::OnEditorUpdate()
	{
		EXECUTE_ON_COMPONENTS(Animator, OnEditorUpdate);
	}

	void Scene::Update()
	{
		m_State = SceneState::Update;

		EXECUTE_ON_COMPONENTS(ScriptComponent, Update);
		EXECUTE_ON_COMPONENTS(Animator, Update);
		EXECUTE_ON_COMPONENTS(RigidBody, Update);
	}

	void Scene::OnDestroy()
	{
		m_State = SceneState::Destroy;
		EXECUTE_ON_COMPONENTS(ScriptComponent, OnDestroy);
		EXECUTE_ON_COMPONENTS(Animator, OnDestroy);
		EXECUTE_ON_COMPONENTS(BoxCollider, OnDestroy);
		EXECUTE_ON_COMPONENTS(SphereCollider, OnDestroy);
		EXECUTE_ON_COMPONENTS(BoxCollider, OnDestroy);
		EXECUTE_ON_COMPONENTS(CharacterController, OnDestroy);
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