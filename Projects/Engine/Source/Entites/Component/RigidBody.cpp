#include "RigidBody.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "PhysicsSystem.h"
#include "Enum.h"

namespace Odyssey
{
	RigidBody::RigidBody(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	RigidBody::RigidBody(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{

	}

	void RigidBody::Awake()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			if (BoxCollider* collider = m_GameObject.TryGetComponent<BoxCollider>())
			{
				float3 extents = collider->GetExtents();
				float3 center = collider->GetCenter();

				float3 position, scale;
				quat rotation;
				transform->DecomposeWorldMatrix(position, rotation, scale);

				m_Body = PhysicsSystem::RegisterBox(center + position, rotation,  extents, m_Properties, m_PhysicsLayer);
				m_BodyID = m_Body->GetID();
			}
			else if (SphereCollider* sphereCollider = m_GameObject.TryGetComponent<SphereCollider>())
			{
				float3 center = sphereCollider->GetCenter();
				float radius = sphereCollider->GetRadius();

				float3 position, scale;
				quat rotation;
				transform->DecomposeWorldMatrix(position, rotation, scale);

				m_Body = PhysicsSystem::RegisterSphere(center + position, rotation, radius, m_Properties, m_PhysicsLayer);
				m_BodyID = m_Body->GetID();
			}
		}
	}

	void RigidBody::Update()
	{
		//if (BoxCollider* collider = m_GameObject.TryGetComponent<BoxCollider>())
		//{
		//	if (collider->GetLayer() == PhysicsLayer::Dynamic)
		//	{
		//		BodyInterface& bodyInterface = PhysicsSystem::GetBodyInterface();
		//		bodyInterface.AddLinearVelocity(m_BodyID, Vec3(0, -1.0f * Time::DeltaTime(), 0));
		//	}
		//}
	}

	void RigidBody::Destroy()
	{
		PhysicsSystem::Deregister(m_Body);
		m_Body = nullptr;
	}

	void RigidBody::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", RigidBody::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Physics Layer", Enum::ToString(m_PhysicsLayer));
		componentNode.WriteData("Friction", m_Properties.Friction);
		componentNode.WriteData("Max Linear Velocity", m_Properties.MaxLinearVelocity);
	}

	void RigidBody::Deserialize(SerializationNode& node)
	{
		std::string layer;
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Physics Layer", layer);
		node.ReadData("Friction", m_Properties.Friction);
		node.ReadData("Max Linear Velocity", m_Properties.MaxLinearVelocity);

		if (!layer.empty())
			m_PhysicsLayer = Enum::ToEnum<PhysicsLayer>(layer);
	}

	void RigidBody::AddLinearVelocity(float3 velocity)
	{
		if (m_Body)
		{
			BodyInterface& bodyInterface = PhysicsSystem::GetBodyInterface();
			Vec3 currentVelocity = bodyInterface.GetLinearVelocity(m_BodyID);
			bodyInterface.SetLinearVelocity(m_BodyID, currentVelocity + ToJoltVec3(velocity));
		}
	}

	float3 RigidBody::GetLinearVelocity()
	{
		if (m_Body)
		{
			BodyInterface& bodyInterface = PhysicsSystem::GetBodyInterface();
			Vec3 currentVelocity = bodyInterface.GetLinearVelocity(m_BodyID);
			return ToFloat3(currentVelocity);
		}

		return float3(0.0f);
	}

	float RigidBody::GetFriction()
	{
		return m_Properties.Friction;
	}

	float RigidBody::GetMaxLinearVelocity()
	{
		return m_Properties.MaxLinearVelocity;
	}

	void RigidBody::SetLinearVelocity(float3 velocity)
	{
		if (m_Body)
			PhysicsSystem::GetBodyInterface().SetLinearVelocity(m_BodyID, ToJoltVec3(velocity));
	}

	void RigidBody::SetMaxLinearVelocity(float velocity)
	{
		m_Properties.MaxLinearVelocity = velocity;

		if (m_Body)
			m_Body->GetMotionProperties()->SetMaxLinearVelocity(m_Properties.MaxLinearVelocity);
	}

	void RigidBody::SetFriction(float friction)
	{
		m_Properties.Friction = std::clamp(friction, 0.0f, 1.0f);

		if (m_Body)
			PhysicsSystem::GetBodyInterface().SetFriction(m_BodyID, m_Properties.Friction);
	}

	void RigidBody::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	BodyID RigidBody::GetBodyID()
	{
		return m_Body->GetID();
	}
}