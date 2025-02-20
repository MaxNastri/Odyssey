#include "FluidBody.h"
#include "Transform.h"
#include "OdysseyTime.h"
#include "DebugRenderer.h"

namespace Odyssey
{
	FluidBody::FluidBody(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	FluidBody::FluidBody(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void FluidBody::Awake()
	{

	}

	void FluidBody::Update()
	{

	}

	void FluidBody::OnDestroy()
	{
		SetDebugEnabled(false);
	}

	void FluidBody::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", FluidBody::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Extents", m_Extents);
		componentNode.WriteData("Buoyancy", m_Buoyancy);
		componentNode.WriteData("Linear Drag", m_LinearDrag);
		componentNode.WriteData("Angular Drag", m_AngularDrag);
		componentNode.WriteData("Fluid Velocity", m_FluidVelocity);
		componentNode.WriteData("Gravity Factor", m_GravityFactor);
	}

	void FluidBody::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Center", m_Center);
		node.ReadData("Extents", m_Extents);
		node.ReadData("Buoyancy", m_Buoyancy);
		node.ReadData("Linear Drag", m_LinearDrag);
		node.ReadData("Angular Drag", m_AngularDrag);
		node.ReadData("Fluid Velocity", m_FluidVelocity);
		node.ReadData("Gravity Factor", m_GravityFactor);
	}

	void FluidBody::CheckCollision(const BroadPhaseQuery& query)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			// Cache the world position and up for each hit
			float3 position, scale;
			quat rotation;
			transform->DecomposeWorldMatrix(position, rotation, scale);

			m_SurfacePosition = ToJoltVec3(position + m_Center + vec3(0.0f, 1.0f, 0.0f));
			m_SurfaceNormal = ToJoltVec3(transform->Up());

			// Calculate the AABB
			Vec3 extents = ToJoltVec3(m_Extents);
			AABox aabb(-extents, extents);
			aabb.Translate(m_SurfacePosition);

			query.CollideAABox(aabb, *this, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::Dynamic), SpecifiedObjectLayerFilter(PhysicsLayers::Dynamic));
		}
	}

	void FluidBody::AddHit(const BodyID& inBodyID)
	{
		BodyLockWrite* lock = PhysicsSystem::Instance().GetBodyLock(inBodyID);
		Body& body = lock->GetBody();

		if (body.IsActive() && !body.IsKinematic())
		{
			body.ApplyBuoyancyImpulse(m_SurfacePosition, m_SurfaceNormal, m_Buoyancy, m_LinearDrag, m_AngularDrag,
				ToJoltVec3(m_FluidVelocity), PhysicsSystem::Instance().GetGravity() * m_GravityFactor, Time::DeltaTime());
		}

		delete lock;
		lock = nullptr;
	}

	void FluidBody::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}

	void FluidBody::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			float3 position, scale;
			quat rotation;
			transform->DecomposeWorldMatrix(position, rotation, scale);

			float3 right = transform->Right();
			float3 up = transform->Up();
			float3 forward = transform->Forward();

			DebugRenderer::AddAABB(position + m_Center, m_Extents, float3(0.0f, 1.0f, 0.0f));
		}
	}
}