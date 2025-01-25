#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"
#include "PhysicsLayers.h"

namespace JPH
{
	class Body;
}

namespace Odyssey
{
	struct BodyProperties
	{
	public:
		bool Kinematic = false;
		float Mass = 0.0f;
		float Friction = 0.5f;
		float MaxLinearVelocity = 5.0f;
		float GravityFactor = 1.0f;
		float3 SurfaceVelocity = float3(0.0f);
	};

	class RigidBody
	{
		CLASS_DECLARATION(Odyssey, RigidBody)
	public:
		RigidBody() = default;
		RigidBody(const GameObject& gameObject);
		RigidBody(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake();
		void Update();
		void OnDestroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		float3 GetLinearVelocity();
		float3 GetSurfaceVelocity();
		bool IsKinematic();
		float GetMass();
		float GetFriction();
		float GetMaxLinearVelocity();

	public:
		void SetLinearVelocity(float3 velocity);
		void SetMaxLinearVelocity(float velocity);
		void SetKinematic(bool kinematic);
		void SetMass(float mass);
		void SetFriction(float friction);
		void SetSurfaceVelocity(float3 velocity);

	public:
		void AddLinearVelocity(float3 velocity);
		void AddSurfaceVelocity(float3 velocity);

	public:
		void SetEnabled(bool enabled);
		void SetLayer(PhysicsLayer layer) { m_PhysicsLayer = layer; }

	public:
		bool IsEnabled() { return m_Enabled; }
		PhysicsLayer GetLayer() { return m_PhysicsLayer; }
		BodyID GetBodyID();

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Body* m_Body = nullptr;
		BodyID m_BodyID;

	private: // Serialized
		PhysicsLayer m_PhysicsLayer = PhysicsLayer::Static;
		BodyProperties m_Properties;
	};
}