#pragma once
#include "Jolt.h"

namespace Odyssey
{
	namespace PhysicsLayers
	{
		static constexpr ObjectLayer Static = 0;
		static constexpr ObjectLayer Dynamic = 1;
		static constexpr ObjectLayer Count = 2;

	}

	enum class PhysicsLayer
	{
		Static = PhysicsLayers::Static,
		Dynamic = PhysicsLayers::Dynamic
	};

	namespace BroadPhaseLayers
	{
		static constexpr BroadPhaseLayer Static(0);
		static constexpr BroadPhaseLayer Dynamic(1);
		static constexpr uint32_t Count(2);
	}

	// Class that determines if two object layers can collide
	class ObjectLayerFilter : public ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
		{
			switch (inObject1)
			{
				case PhysicsLayers::Static: // Non moving only collides with moving
					return inObject2 == PhysicsLayers::Dynamic;
				case PhysicsLayers::Dynamic: // Moving collides with everything
					return true;
				default:
					JPH_ASSERT(false);
					return false;
			}
		}
	};

	// Class that determines if an object layer can collide with a broadphase layer
	class BroadPhaseLayerFilter : public ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
		{
			switch (inLayer1)
			{
				case PhysicsLayers::Static:
					return inLayer2 == BroadPhaseLayers::Dynamic;
				case PhysicsLayers::Dynamic:
					return true;
				default:
					JPH_ASSERT(false);
					return false;
			}
		}
	};

	// This defines a mapping between object and broadphase layers.
	class BroadPhaseLayerMap final : public BroadPhaseLayerInterface
	{
	public:
		BroadPhaseLayerMap()
		{
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[PhysicsLayers::Static] = BroadPhaseLayers::Static;
			mObjectToBroadPhase[PhysicsLayers::Dynamic] = BroadPhaseLayers::Dynamic;
		}

		virtual uint GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::Count;
		}

		virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
		{
			JPH_ASSERT(inLayer < PhysicsLayers::Count);
			return mObjectToBroadPhase[inLayer];
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
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		BroadPhaseLayer mObjectToBroadPhase[PhysicsLayers::Count];
	};
}