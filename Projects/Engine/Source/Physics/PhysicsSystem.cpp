#include "PhysicsSystem.h"
#include "SceneManager.h"

#include "Transform.h"
#include "RigidBody.h"
#include "OdysseyTime.h"
#include "Colliders.h"
#include "FluidBody.h"
#include "CharacterController.h"

namespace Odyssey
{
	void PhysicsSystem::Init()
	{
		s_Instance = new PhysicsSystem();
	}

	void PhysicsSystem::Update()
	{
		s_Instance->FixedUpdate();
	}

	void PhysicsSystem::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	PhysicsSystem::PhysicsSystem()
	{
		RegisterDefaultAllocator();
		Factory::sInstance = new Factory();
		RegisterTypes();
		m_Allocator = new TempAllocatorImpl(10 * 1024 * 1024);

		// We need a job system that will execute physics jobs on multiple threads. Typically
		// you would implement the JobSystem interface yourself and let Jolt Physics run on top
		// of your own job scheduler. JobSystemThreadPool is an example implementation.
		m_JobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

		// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
		const uint cMaxBodies = 65536;

		// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
		const uint cNumBodyMutexes = 0;

		// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
		// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
		// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
		const uint cMaxBodyPairs = 65536;

		// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
		// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
		const uint cMaxContactConstraints = 10240;

		// Now we can create the actual physics system.
		m_PhysicsSystem;
		m_PhysicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, m_BroadPhaseLayerMap, m_BroadPhaseLayerFilter, m_PhysicsLayerFilter);
		m_PhysicsSystem.SetContactListener(this);

