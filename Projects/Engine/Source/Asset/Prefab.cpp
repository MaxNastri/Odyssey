#include "Prefab.h"
#include "SceneManager.h"
#include "PropertiesComponent.h"

namespace Odyssey
{
	Prefab::Prefab(const Path& assetPath)
		: Asset(assetPath)
	{
		Load();
	}

	Prefab::Prefab(const Path& assetPath, GameObject& instance)
		: Asset(assetPath)
	{
		Save(instance);
	}

	void SerializeNode(Ref<SceneNode> node, SerializationNode& gameObjectsNode, int32_t parentID, int32_t& nodeID)
	{
		// Get the GUID of the parent node, if a parent exists
		GUID parent = node->Parent->Entity.IsValid() ? node->Parent->Entity.GetGUID() : GUID::Empty();
		
		PropertiesComponent properties = node->Entity.GetComponent<PropertiesComponent>();
		// Append a child and set map
		SerializationNode gameObjectNode = gameObjectsNode.AppendChild();
		gameObjectNode.SetMap();
		gameObjectNode.WriteData("Sort Order", properties.SortOrder);
		gameObjectNode.WriteData("ID", nodeID);
		gameObjectNode.WriteData("Parent", parentID);

		node->Entity.Serialize(gameObjectNode);

		parentID = nodeID;

		// Serialize the node's children (depth first)
		for (size_t i = 0; i < node->Children.size(); i++)
		{
			nodeID++;
			SerializeNode(node->Children[i], gameObjectsNode, parentID, nodeID);
		}
	}

	void Prefab::Save(GameObject& prefabInstance)
	{
		// Create the serializer
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Create a list of game objects
		SerializationNode gameObjectsNode = root.CreateSequenceNode("GameObjects");

		// Get the scene graph
		Scene* scene = SceneManager::GetActiveScene();
		SceneGraph& sceneGraph = scene->GetSceneGraph();
		
		// Get the scene graph node for the game object
		Ref<SceneNode> node = sceneGraph.GetNode(prefabInstance);

		// Start serializing
		int32_t nodeID = 0;
		SerializeNode(node, gameObjectsNode, -1, nodeID);

		serializer.WriteToDisk(m_AssetPath);
	}

	void Prefab::Load()
	{
		AssetDeserializer deserializer(m_AssetPath);

		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			SerializationNode gameObjectsNode = root.GetNode("GameObjects");

			assert(gameObjectsNode.IsSequence());
			assert(gameObjectsNode.HasChildren());

			// Get the scene graph
			Scene* scene = SceneManager::GetActiveScene();
			SceneGraph& sceneGraph = scene->GetSceneGraph();

			std::map<GUID, size_t> childToParentMap;
			std::map<size_t, GUID> nodeIDToGUID;

			for (size_t i = 0; i < gameObjectsNode.ChildCount(); i++)
			{
				SerializationNode gameObjectNode = gameObjectsNode.GetChild(i);
				assert(gameObjectNode.IsMap());

				GameObject gameObject = scene->CreateGameObject();
				PropertiesComponent properties = gameObject.GetComponent<PropertiesComponent>();

				int32_t parent = -1;
				int32_t nodeID = -1;
				gameObjectNode.ReadData("Sort Order", properties.SortOrder);
				gameObjectNode.ReadData("ID", nodeID);
				gameObjectNode.ReadData("Parent", parent);

				gameObject.DeserializeAsPrefab(gameObjectNode);

				GUID guid = gameObject.GetGUID();

				// Insert into the nodeID -> GUID lookup
				nodeIDToGUID[nodeID] = guid;

				// Insert into the GUID -> parent node ID lookup
				if (parent >= 0)
					childToParentMap[guid] = parent;
			}

			// Change to local id per game object
			for (auto& [childGUID, parentNodeID] : childToParentMap)
			{
				GUID parentGUID = nodeIDToGUID[parentNodeID];
				GameObject child = scene->GetGameObject(childGUID);
				GameObject parent = scene->GetGameObject(parentGUID);
				sceneGraph.SetParent(parent, child);
			}
		}
	}
}