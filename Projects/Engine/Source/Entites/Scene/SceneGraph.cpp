#include "SceneGraph.h"

namespace Odyssey
{
	SceneGraph::SceneGraph()
	{
		m_Root = std::make_shared<Node>();
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
			RemoveParent(node);
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
		if (auto parent = node->Parent)
		{
			auto& children = parent->Children;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (children[i]->Entity.Equals(node->Entity))
					children.erase(children.begin() + i);
			}
		}

		node->Parent = nullptr;
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