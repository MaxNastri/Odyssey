#include "SceneGraph.h"
#include "PropertiesComponent.h"
#include "Scene.h"

namespace Odyssey
{
	SceneGraph::SceneGraph()
	{
		m_Root = new SceneNode();
	}

	void SceneGraph::Serialize(Scene* scene, SerializationNode& serializationNode)
	{
		SerializationNode sceneGraphNode = serializationNode.CreateSequenceNode("Scene Graph Nodes");

		for (Ref<SceneNode>& node : m_Nodes)
		{
			// Skip game objects who dont' want to be serialized
			PropertiesComponent properties = node->Entity.GetComponent<PropertiesComponent>();
			if (!properties.Serialize)
				continue;

			SerializationNode sceneNode = sceneGraphNode.AppendChild();
			sceneNode.SetMap();

			sceneNode.WriteData("Entity", node->Entity.GetGUID());

			GUID parent;

			if (node->Parent && node->Parent->Entity.IsValid())
				parent = node->Parent->Entity.GetGUID();

			sceneNode.WriteData("Parent", parent);
		}
	}

	void SceneGraph::Deserialize(Scene* scene, SerializationNode& serializationNode)
	{
		SerializationNode sceneGraphNode = serializationNode.GetNode("Scene Graph Nodes");
		assert(sceneGraphNode.IsSequence());
		assert(sceneGraphNode.HasChildren());

		struct NodeConnection
		{
			Ref<SceneNode> Node;
			GUID Parent;
		};
		std::vector<NodeConnection> connections;

		for (size_t i = 0; i < sceneGraphNode.ChildCount(); i++)
		{
			SerializationNode sceneNode = sceneGraphNode.GetChild(i);

			assert(sceneNode.IsMap());

			Ref<SceneNode> node = m_Nodes.emplace_back(new SceneNode());
			NodeConnection& connection = connections.emplace_back();
			connection.Node = node;

			GUID entity;
			sceneNode.ReadData("Entity", entity.Ref());
			sceneNode.ReadData("Parent", connection.Parent.Ref());

			if (entity)
				node->Entity = scene->GetGameObject(entity);
		}

		for (auto& connection : connections)
		{
			if (connection.Parent)
			{
				GameObject parent = scene->GetGameObject(connection.Parent);
				connection.Node->Parent = GetNode(parent);
				connection.Node->Parent->Children.push_back(connection.Node);

				PropertiesComponent& properties = connection.Node->Entity.GetComponent<PropertiesComponent>();
				if (properties.SortOrder == -1)
					properties.SortOrder = connection.Node->Parent->Children.size() - 1;
			}
			else
			{
				connection.Node->Parent = m_Root;
				m_Root->Children.push_back(connection.Node);

				PropertiesComponent& properties = connection.Node->Entity.GetComponent<PropertiesComponent>();
				if (properties.SortOrder == -1)
					properties.SortOrder = m_Root->Children.size() - 1;
			}
		}

		m_Root->SortChildren();
	}

	void SceneGraph::AddEntity(const GameObject& entity)
	{
		// Create a new scenenode
		Ref<SceneNode> node = new SceneNode(entity);

		// Set the node's parent as the root and add to the root's children
		node->Parent = m_Root;
		m_Nodes.push_back(node);
		m_Root->Children.push_back(node);

		// Apply default sort order logic if non is set
		PropertiesComponent& properties = node->Entity.GetComponent<PropertiesComponent>();
		if (properties.SortOrder == -1)
			properties.SortOrder = m_Root->Children.size() - 1;

		// Sort the root
		m_Root->SortChildren();
	}

	void SceneGraph::RemoveEntityAndChildren(const GameObject& entity)
	{
		if (Ref<SceneNode> node = GetNode(entity))
			RemoveNodeAndChildren(node);
	}

	void SceneGraph::SetParent(const GameObject& parent, const GameObject& entity)
	{
		Ref<SceneNode> parentNode = GetNode(parent);
		Ref<SceneNode> node = GetNode(entity);

		if (node && parentNode)
		{
			RemoveParent(node);
			node->Parent = parentNode;
			node->Parent->Children.push_back(node);

			// Change the node's sort order to the bottom of the children
			PropertiesComponent& properties = node->Entity.GetComponent<PropertiesComponent>();
			properties.SortOrder = node->Children.size() - 1;
		}
	}

