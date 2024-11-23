#pragma once
#include "Ref.h"
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
			Ref<Node> Parent = nullptr;
			GameObject Entity;
			std::vector<Ref<Node>> Children;
		};

	public:
		SceneGraph();

	public:
		void Serialize(Scene* scene, SerializationNode& serializationNode);
		void Deserialize(Scene* scene, SerializationNode& serializationNode);

	public:
		const Node* GetSceneRoot() { return m_Root.Get(); }

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
		Ref<Node> FindNode(const GameObject& entity);
		void RemoveNodeAndChildren(Ref<Node> node);
		void RemoveParent(Ref<Node> node);
		void RemoveChildNode(Ref<Node> parentNode, Ref<Node> childNode);

	private:
		Ref<Node> m_Root;
		std::vector<Ref<Node>> m_Nodes;
	};
}