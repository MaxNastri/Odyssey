#include "SceneGraph.h"
#include "PropertiesComponent.h"
#include "Scene.h"

namespace Odyssey
{
	SceneGraph::SceneGraph()
	{
		m_Root = new Node();
	}

	void SceneGraph::Serialize(Scene* scene, SerializationNode& serializationNode)
	{
		SerializationNode sceneGraphNode = serializationNode.CreateSequenceNode("Scene Graph Nodes");

		for (Ref<SceneGraph::Node>& node : m_Nodes)
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
			Ref<Node> Node;
			GUID Parent;
		};
		std::vector<NodeConnection> connections;

		for (size_t i = 0; i < sceneGraphNode.ChildCount(); i++)
		{
			SerializationNode sceneNode = sceneGraphNode.GetChild(i);

			assert(sceneNode.IsMap());

			Ref<Node> node = m_Nodes.emplace_back(new Node());
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
				connection.Node->Parent = FindNode(parent);
				connection.Node->Parent->Children.push_back(connection.Node);
			}
			else
			{
				connection.Node->Parent = m_Root;
				m_Root->Children.push_back(connection.Node);
			}
		}
	}

	void SceneGraph::AddEntity(const GameObject& entity)
	{
		Ref<SceneGraph::Node> node = new Node(entity);
		node->Parent = m_Root;
		m_Nodes.push_back(node);
		m_Root->Children.push_back(node);
	}

	void SceneGraph::RemoveEntityAndChildren(const GameObject& entity)
	{
		if (Ref<SceneGraph::Node> node = FindNode(entity))
			RemoveNodeAndChildren(node);
	}

	void SceneGraph::SetParent(const GameObject& parent, const GameObject& entity)
	{
		Ref<SceneGraph::Node> parentNode = FindNode(parent);
		Ref<SceneGraph::Node> node = FindNode(entity);

		if (node && parentNode)
		{
			RemoveParent(node);
			node->Parent = parentNode;
			node->Parent->Children.push_back(node);
		}
	}

	void SceneGraph::RemoveParent(const GameObject& entity)
	{
		if (Ref<SceneGraph::Node> node = FindNode(entity))
		{
			RemoveParent(node);

			// Re-child the node to the root
			node->Parent = m_Root;
			m_Root->Children.push_back(node);
		}
	}

	GameObject SceneGraph::GetParent(const GameObject& entity)
	{
		if (auto node = FindNode(entity))
		{
			if (node->Parent)
				return node->Parent->Entity;
		}

		return GameObject();
	}

	void SceneGraph::AddChild(const GameObject& entity, const GameObject& child)
	{
		auto node = FindNode(entity);
		auto childNode = FindNode(child);

		if (node && childNode)
		{
			RemoveParent(childNode);
			childNode->Parent = node;
			node->Children.push_back(childNode);
		}
	}

	void SceneGraph::RemoveChild(const GameObject& entity, const GameObject& child)
	{
		Ref<Node> node = FindNode(entity);
		Ref<Node> childNode = FindNode(child);

		if (node && childNode)
		{
			RemoveParent(childNode);
			RemoveChildNode(node, childNode);
		}
	}

	std::vector<GameObject> SceneGraph::GetChildren(const GameObject& entity)
	{
		std::vector<GameObject> children;

		if (auto node = FindNode(entity))
		{
			for (size_t i = 0; i < node->Children.size(); i++)
			{
				children.push_back(node->Children[i]->Entity);
			}
		}

		return children;
	}

	Ref<SceneGraph::Node> SceneGraph::FindNode(const GameObject& entity)
	{
		for (size_t i = 0; i < m_Nodes.size(); i++)
		{
			if (m_Nodes[i]->Entity.Equals(entity))
				return m_Nodes[i];
		}

		return nullptr;
	}

	void SceneGraph::RemoveNodeAndChildren(Ref<Node> node)
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

	void SceneGraph::RemoveParent(Ref<Node> node)
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

	void SceneGraph::RemoveChildNode(Ref<Node> parentNode, Ref<Node> childNode)
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
}