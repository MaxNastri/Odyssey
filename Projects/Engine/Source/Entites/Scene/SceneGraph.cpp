#include "SceneGraph.h"
#include "PropertiesComponent.h"
#include "Scene.h"

namespace Odyssey
{
	SceneGraph::SceneGraph()
	{
		m_Root = std::make_shared<Node>();
	}

	void SceneGraph::Serialize(Scene* scene, SerializationNode& serializationNode)
	{
		SerializationNode sceneGraphNode = serializationNode.CreateSequenceNode("Scene Graph Nodes");

		for (auto& node : m_Nodes)
		{
			// Skip game objects who dont' want to be serialized
			PropertiesComponent properties = node->Entity.GetComponent<PropertiesComponent>();
			if (!properties.Serialize)
				continue;

			SerializationNode sceneNode = sceneGraphNode.AppendChild();
			sceneNode.SetMap();

			sceneNode.WriteData("Entity", node->Entity.GetGUID());

			GUID parent = GUID(0);
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
			std::shared_ptr<Node> Node;
			GUID Parent;
		};
		std::vector<NodeConnection> connections;

		for (size_t i = 0; i < sceneGraphNode.ChildCount(); i++)
		{
			SerializationNode sceneNode = sceneGraphNode.GetChild(i);

			assert(sceneNode.IsMap());

			std::shared_ptr<Node> node = m_Nodes.emplace_back(std::make_shared<Node>());
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
		auto node = std::make_shared<Node>(entity);
		node->Parent = m_Root;
		m_Nodes.push_back(node);
		m_Root->Children.push_back(node);
	}

	void SceneGraph::RemoveEntityAndChildren(const GameObject& entity)
	{
		if (auto node = FindNode(entity))
			RemoveNodeAndChildren(node);
	}

	void SceneGraph::SetParent(const GameObject& parent, const GameObject& entity)
	{
		auto parentNode = FindNode(parent);
		auto node = FindNode(entity);

		if (node && parentNode)
		{
			RemoveParent(node);
			node->Parent = parentNode;
			node->Parent->Children.push_back(node);
		}
	}

	void SceneGraph::RemoveParent(const GameObject& entity)
	{
		if (auto node = FindNode(entity))
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
		std::shared_ptr<Node> node = FindNode(entity);
		std::shared_ptr<Node> childNode = FindNode(child);

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

	std::shared_ptr<SceneGraph::Node> SceneGraph::FindNode(const GameObject& entity)
	{
		for (size_t i = 0; i < m_Nodes.size(); i++)
		{
			if (m_Nodes[i]->Entity.Equals(entity))
				return m_Nodes[i];
		}

		return nullptr;
	}

	void SceneGraph::RemoveNodeAndChildren(std::shared_ptr<Node> node)
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

	void SceneGraph::RemoveParent(std::shared_ptr<Node> node)
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

	void SceneGraph::RemoveChildNode(std::shared_ptr<Node> parentNode, std::shared_ptr<Node> childNode)
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