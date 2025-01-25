#pragma once
#include "PhysicsLayers.h"
#include "PhysicsListeners.h"
#include "RigidBody.h"
#include "Colliders.h"

namespace Odyssey
{
	class PhysicsSystem
	{
	public:
		static void Init();
		static void Update();
		static void Destroy();

		static Body* RegisterBox(float3 position, quat rotation, float3 extents, BodyProperties& properties, PhysicsLayer layer);
		static Body* RegisterSphere(float3 position, quat rotation, float radius, BodyProperties& properties, PhysicsLayer layer);
		static Body* RegisterCapsule(float3 position, quat rotation, float radius, float height, BodyProperties& properties, PhysicsLayer layer);
		static Ref<CharacterVirtual> RegisterCharacter(Ref<CharacterVirtualSettings>& settings);
		static void Deregister(Body* body);
		static void DeregisterCharacter(Ref<CharacterVirtual>& character);

	public:
		static BodyInterface& GetBodyInterface();
		static BodyProperties* GetBodyProperties(BodyID id);

	private:
		static Body* CreateBody(ShapeRefC shapeRef, float3 position, quat rotation, BodyProperties& properties, PhysicsLayer layer);

	private:
		inline static std::map<BodyID, BodyProperties*> s_BodyProperties;
		inline static CharacterPhysicsListener s_CharacterSolver;

	private:
		inline static JPH::PhysicsSystem m_PhysicsSystem;
		inline static JobSystemThreadPool* m_JobSystem;
		inline static TempAllocatorImpl* m_Allocator;

		// Create mapping table from object layer to broadphase layer
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler interface.
		inline static BroadPhaseLayerMap m_BroadPhaseLayerMap;

		// Create class that filters object vs broadphase layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler interface.
		inline static BroadPhaseLayerFilter m_BroadPhaseLayerFilter;

		// Create class that filters object vs object layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
		inline static ObjectLayerFilter m_PhysicsLayerFilter;
	};
}