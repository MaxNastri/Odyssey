#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"
#include "PhysicsSystem.h"

namespace Odyssey
{
	class FluidBody : public CollideShapeBodyCollector
	{
		CLASS_DECLARATION(Odyssey, FluidBody)
	public:
		FluidBody() = default;
		FluidBody(const GameObject& gameObject);
		FluidBody(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake();
		void Update();
		void OnDestroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void CheckCollision(const BroadPhaseQuery& bpQuery);
		virtual void AddHit(const BodyID& inBodyID) override;

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsDebugEnabled() { return m_DebugEnabled; }
		float3 GetCenter() { return m_Center; }
		float3 GetExtents() { return m_Extents; }
		float GetBuoyancy() { return m_Buoyancy; }
		float GetLinearDrag() { return m_LinearDrag; }
		float GetAngularDrag() { return m_AngularDrag; }
		float3 GetFluidVelocity() { return m_FluidVelocity; }
		float GetGravityFactor() { return m_GravityFactor; }

	public:
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		void SetCenter(float3 center) { m_Center = center; }
		void SetExtents(float3 extents) { m_Extents = extents; }
		void SetBuoyancy(float buoyancy) { m_Buoyancy = buoyancy; }
		void SetLinearDrag(float linearDrag) { m_LinearDrag = linearDrag; }
		void SetAngularDrag(float angularDrag) { m_AngularDrag = angularDrag; }
		void SetFluidVelocity(float3 velocity) { m_FluidVelocity = velocity; }
		void SetGravityFactor(float gravityFactor) { m_GravityFactor = gravityFactor; }
		void SetDebugEnabled(bool enabled);

	private:
		void DebugDraw();

	private:
		bool m_Enabled = true;
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
		GameObject m_GameObject;

	private:
		float3 m_Center = float3(0.0f);
		float3 m_Extents = float3(1.0f);
		float m_Buoyancy = 1.0f;
		float m_LinearDrag = 0.5f;
		float m_AngularDrag = 0.01f;
		float3 m_FluidVelocity = float3(0.0f);
		float m_GravityFactor = 1.0f;

	private:
		Vec3 m_SurfacePosition = Vec3::sZero();
		Vec3 m_SurfaceNormal = Vec3::sAxisY();
	};
}