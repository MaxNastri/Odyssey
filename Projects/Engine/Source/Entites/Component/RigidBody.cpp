#include "RigidBody.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "PhysicsSystem.h"
#include "OdysseyTime.h"

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
		if (BoxCollider* collider = m_GameObject.TryGetComponent<BoxCollider>())
		{
			if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			{
				PhysicsLayer layer = collider->GetLayer();
				float3 extents = collider->GetExtents();
				float3 center = collider->GetCenter();
				float3 worldPos = transform->GetWorldPosition();

				m_BodyID = PhysicsSystem::Register(center + worldPos, extents, layer);
			}
		}
	}

	void RigidBody::Update()
	{
		if (BoxCollider* collider = m_GameObject.TryGetComponent<BoxCollider>())
		{
			if (collider->GetLayer() == PhysicsLayer::Dynamic)
			{
				BodyInterface& bodyInterface = PhysicsSystem::GetBodyInterface();
				bodyInterface.AddLinearVelocity(m_BodyID, Vec3(0, -1.0f, 0));
			}
		}
	}

	void RigidBody::Destroy()
	{
		PhysicsSystem::Deregister(m_BodyID);
	}

	void RigidBody::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", RigidBody::Type);
		componentNode.WriteData("Enabled", m_Enabled);
	}

	void RigidBody::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
	}

	void RigidBody::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}
}