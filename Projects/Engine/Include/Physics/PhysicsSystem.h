#pragma once
#include "PhysicsLayers.h"
#include "PhysicsListeners.h"

namespace Odyssey
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

	public:
		static void Init();
		static void Update();
		static void Destroy();

		static BodyID Register(float3 position, float3 extents, PhysicsLayer layer);
		static void Deregister(BodyID id);
		static BodyInterface& GetBodyInterface();

	private:
		void FixedUpdate();
		BodyID RegisterBody(float3 position, float3 extents, PhysicsLayer layer);
		void DeregisterBody(BodyID id);
		BodyInterface& GetPhysicsBodyInterface();

	private:
		// Singleton
		inline static PhysicsSystem* s_Instance = nullptr;

	private:
		JPH::PhysicsSystem m_PhysicsSystem;
		JobSystemThreadPool* m_JobSystem;
		TempAllocatorImpl* m_Allocator;

		// Create mapping table from object layer to broadphase layer
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler interface.
		BroadPhaseLayerMap m_BroadPhaseLayerMap;

		// Create class that filters object vs broadphase layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler interface.
		BroadPhaseLayerFilter m_BroadPhaseLayerFilter;

		// Create class that filters object vs object layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
		ObjectLayerFilter m_PhysicsLayerFilter;
	};
}