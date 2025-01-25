#include "RigidBody.h"
#include "Colliders.h"
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
			else if (CapsuleCollider* capsuleCollider = m_GameObject.TryGetComponent<CapsuleCollider>())
			{
				float3 center = capsuleCollider->GetCenter();
				float radius = capsuleCollider->GetRadius();
				float height = capsuleCollider->GetHeight();

				float3 position, scale;
				quat rotation;
				transform->DecomposeWorldMatrix(position, rotation, scale);

				m_Body = PhysicsSystem::RegisterCapsule(center + position, rotation, radius, height, m_Properties, m_PhysicsLayer);
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

	void RigidBody::OnDestroy()
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
		componentNode.WriteData("Kinematic", m_Properties.Kinematic);
		componentNode.WriteData("Mass", m_Properties.Mass);
		componentNode.WriteData("Friction", m_Properties.Friction);
		componentNode.WriteData("Max Linear Velocity", m_Properties.MaxLinearVelocity);
		componentNode.WriteData("Gravity Factor", m_Properties.GravityFactor);
		componentNode.WriteData("Surface Velocity", m_Properties.SurfaceVelocity);
		componentNode.WriteData("Push Character", m_Properties.PushCharacter);
		componentNode.WriteData("Receive Force", m_Properties.ReceiveForce);
	}

	void RigidBody::Deserialize(SerializationNode& node)
	{
		std::string layer;
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Physics Layer", layer);
		node.ReadData("Kinematic", m_Properties.Kinematic);
		node.ReadData("Mass", m_Properties.Mass);
		node.ReadData("Friction", m_Properties.Friction);
		node.ReadData("Max Linear Velocity", m_Properties.MaxLinearVelocity);
		node.ReadData("Gravity Factor", m_Properties.GravityFactor);
		node.ReadData("Surface Velocity", m_Properties.SurfaceVelocity);
		node.ReadData("Push Character", m_Properties.PushCharacter);
		node.ReadData("Receive Force", m_Properties.ReceiveForce);

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

	void RigidBody::AddSurfaceVelocity(float3 velocity)
	{
		m_Properties.SurfaceVelocity += velocity;
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

	float3 RigidBody::GetSurfaceVelocity()
	{
		return m_Properties.SurfaceVelocity;
	}

	bool RigidBody::IsKinematic()
	{
		return m_Properties.Kinematic;
	}

	float RigidBody::GetMass()
	{
		return m_Properties.Mass;
	}

	float RigidBody::GetFriction()
	{
		return m_Properties.Friction;
	}

	float RigidBody::GetMaxLinearVelocity()
	{
		return m_Properties.MaxLinearVelocity;
	}

	bool RigidBody::CanPushCharacter()
	{
		return m_Properties.PushCharacter;
	}

	bool RigidBody::CanReceiveForce()
	{
		return m_Properties.ReceiveForce;
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

	void RigidBody::SetKinematic(bool kinematic)
	{
		m_Properties.Kinematic = kinematic;
	}

	void RigidBody::SetMass(float mass)
	{
		m_Properties.Mass = mass;
	}

	void RigidBody::SetFriction(float friction)
	{
		m_Properties.Friction = std::clamp(friction, 0.0f, 1.0f);

		if (m_Body)
			PhysicsSystem::GetBodyInterface().SetFriction(m_BodyID, m_Properties.Friction);
	}

	void RigidBody::SetSurfaceVelocity(float3 velocity)
	{
		m_Properties.SurfaceVelocity = velocity;
	}

	void RigidBody::SetPushCharacter(bool enabled)
	{
		m_Properties.PushCharacter = enabled;
	}

	void RigidBody::SetReceiveForce(bool enabled)
	{
		m_Properties.ReceiveForce = enabled;
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