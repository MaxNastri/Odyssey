#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"
#include <Jolt/Physics/Body/BodyID.h>

namespace JPH
{
	class Body;
}

namespace Odyssey
{
	struct BodyProperties
	{
	public:
		float Friction = 0.5f;
		float MaxLinearVelocity = 5.0f;
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
		void Destroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		float3 GetLinearVelocity();
		float GetFriction();
		float GetMaxLinearVelocity();

	public:
		void SetLinearVelocity(float3 velocity);
		void SetMaxLinearVelocity(float velocity);
		void SetFriction(float friction);
	public:
		void AddLinearVelocity(float3 velocity);

	public:
		void SetEnabled(bool enabled);

	public:
		bool IsEnabled() { return m_Enabled; }
		BodyID GetBodyID();

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Body* m_Body = nullptr;
		BodyID m_BodyID;

	private: // Serialized
		BodyProperties m_Properties;
	};
}