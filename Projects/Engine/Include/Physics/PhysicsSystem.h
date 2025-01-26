#pragma once
#include "Jolt.h"
#include "PhysicsLayers.h"
#include "RigidBody.h"
#include "Colliders.h"
#include "ReadWriteLock.h"

namespace Odyssey
{
	enum class CollisionState
	{
		None = 0,
		Enter = 1,
		Stay = 2,
		Exit = 3,
	};

	struct CollisionData
	{
	public:
		CollisionData() = default;
		CollisionData(uint64_t id) : ID(id) { }

	public:
		uint64_t ID = 0;
		CollisionState State = CollisionState::None;;
		GameObject Body1;
		GameObject Body2;
		float3 ContactNormal = float3(0.0f);
	};

	class PhysicsSystem : public JPH::CharacterContactListener, public JPH::ContactListener
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
		GameObject GetCharacterGameObject(const CharacterVirtual* character);
		Vec3 GetGravity();

	public: // Character contact listeners
		// Called whenever the character collides with a body.
		virtual void OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

		// Callback to adjust the velocity of a body as seen by the character. Can be adjusted to e.g. implement a conveyor belt or an inertial dampener system of a sci-fi space ship.
		virtual void OnAdjustBodyVelocity(const CharacterVirtual* inCharacter, const Body& inBody2, Vec3& ioLinearVelocity, Vec3& ioAngularVelocity) override;

		// Called whenever the character collides with a virtual character.
		virtual void OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

		// Called whenever the character movement is solved and a constraint is hit. Allows the listener to override the resulting character velocity (e.g. by preventing sliding along certain surfaces).
		virtual void OnContactSolve(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, Vec3Arg inContactVelocity, const PhysicsMaterial* inContactMaterial, Vec3Arg inCharacterVelocity, Vec3& ioNewCharacterVelocity) override;

		virtual ValidateResult OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override;

		virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;

	private:
		void FixedUpdate();
		void PreProcessCollisionData();
		void ProcessCollisionData();

		Body* CreateBody(ShapeRefC shapeRef, float3 position, quat rotation, BodyProperties& properties, PhysicsLayer layer);

	private: // Singleton
		inline static PhysicsSystem* s_Instance = nullptr;

	private:
		std::map<BodyID, BodyProperties*> s_BodyProperties;
		std::map<BodyID, GameObject> s_BodyToGameObject;
		std::map<const CharacterVirtual*, GameObject> s_CharacterToGameObject;

	private: // Collision
		ReadWriteLock m_CollisionDataLock;
		std::unordered_map<uint64_t, CollisionData> m_CollisionData;

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