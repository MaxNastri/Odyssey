#pragma once
#include "Ref.h"
#include "GameObject.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	struct SceneNode
	{
	public:
		SceneNode() = default;
		SceneNode(const GameObject& entity) : Entity(entity) { }

	public:
		void SortChildren(bool recursive = false);

	public:
		Ref<SceneNode> Parent = nullptr;
		GameObject Entity;
		std::vector<Ref<SceneNode>> Children;
	};

	class SceneGraph
	{
	public:
		SceneGraph();

	public:
		void Serialize(Scene* scene, SerializationNode& serializationNode);
		void Deserialize(Scene* scene, SerializationNode& serializationNode);

	public:
		Ref<SceneNode>& GetSceneRoot() { return m_Root; }

	public:
		void AddEntity(const GameObject& entity);
		void RemoveEntityAndChildren(const GameObject& entity);
		void SetParent(const GameObject& parent, const GameObject& entity);
		void RemoveParent(const GameObject& entity);
		GameObject GetParent(const GameObject& entity);
		void AddChild(const GameObject& entity, const GameObject& child);
		void RemoveChild(const GameObject& entity, const GameObject& child);
		std::vector<GameObject> GetChildren(const GameObject& entity);
		std::vector<GameObject> GetAllChildren(const GameObject& entity);
		Ref<SceneNode> GetNode(const GameObject& entity);

	private:
		void RemoveNode(Ref<SceneNode> node);
		void RemoveParent(Ref<SceneNode> node);
		void RemoveChildNode(Ref<SceneNode> parentNode, Ref<SceneNode> childNode);
		void GetAllChildren(const GameObject& entity, std::vector<GameObject>& children);

	private:
		Ref<SceneNode> m_Root;
		std::vector<Ref<SceneNode>> m_Nodes;
	};
}