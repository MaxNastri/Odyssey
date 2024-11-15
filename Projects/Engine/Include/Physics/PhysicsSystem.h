#pragma once
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace Odyssey
{
	namespace PhysicsLayers
	{
		static constexpr ObjectLayer Static = 0;
		static constexpr ObjectLayer Dynamic = 1;
		static constexpr ObjectLayer Count = 2;
	}

	class PhysicsLayerFilter : public ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollider(ObjectLayer obj1, ObjectLayer obj2)
		{
			switch (obj1)
			{
				case PhysicsLayers::Static:
					return obj2 == PhysicsLayers::Dynamic;
				case PhysicsLayers::Dynamic:
					return true;
				default:
					assert(false);
					return false;
			}
		}
	};

	namespace BroadPhaseLayers
	{
		static constexpr BroadPhaseLayer Static(0);
		static constexpr BroadPhaseLayer Dynamic(1);
		static constexpr uint32_t Count(2);
	}

	// Defines a mapping between object and broadphase layers
	class BroadPhaseLayerMap final : public BroadPhaseLayerInterface
	{
	public:
		BroadPhaseLayerMap()
		{
			m_ObjectToBroadPhase[PhysicsLayers::Static] = BroadPhaseLayers::Static;
			m_ObjectToBroadPhase[PhysicsLayers::Dynamic] = BroadPhaseLayers::Dynamic;
		}

		virtual uint32_t GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::Count;
		}

		virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer objLayer) const override
		{
			assert(objLayer < PhysicsLayers::Count);
			return m_ObjectToBroadPhase[objLayer];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
		{
			switch ((BroadPhaseLayer::Type)inLayer)
			{
				case (BroadPhaseLayer::Type)BroadPhaseLayers::Static:
					return "Static";
				case (BroadPhaseLayer::Type)BroadPhaseLayers::Dynamic:
					return "Dynamic";
				default:
					JPH_ASSERT(false);
					return "INVALID";
			}
		}
#endif
	private:
		BroadPhaseLayer m_ObjectToBroadPhase[PhysicsLayers::Count];
	};

	class PhysicsSystem
	{

	};
}