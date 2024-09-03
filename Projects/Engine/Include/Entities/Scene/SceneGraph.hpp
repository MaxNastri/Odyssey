#pragma once
#include "GameObject.h"

namespace Odyssey
{
	class SceneGraph
	{
	private:
		struct Node
		{
		public:
			Node() = default;
			Node(const GameObject& entity) : Entity(entity) { }

		public:
			Node* Parent = nullptr;
			GameObject Entity;
			std::vector<Node*> Children;
		};

	public:
		SceneGraph() = default;

	public:
		void AddEntity(const GameObject& parent, const GameObject& entity)
		{
			Node* parentNode = FindOrCreateNode(parent);

			Node* entityNode = FindOrCreateNode(entity);
			entityNode->Parent;
		}

		void AddEntity(const GameObject& entity)
		{
			FindOrCreateNode(entity);
		}

		GameObject GetParent(const GameObject& entity)
		{
			if (Node* node = FindNode(entity))
				if (node->Parent)
					return node->Parent->Entity;

			return GameObject();
		}

		void SetParent(const GameObject& parent, const GameObject& entity)
		{
			if (Node* parentNode = FindNode(parent))
				if (Node* node = FindNode(entity))
					node->Parent = parentNode;
		}

		void RemoveParent(const GameObject& entity)
		{
			if (Node* node = FindNode(entity))
				node->Parent = nullptr;
		}

		void AddChild(const GameObject& entity, const GameObject& child)
		{
			if (Node* node = FindNode(entity))
			{
				Node* childNode = FindOrCreateNode(child);
				node->Children.push_back(childNode);
			}
		}

		void RemoveChild(const GameObject& entity, const GameObject& child)
		{
			if (Node* node = FindNode(entity))
			{
				auto& children = node->Children;
				for (size_t i = 0; i < children.size(); i++)
				{
					if (children[i]->Entity.Equals(child))
					{
						children.erase(children.begin() + i);
						break;
					}
				}
			}
		}

		std::vector<GameObject> GetChildren(const GameObject& entity)
		{
			std::vector<GameObject> children;

			if (Node* node = FindNode(entity))
			{
				for (size_t i = 0; i < node->Children.size(); i++)
				{
					children.push_back(node->Children[i]->Entity);
				}
			}

			return children;
		}

		void RemoveEntity(const GameObject& entity)
		{
			for (size_t i = 0; i < m_Nodes.size(); i++)
			{
				if (m_Nodes[i].Entity.Equals(entity))
				{
					m_Nodes.erase(m_Nodes.begin() + i);
					break;
				}
			}
		}

	private:
		Node* FindNode(const GameObject& entity)
		{
			for (size_t i = 0; i < m_Nodes.size(); i++)
			{
				if (m_Nodes[i].Entity.Equals(entity))
					return &m_Nodes[i];
			}

			return nullptr;
		}

		Node* FindOrCreateNode(const GameObject& entity)
		{
			if (Node* node = FindNode(entity))
				return node;

			return &m_Nodes.emplace_back(Node(entity));
		}

	private:
		std::vector<Node> m_Nodes;
	};
}