	void SceneGraph::RemoveParent(const GameObject& entity)
	{
		if (Ref<SceneNode> node = GetNode(entity))
		{
			RemoveParent(node);

			// Re-child the node to the root
			node->Parent = m_Root;
			m_Root->Children.push_back(node);
		}
	}

	GameObject SceneGraph::GetParent(const GameObject& entity)
	{
		if (auto node = GetNode(entity))
		{
			if (node->Parent)
				return node->Parent->Entity;
		}

		return GameObject();
	}

	void SceneGraph::AddChild(const GameObject& entity, const GameObject& child)
	{
		auto node = GetNode(entity);
		auto childNode = GetNode(child);

		if (node && childNode)
		{
			RemoveParent(childNode);
			childNode->Parent = node;
			node->Children.push_back(childNode);

			// Change the node's sort order to the bottom of the children
			PropertiesComponent& properties = childNode->Entity.GetComponent<PropertiesComponent>();
			properties.SortOrder = node->Children.size() - 1;
		}
	}

	void SceneGraph::RemoveChild(const GameObject& entity, const GameObject& child)
	{
		Ref<SceneNode> node = GetNode(entity);
		Ref<SceneNode> childNode = GetNode(child);

		if (node && childNode)
		{
			RemoveParent(childNode);
			RemoveChildNode(node, childNode);
		}
	}

	std::vector<GameObject> SceneGraph::GetChildren(const GameObject& entity)
	{
		std::vector<GameObject> children;

		if (auto node = GetNode(entity))
		{
			for (size_t i = 0; i < node->Children.size(); i++)
			{
				children.push_back(node->Children[i]->Entity);
			}
		}

		return children;
	}

	Ref<SceneNode> SceneGraph::GetNode(const GameObject& entity)
	{
		for (size_t i = 0; i < m_Nodes.size(); i++)
		{
			if (m_Nodes[i]->Entity.Equals(entity))
				return m_Nodes[i];
		}

		return nullptr;
	}

	void SceneGraph::RemoveNodeAndChildren(Ref<SceneNode> node)
	{
		for (size_t i = 0; i < node->Children.size(); i++)
		{
			RemoveNodeAndChildren(node->Children[i]);
		}

		// Remove this node from the parent's child list
		if (node->Parent)
			RemoveChildNode(node->Parent, node);

		for (size_t i = 0; i < m_Nodes.size(); i++)
		{
			if (m_Nodes[i]->Entity.Equals(node->Entity))
			{
				m_Nodes.erase(m_Nodes.begin() + i);
				break;
			}
		}
	}

	void SceneGraph::RemoveParent(Ref<SceneNode> node)
	{
		auto parent = node->Parent;

		if (parent)
		{
			auto& children = parent->Children;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (children[i]->Entity.Equals(node->Entity))
				{
					children.erase(children.begin() + i);
					break;
				}
			}
		}

	}

	void SceneGraph::RemoveChildNode(Ref<SceneNode> parentNode, Ref<SceneNode> childNode)
	{
		auto& children = parentNode->Children;

		for (size_t i = 0; i < children.size(); i++)
		{
			if (children[i]->Entity.Equals(childNode->Entity))
			{
				children.erase(children.begin() + i);
				break;
			}
		}
	}

	void SceneNode::SortChildren(bool recursive)
	{
		struct CustomSort
		{
			bool operator()(Ref<SceneNode> a, Ref<SceneNode> b) const
			{
				PropertiesComponent& propertiesA = a->Entity.GetComponent<PropertiesComponent>();
				PropertiesComponent& propertiesB = b->Entity.GetComponent<PropertiesComponent>();

				return propertiesA.SortOrder < propertiesB.SortOrder;
			}
		} customSort;

		std::sort(Children.begin(), Children.end(), customSort);

		if (recursive)
		{
			for (size_t i = 0; i < Children.size(); i++)
				Children[i]->SortChildren(recursive);
		}
	}
}