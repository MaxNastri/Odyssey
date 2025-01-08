#include "Prefab.h"
#include "SceneManager.h"
#include "PropertiesComponent.h"

namespace Odyssey
{
	Prefab::Prefab(const Path& assetPath)
		: Asset(assetPath)
	{
	}

	Prefab::Prefab(const Path& assetPath, GameObject& instance)
		: Asset(assetPath)
	{
		Save(instance);
	}

	void SerializeGameObject(GameObject& gameObject, SerializationNode& gameObjectsNode, std::map<GUID, GUID>& remap, bool serializeParent = true)
	{
		// Get the GUID of the parent node, if a parent exists
		GUID parent = gameObject.GetParent().IsValid() && serializeParent ? gameObject.GetParent().GetGUID() : GUID::Empty();
		
		// Check the parent's guid against the remap
		if (remap.contains(parent))
			parent = remap[parent];

		PropertiesComponent properties = gameObject.GetComponent<PropertiesComponent>();

		// Append a child and set map
		SerializationNode gameObjectNode = gameObjectsNode.AppendChild();
		gameObjectNode.SetMap();
		gameObjectNode.WriteData("Sort Order", properties.SortOrder);
		gameObjectNode.WriteData("Parent", parent.CRef());

		gameObject.SerializeAsPrefab(gameObjectNode, remap);
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
		
		// Get all child game objects of the instance
		std::vector<GameObject> children = sceneGraph.GetAllChildren(prefabInstance);

		// Remap the instance's guids to brand new guids before serializing
		std::map<GUID, GUID> remap;
		remap[prefabInstance.GetGUID()] = GUID::New();

		// Remap children as well
		for (auto& child : children)
			remap[child.GetGUID()] = GUID::New();

		// When we serialize, pass along the remap to serialize the new guids and update any references to the old guids
		SerializeGameObject(prefabInstance, gameObjectsNode, remap, false);

		for (auto& child : children)
			SerializeGameObject(child, gameObjectsNode, remap);

		serializer.WriteToDisk(m_AssetPath);
	}

	GameObject Prefab::LoadInstance()
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

			// For deserialization, we remap from the GUID stored in the prefab to a brand new instance guid
			std::map<GUID, GUID> remap;

			GameObject baseGameObject;

			// Run through and gather the guids first so we can remap
			for (size_t i = 0; i < gameObjectsNode.ChildCount(); i++)
			{
				SerializationNode gameObjectNode = gameObjectsNode.GetChild(i);
				assert(gameObjectNode.IsMap());

				// Read the guid only for now
				GUID guid;
				gameObjectNode.ReadData("GUID", guid.Ref());

				// Remap the guid to a brand new instance guid
				GameObject gameObject = scene->CreateGameObject();
				remap[guid] = gameObject.GetGUID();
			}

			std::map<GUID, GUID> gameObjectToParent;

			// Now run through again and deserialize fully using the remap
			for (size_t i = 0; i < gameObjectsNode.ChildCount(); i++)
			{
				SerializationNode gameObjectNode = gameObjectsNode.GetChild(i);
				assert(gameObjectNode.IsMap());

				GUID parent;
				GUID guid;
				int64_t sortOrder;
				gameObjectNode.ReadData("Sort Order", sortOrder);
				gameObjectNode.ReadData("Parent", parent.Ref());
				gameObjectNode.ReadData("GUID", guid.Ref());

				// Remap to the new instance guid
				GUID instanceGUID = remap[guid];

				// The game object was created in the last loop through when gathering guids
				GameObject gameObject = scene->GetGameObject(instanceGUID);
				PropertiesComponent properties = gameObject.GetComponent<PropertiesComponent>();
				properties.SortOrder = sortOrder;

				// Deserialize as a prefab, passing along the remap
				gameObject.DeserializeAsPrefab(gameObjectNode, remap);

				if (parent)
					gameObjectToParent[instanceGUID] = remap[parent];
				else
					baseGameObject = gameObject;

			}

			// Modify the scene graph to apply the hierarchy
			for (auto& [childGUID, parentGUID] : gameObjectToParent)
			{
				GameObject child = scene->GetGameObject(childGUID);
				GameObject parent = scene->GetGameObject(parentGUID);
				sceneGraph.SetParent(parent, child);
			}

			// Re-sort after applying the new hierarchy
			sceneGraph.GetNode(baseGameObject)->SortChildren(true);

			return baseGameObject;
		}

		return GameObject();
	}
}