		// A body activation listener gets notified when bodies activate and go to sleep
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		//PhysicsBodyActivationListener body_activation_listener;
		//m_PhysicsSystem.SetBodyActivationListener(&body_activation_listener);

		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		//PhysicsContactListener contact_listener;
		//m_PhysicsSystem.SetContactListener(&contact_listener);
	}

	PhysicsSystem::~PhysicsSystem()
	{
		// Unregisters all types with the factory and cleans up the default material
		UnregisterTypes();

		// Destroy the factory
		delete Factory::sInstance;
		Factory::sInstance = nullptr;

		delete m_Allocator;
		m_Allocator = nullptr;

		delete m_JobSystem;
		m_JobSystem = nullptr;
	}

	Body* PhysicsSystem::RegisterBox(GameObject& gameObject, float3 position, quat rotation, float3 extents, BodyProperties& properties, PhysicsLayer layer)
	{
		// Create the box shape settings
		ShapeRefC shapeRef = BoxShapeSettings(ToJoltVec3(extents)).Create().Get();

		// Create and track the body
		Body* body = CreateBody(shapeRef, position, rotation, properties, layer);
		s_BodyToGameObject[body->GetID()] = gameObject;

		return body;
	}

	Body* PhysicsSystem::RegisterSphere(GameObject& gameObject, float3 position, quat rotation, float radius, BodyProperties& properties, PhysicsLayer layer)
	{
		// Create the sphere shape
		ShapeRefC shapeRef = SphereShapeSettings(radius).Create().Get();

		// Create and track the body
		Body* body = CreateBody(shapeRef, position, rotation, properties, layer);
		s_BodyToGameObject[body->GetID()] = gameObject;

		return body;
	}

	Body* PhysicsSystem::RegisterCapsule(GameObject& gameObject, float3 position, quat rotation, float radius, float height, BodyProperties& properties, PhysicsLayer layer)
	{
		// Create the capsule shape
		ShapeRefC shapeRef = CapsuleShapeSettings(height * 0.5f, radius).Create().Get();

		// Create and track the body
		Body* body = CreateBody(shapeRef, position, rotation, properties, layer);
		s_BodyToGameObject[body->GetID()] = gameObject;

		return body;
	}

	void PhysicsSystem::Deregister(Body* body)
	{
		if (s_BodyProperties.contains(body->GetID()))
			s_BodyProperties.erase(body->GetID());

		if (s_BodyToGameObject.contains(body->GetID()))
			s_BodyToGameObject.erase(body->GetID());

		BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		bodyInterface.RemoveBody(body->GetID());
		bodyInterface.DestroyBody(body->GetID());
	}

	BodyInterface& PhysicsSystem::GetBodyInterface()
	{
		return m_PhysicsSystem.GetBodyInterface();
	}

	BodyProperties* PhysicsSystem::GetBodyProperties(BodyID id)
	{
		if (s_BodyProperties.contains(id))
			return s_BodyProperties[id];

		return nullptr;
	}

	BodyLockWrite* PhysicsSystem::GetBodyLock(BodyID bodyID)
	{
		return new BodyLockWrite(m_PhysicsSystem.GetBodyLockInterface(), bodyID);
	}

	GameObject PhysicsSystem::GetBodyGameObject(BodyID bodyID)
	{
		if (s_BodyToGameObject.contains(bodyID))
			return s_BodyToGameObject[bodyID];

		return GameObject();
	}

	GameObject PhysicsSystem::GetCharacterGameObject(const CharacterVirtual* character)
	{
		if (s_CharacterToGameObject.contains(character))
			return s_CharacterToGameObject[character];

		return GameObject();
	}

	Vec3 PhysicsSystem::GetGravity()
	{
		return m_PhysicsSystem.GetGravity();
	}

	void PhysicsSystem::OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
	{
		if (BodyProperties* properties = GetBodyProperties(inBodyID2))
		{
			ioSettings.mCanPushCharacter = properties->PushCharacter;
			ioSettings.mCanReceiveImpulses = properties->ReceiveForce;
		}
	}

	void PhysicsSystem::OnAdjustBodyVelocity(const CharacterVirtual* inCharacter, const Body& inBody2, Vec3& ioLinearVelocity, Vec3& ioAngularVelocity)
	{
		// Apply the surface velocity to the character
		if (BodyProperties* properties = GetBodyProperties(inBody2.GetID()))
			ioLinearVelocity += ToJoltVec3(properties->SurfaceVelocity);
	}

	void PhysicsSystem::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
	{

	}

	void PhysicsSystem::OnContactSolve(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, Vec3Arg inContactVelocity, const PhysicsMaterial* inContactMaterial, Vec3Arg inCharacterVelocity, Vec3& ioNewCharacterVelocity)
	{
		m_CollisionDataLock.Lock(LockState::Write);
		GameObject body1 = GetCharacterGameObject(inCharacter);
		GameObject body2 = GetBodyGameObject(inBodyID2);
		uint64_t collisionID = HashCombine(body1.GetGUID(), body2.GetGUID());

		if (!m_CollisionData.contains(collisionID))
			m_CollisionData[collisionID] = CollisionData(collisionID);

		CollisionData& data = m_CollisionData[collisionID];
		if (data.State == CollisionState::Enter || data.State == CollisionState::Exit)
			data.State = CollisionState::Stay;
		else
			data.State = CollisionState::Enter;

		data.Body1 = body1;
		data.Body2 = body2;
		data.ContactNormal = ToFloat3(inContactNormal);
		m_CollisionDataLock.Unlock(LockState::Write);
	}

	ValidateResult PhysicsSystem::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
	{
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void PhysicsSystem::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
	{
		m_CollisionDataLock.Lock(LockState::Write);
		GameObject body1 = GetBodyGameObject(inBody1.GetID());
		GameObject body2 = GetBodyGameObject(inBody2.GetID());
		uint64_t collisionID = HashCombine(body1.GetGUID(), body2.GetGUID());

		if (!m_CollisionData.contains(collisionID))
			m_CollisionData[collisionID] = CollisionData(collisionID);

		CollisionData& data = m_CollisionData[collisionID];
		if (data.State == CollisionState::Enter || data.State == CollisionState::Exit)
			data.State = CollisionState::Stay;
		else
			data.State = CollisionState::Enter;

		data.Body1 = body1;
		data.Body2 = body2;
		data.ContactNormal = ToFloat3(inManifold.mWorldSpaceNormal);
		m_CollisionDataLock.Unlock(LockState::Write);
	}

	Ref<CharacterVirtual> PhysicsSystem::RegisterCharacter(GameObject& gameObject, Ref<CharacterVirtualSettings>& settings)
	{
		Ref<CharacterVirtual> character = new CharacterVirtual(settings.Get(), RVec3::sZero(), Quat::sIdentity(), 0, &m_PhysicsSystem);
		character->SetListener(this);

		s_CharacterToGameObject[character.Get()] = gameObject;
		return character;
	}

	void PhysicsSystem::DeregisterCharacter(Ref<CharacterVirtual>& character)
	{
		character->SetListener(nullptr);
		if (s_CharacterToGameObject.contains(character.Get()))
			s_CharacterToGameObject.erase(character.Get());
	}

	Body* PhysicsSystem::CreateBody(ShapeRefC shapeRef, float3 position, quat rotation, BodyProperties& properties, PhysicsLayer layer)
	{
		BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		EMotionType motion = EMotionType::Static;

		if (properties.Kinematic)
			motion = EMotionType::Kinematic;
		else if (layer == PhysicsLayer::Dynamic)
			motion = EMotionType::Dynamic;

		BodyCreationSettings bodySettings(shapeRef, ToJoltVec3(position), ToJoltQuat(rotation), motion, (uint32_t)layer);

		// Apply our body properties to the creation settings
		if (properties.Mass > 0.0f)
		{
			bodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
			bodySettings.mMassPropertiesOverride.mMass = properties.Mass;
		}

		bodySettings.mMaxLinearVelocity = properties.MaxLinearVelocity;
		bodySettings.mFriction = properties.Friction;
		bodySettings.mGravityFactor = properties.GravityFactor;

		// Create the actual rigid body
		Body* body = bodyInterface.CreateBody(bodySettings); // Note that if we run out of bodies this can return nullptr

		// Add it to the world
		bodyInterface.AddBody(body->GetID(), EActivation::Activate);

		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
		// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
		// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		m_PhysicsSystem.OptimizeBroadPhase();

		s_BodyProperties[body->GetID()] = &properties;
		return body;
	}

	void PhysicsSystem::FixedUpdate()
	{
		// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
		const float cDeltaTime = 1.0f / 60.0f;

		// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
		const int cCollisionSteps = 1;

		// Read from the transform
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			if (activeScene->IsRunning())
			{
				PreProcessCollisionData();

				auto fluidView = activeScene->GetAllEntitiesWith<Transform, FluidBody>();
				for (auto& entity : fluidView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					FluidBody& fluidBody = gameObject.GetComponent<FluidBody>();
					fluidBody.CheckCollision(m_PhysicsSystem.GetBroadPhaseQuery());
				}

				auto characterView = activeScene->GetAllEntitiesWith<Transform, CharacterController>();

				for (auto& entity : characterView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					CharacterController& controller = gameObject.GetComponent<CharacterController>();
					Ref<CharacterVirtual> character = controller.GetCharacter();
					float3 position, scale;
					quat rotation;
					transform.DecomposeWorldMatrix(position, rotation, scale);

					character->SetRotation(ToJoltQuat(rotation));
					character->SetPosition(ToJoltVec3(position));
					controller.UpdateVelocity(m_PhysicsSystem.GetGravity(), Time::DeltaTime());

					CharacterVirtual::ExtendedUpdateSettings updateSettings;
					updateSettings.mStickToFloorStepDown = -character->GetUp() * controller.GetStepDown();
					updateSettings.mWalkStairsStepUp = character->GetUp() * controller.GetStepUp();

					controller.GetCharacter()->ExtendedUpdate(Time::DeltaTime(),
						m_PhysicsSystem.GetGravity(),
						updateSettings,
						m_PhysicsSystem.GetDefaultBroadPhaseLayerFilter(PhysicsLayers::Dynamic),
						m_PhysicsSystem.GetDefaultLayerFilter(PhysicsLayers::Dynamic),
						{ },
						{ },
						*m_Allocator);
				}

				auto view = activeScene->GetAllEntitiesWith<Transform, RigidBody, BoxCollider>();
				for (auto& entity : view)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					BoxCollider& collider = gameObject.GetComponent<BoxCollider>();

					if (rigidBody.GetLayer() == PhysicsLayer::Dynamic)
					{
						float3 position, scale;
						quat rotation;
						transform.DecomposeWorldMatrix(position, rotation, scale);

						GetBodyInterface().SetPositionAndRotation(rigidBody.GetBodyID(), ToJoltVec3(position) + ToJoltVec3(collider.GetCenter()), ToJoltQuat(rotation), EActivation::Activate);
					}
				}

				auto sphereView = activeScene->GetAllEntitiesWith<Transform, RigidBody, SphereCollider>();
				for (auto& entity : sphereView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					SphereCollider& collider = gameObject.GetComponent<SphereCollider>();

					if (rigidBody.GetLayer() == PhysicsLayer::Dynamic)
					{
						float3 position, scale;
						quat rotation;
						transform.DecomposeWorldMatrix(position, rotation, scale);

						GetBodyInterface().SetPositionAndRotation(rigidBody.GetBodyID(), ToJoltVec3(position) + ToJoltVec3(collider.GetCenter()), ToJoltQuat(rotation), EActivation::Activate);
					}
				}

				auto capsuleView = activeScene->GetAllEntitiesWith<Transform, RigidBody, CapsuleCollider>();
				for (auto& entity : capsuleView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					CapsuleCollider& collider = gameObject.GetComponent<CapsuleCollider>();

					if (rigidBody.GetLayer() == PhysicsLayer::Dynamic)
					{
						float3 position, scale;
						quat rotation;
						transform.DecomposeWorldMatrix(position, rotation, scale);

						GetBodyInterface().SetPositionAndRotation(rigidBody.GetBodyID(), ToJoltVec3(position) + ToJoltVec3(collider.GetCenter()), ToJoltQuat(rotation), EActivation::Activate);
					}
				}
			}
		}

		// Step the world
		m_PhysicsSystem.Update(Time::DeltaTime(), 2, m_Allocator, m_JobSystem);

		// Writeback to the transform
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			if (activeScene->IsRunning())
			{
				auto characterView = activeScene->GetAllEntitiesWith<Transform, CharacterController>();
				for (auto& entity : characterView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					CharacterController& controller = gameObject.GetComponent<CharacterController>();

					float3 position = ToFloat3(controller.GetCharacter()->GetPosition());
					controller.ResetVelocity();
					transform.SetPosition(position);
					transform.SetLocalSpace();
				}

				auto view = activeScene->GetAllEntitiesWith<Transform, RigidBody, BoxCollider>();
				for (auto& entity : view)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					BoxCollider& collider = gameObject.GetComponent<BoxCollider>();

					// Get the post-physics simulation position and rotation in world space
					Vec3 simPosition;
					Quat simRotation;
					GetBodyInterface().GetPositionAndRotation(rigidBody.GetBodyID(), simPosition, simRotation);

					transform.SetPosition(ToFloat3(simPosition) - collider.GetCenter());
					transform.SetRotation(ToQuat(simRotation));
					transform.SetLocalSpace();
				}

				auto sphereView = activeScene->GetAllEntitiesWith<Transform, RigidBody, SphereCollider>();
				for (auto& entity : sphereView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					SphereCollider& collider = gameObject.GetComponent<SphereCollider>();

					// Get the post-physics simulation position and rotation in world space
					Vec3 simPosition;
					Quat simRotation;
					GetBodyInterface().GetPositionAndRotation(rigidBody.GetBodyID(), simPosition, simRotation);

					transform.SetPosition(ToFloat3(simPosition) - collider.GetCenter());
					transform.SetRotation(ToQuat(simRotation));
					transform.SetLocalSpace();
				}

				auto capsuleView = activeScene->GetAllEntitiesWith<Transform, RigidBody, CapsuleCollider>();
				for (auto& entity : capsuleView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					CapsuleCollider& collider = gameObject.GetComponent<CapsuleCollider>();

					// Get the post-physics simulation position and rotation in world space
					Vec3 simPosition;
					Quat simRotation;
					GetBodyInterface().GetPositionAndRotation(rigidBody.GetBodyID(), simPosition, simRotation);

					transform.SetPosition(ToFloat3(simPosition) - collider.GetCenter());
					transform.SetRotation(ToQuat(simRotation));
					transform.SetLocalSpace();
				}

				ProcessCollisionData();
			}
		}
	}

	void PhysicsSystem::PreProcessCollisionData()
	{
		m_CollisionDataLock.Lock(LockState::Read);

		std::vector<uint64_t> removals;
		for (auto& [collisionID, data] : m_CollisionData)
		{
			// Move collision enter or stay to exit
			// If there is a continued collision, this will flip back to stay on next physics processing
			if (data.State == CollisionState::Enter || data.State == CollisionState::Stay)
				data.State = CollisionState::Exit;
			else if (data.State == CollisionState::Exit)
				removals.push_back(collisionID);
		}

		// Remove the stale collision data
		for (size_t i = 0; i < removals.size(); i++)
			m_CollisionData.erase(removals[i]);

		m_CollisionDataLock.Unlock(LockState::Read);
	}

	void PhysicsSystem::ProcessCollisionData()
	{
		m_CollisionDataLock.Lock(LockState::Read);

		for (auto& [collisionID, data] : m_CollisionData)
		{
			switch (data.State)
			{
				case CollisionState::Enter:
					data.Body1.OnCollisionEnter(data.Body2, data.ContactNormal);
					data.Body2.OnCollisionEnter(data.Body1, data.ContactNormal);
					break;
				case CollisionState::Stay:
					data.Body1.OnCollisionStay(data.Body2, data.ContactNormal);
					data.Body2.OnCollisionStay(data.Body1, data.ContactNormal);
					break;
				case CollisionState::Exit:
					data.Body1.OnCollisionExit(data.Body2);
					data.Body2.OnCollisionExit(data.Body1);
					break;
			}
		}

		m_CollisionDataLock.Unlock(LockState::Read);
	}
}