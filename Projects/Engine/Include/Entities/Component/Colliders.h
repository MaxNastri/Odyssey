#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"

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
		void OnDestroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);
		void SetDebugEnabled(bool enabled);
		void SetCenter(float3 center) { m_Center = center; }
		void SetExtents(float3 extents) { m_Extents = extents; }

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsDebugEnabled() { return m_DebugEnabled; }
		float3 GetCenter() { return m_Center; }
		float3 GetExtents() { return m_Extents; }

	private:
		void DebugDraw();

	private:
		bool m_Enabled = true;
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
		GameObject m_GameObject;
		float3 m_Extents = float3(1.0f);
		float3 m_Center = float3(0.0f);
	};

	class CapsuleCollider
	{
		CLASS_DECLARATION(Odyssey, CapsuleCollider)
	public:
		CapsuleCollider() = default;
		CapsuleCollider(const GameObject& gameObject);
		CapsuleCollider(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake() { }
		void OnDestroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		void SetDebugEnabled(bool enabled);
		void SetCenter(float3 center) { m_Center = center; }
		void SetRadius(float radius) { m_Radius = radius; }
		void SetHeight(float height) { m_Height = height; }

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsDebugEnabled() { return m_DebugEnabled; }
		float3 GetCenter() { return m_Center; }
		float GetRadius() { return m_Radius; }
		float GetHeight() { return m_Height; }

	private:
		void DebugDraw();

	private:
		bool m_Enabled = true;
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
		GameObject m_GameObject;
		float3 m_Center = float3(0.0f);
		float m_Radius = 1.0f;
		float m_Height = 2.0f;
	};

	class SphereCollider
	{
		CLASS_DECLARATION(Odyssey, SphereCollider)
	public:
		SphereCollider() = default;
		SphereCollider(const GameObject& gameObject);
		SphereCollider(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake() { }
		void OnDestroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		void SetDebugEnabled(bool enabled);
		void SetCenter(float3 center) { m_Center = center; }
		void SetRadius(float radius) { m_Radius = radius; }

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsDebugEnabled() { return m_DebugEnabled; }
		float3 GetCenter() { return m_Center; }
		float GetRadius() { return m_Radius; }

	private:
		void DebugDraw();

	private:
		bool m_Enabled = true;
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
		GameObject m_GameObject;
		float3 m_Center = float3(0.0f);
		float m_Radius = 0.0f;
	};

}