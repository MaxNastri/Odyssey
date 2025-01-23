#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"
#include "PhysicsLayers.h"

namespace Odyssey
{
	class BoxCollider
	{
		CLASS_DECLARATION(Odyssey, BoxCollider)
	public:
		BoxCollider() = default;
		BoxCollider(const GameObject& gameObject);
		BoxCollider(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake() { }
		void Destroy() { }
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);
		void SetDebugEnabled(bool enabled);
		void SetLayer(PhysicsLayer layer) { m_PhysicsLayer = layer; }
		void SetCenter(float3 center) { m_Center = center; }
		void SetExtents(float3 extents) { m_Extents = extents; }

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsDebugEnabled() { return m_DebugEnabled; }
		PhysicsLayer GetLayer() { return m_PhysicsLayer; }
		float3 GetCenter() { return m_Center; }
		float3 GetExtents() { return m_Extents; }

	private:
		void DebugDraw();

	private:
		bool m_Enabled = true;
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
		GameObject m_GameObject;
		PhysicsLayer m_PhysicsLayer = PhysicsLayer::Static;
		float3 m_Extents = float3(1.0f);
		float3 m_Center = float3(0.0f);
	};
}