#pragma once
#include "PhysicsLayers.h"
#include "PhysicsListeners.h"
#include "RigidBody.h"
#include "Colliders.h"

namespace Odyssey
{
	class PhysicsSystem
	{
	public: // Singleton
		static void Init();
		static void Update();
		static void Destroy();
		static PhysicsSystem& Instance() { return *s_Instance; }

	private:
		PhysicsSystem();
		~PhysicsSystem();

	public:
		Body* RegisterBox(GameObject& gameObject, float3 position, quat rotation, float3 extents, BodyProperties& properties, PhysicsLayer layer);
		Body* RegisterSphere(GameObject& gameObject, float3 position, quat rotation, float radius, BodyProperties& properties, PhysicsLayer layer);
		Body* RegisterCapsule(GameObject& gameObject, float3 position, quat rotation, float radius, float height, BodyProperties& properties, PhysicsLayer layer);
		Ref<CharacterVirtual> RegisterCharacter(GameObject& gameObject, Ref<CharacterVirtualSettings>& settings);
		void Deregister(Body* body);
		void DeregisterCharacter(Ref<CharacterVirtual>& character);

	public:
		BodyInterface& GetBodyInterface();
		BodyProperties* GetBodyProperties(BodyID id);
		BodyLockWrite* GetBodyLock(BodyID bodyID);
		GameObject GetBodyGameObject(BodyID bodyID);
		GameObject GetCharacterGameObject(CharacterVirtual* character);
		Vec3 GetGravity();

	private:
		void FixedUpdate();
		Body* CreateBody(ShapeRefC shapeRef, float3 position, quat rotation, BodyProperties& properties, PhysicsLayer layer);

	private: // Singleton
		inline static PhysicsSystem* s_Instance = nullptr;

	private:
		std::map<BodyID, BodyProperties*> s_BodyProperties;
		std::map<BodyID, GameObject> s_BodyToGameObject;
		std::map<CharacterVirtual*, GameObject> s_CharacterToGameObject;
		CharacterPhysicsListener s_CharacterSolver;

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