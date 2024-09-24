#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	class SceneGraph
	{
	public:
		struct Node
		{
		public:
			Node() = default;
			Node(const GameObject& entity) : Entity(entity) { }

		public:
			std::shared_ptr<Node> Parent = nullptr;
			GameObject Entity;
			std::vector<std::shared_ptr<Node>> Children;
		};

	public:
		SceneGraph();

	public:
		void Serialize(Scene* scene, SerializationNode& serializationNode);
		void Deserialize(Scene* scene, SerializationNode& serializationNode);

	public:
		const Node* GetSceneRoot() { return m_Root.get(); }

	public:
		void AddEntity(const GameObject& entity);
		void RemoveEntityAndChildren(const GameObject& entity);
		void SetParent(const GameObject& parent, const GameObject& entity);
		void RemoveParent(const GameObject& entity);
		GameObject GetParent(const GameObject& entity);
		void AddChild(const GameObject& entity, const GameObject& child);
		void RemoveChild(const GameObject& entity, const GameObject& child);
		std::vector<GameObject> GetChildren(const GameObject& entity);

	private:
		std::shared_ptr<Node> FindNode(const GameObject& entity);
		void RemoveNodeAndChildren(std::shared_ptr<Node> node);
		void RemoveParent(std::shared_ptr<Node> node);
		void RemoveChildNode(std::shared_ptr<Node> parentNode, std::shared_ptr<Node> childNode);

	private:
		std::shared_ptr<Node> m_Root;
		std::vector<std::shared_ptr<Node>> m_Nodes;
	};
}