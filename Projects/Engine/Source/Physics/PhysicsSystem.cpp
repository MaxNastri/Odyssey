#include "PhysicsSystem.h"
#include "SceneManager.h"

#include "Transform.h"
#include "RigidBody.h"
#include "OdysseyTime.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

namespace Odyssey
{
	void PhysicsSystem::Init()
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

	void PhysicsSystem::Destroy()
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

	Body* PhysicsSystem::RegisterBox(float3 position, quat rotation, float3 extents, BodyProperties& properties, PhysicsLayer layer)
	{
		BodyInterface& body_interface = m_PhysicsSystem.GetBodyInterface();

		// Create the box shape settings
		BoxShapeSettings shapeSettings(ToJoltVec3(extents));
		shapeSettings.SetEmbedded();

		// Create the shape
		ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
		ShapeRefC floor_shape = shapeResult.Get();

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		EMotionType motion = layer == PhysicsLayer::Static ? EMotionType::Static : EMotionType::Dynamic;
		BodyCreationSettings bodySettings(floor_shape, ToJoltVec3(position), ToJoltQuat(rotation), motion, (uint32_t)layer);

		// Apply our body properties to the creation settings
		bodySettings.mMaxLinearVelocity = properties.MaxLinearVelocity;
		bodySettings.mFriction = properties.Friction;

		// Create the actual rigid body
		Body* body = body_interface.CreateBody(bodySettings); // Note that if we run out of bodies this can return nullptr

		// Add it to the world
		body_interface.AddBody(body->GetID(), EActivation::Activate);

		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
		// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
		// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		m_PhysicsSystem.OptimizeBroadPhase();

		return body;
	}

	Body* PhysicsSystem::RegisterSphere(float3 position, quat rotation, float radius, BodyProperties& properties, PhysicsLayer layer)
	{
		BodyInterface& body_interface = m_PhysicsSystem.GetBodyInterface();

		// Create the box shape settings
		SphereShapeSettings shapeSettings(radius);
		shapeSettings.SetEmbedded();

		// Create the shape
		ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
		ShapeRefC shapeRef = shapeResult.Get();

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		EMotionType motion = layer == PhysicsLayer::Static ? EMotionType::Static : EMotionType::Dynamic;
		BodyCreationSettings bodySettings(shapeRef, ToJoltVec3(position), ToJoltQuat(rotation), motion, (uint32_t)layer);

		// Apply our body properties to the creation settings
		bodySettings.mMaxLinearVelocity = properties.MaxLinearVelocity;
		bodySettings.mFriction = properties.Friction;

		// Create the actual rigid body
		Body* body = body_interface.CreateBody(bodySettings); // Note that if we run out of bodies this can return nullptr

		// Add it to the world
		body_interface.AddBody(body->GetID(), EActivation::Activate);

		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
		// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
		// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		m_PhysicsSystem.OptimizeBroadPhase();

		return body;
	}

	void PhysicsSystem::Deregister(Body* body)
	{
		BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		bodyInterface.RemoveBody(body->GetID());
		bodyInterface.DestroyBody(body->GetID());
	}

	BodyInterface& PhysicsSystem::GetBodyInterface()
	{
		return m_PhysicsSystem.GetBodyInterface();
	}

	void PhysicsSystem::Update()
	{
		// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
		const float cDeltaTime = 1.0f / 60.0f;

		// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
		const int cCollisionSteps = 1;

		// Broadphase results, will apply buoyancy to any body that intersects with the water volume
		// ADDS A COOL WATER FLOATING EFFECT
		//class MyCollector : public CollideShapeBodyCollector
		//{
		//public:
		//	MyCollector(JPH::PhysicsSystem* inSystem, RVec3Arg inSurfacePosition, Vec3Arg inSurfaceNormal, float inDeltaTime) : mSystem(inSystem), mSurfacePosition(inSurfacePosition), mSurfaceNormal(inSurfaceNormal), mDeltaTime(inDeltaTime) { }

		//	virtual void			AddHit(const BodyID& inBodyID) override
		//	{
		//		BodyLockWrite lock(mSystem->GetBodyLockInterface(), inBodyID);
		//		Body& body = lock.GetBody();
		//		if (body.IsActive())
		//			body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, 1.9f, 0.75f, 0.05f, Vec3(0.2f, 0.0f, 0.0f), mSystem->GetGravity(), mDeltaTime);
		//	}

		//private:
		//	JPH::PhysicsSystem* mSystem;
		//	RVec3					mSurfacePosition;
		//	Vec3					mSurfaceNormal;
		//	float					mDeltaTime;
		//};

		//MyCollector collector(&m_PhysicsSystem, Vec3(0, 1.0f, 0), Vec3::sAxisY(), Time::DeltaTime());

		//// Apply buoyancy to all bodies that intersect with the water
		//AABox water_box(-Vec3(100, 100, 100), Vec3(100, 0, 100));
		//water_box.Translate(Vec3(Vec3(0, 1.0f, 0)));
		//m_PhysicsSystem.GetBroadPhaseQuery().CollideAABox(water_box, collector, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::Dynamic), SpecifiedObjectLayerFilter(PhysicsLayers::Dynamic));


		// Read from the transform
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			if (activeScene->IsRunning())
			{
				auto view = activeScene->GetAllEntitiesWith<Transform, RigidBody, BoxCollider>();
				for (auto& entity : view)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					BoxCollider& boxCollider = gameObject.GetComponent<BoxCollider>();

					if (rigidBody.GetLayer() == PhysicsLayer::Dynamic)
					{
						float3 position, scale;
						quat rotation;
						transform.DecomposeWorldMatrix(position, rotation, scale);

						GetBodyInterface().SetPositionAndRotation(rigidBody.GetBodyID(), ToJoltVec3(position) + ToJoltVec3(boxCollider.GetCenter()), ToJoltQuat(rotation), EActivation::Activate);
					}
				}

				auto sphereView = activeScene->GetAllEntitiesWith<Transform, RigidBody, SphereCollider>();
				for (auto& entity : sphereView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					SphereCollider& sphereCollider = gameObject.GetComponent<SphereCollider>();

					if (rigidBody.GetLayer() == PhysicsLayer::Dynamic)
					{
						float3 position, scale;
						quat rotation;
						transform.DecomposeWorldMatrix(position, rotation, scale);

						GetBodyInterface().SetPositionAndRotation(rigidBody.GetBodyID(), ToJoltVec3(position) + ToJoltVec3(sphereCollider.GetCenter()), ToJoltQuat(rotation), EActivation::Activate);
					}
				}
			}
		}

		// Step the world
		m_PhysicsSystem.Update(Time::DeltaTime(), cCollisionSteps, m_Allocator, m_JobSystem);

		// Writeback to the transform
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			if (activeScene->IsRunning())
			{
				auto view = activeScene->GetAllEntitiesWith<Transform, RigidBody, BoxCollider>();
				for (auto& entity : view)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					BoxCollider& boxCollider = gameObject.GetComponent<BoxCollider>();

					// Get the post-physics simulation position and rotation in world space
					Vec3 simPosition;
					Quat simRotation;
					GetBodyInterface().GetPositionAndRotation(rigidBody.GetBodyID(), simPosition, simRotation);

					transform.SetPosition(ToFloat3(simPosition) - boxCollider.GetCenter());
					transform.SetRotation(ToQuat(simRotation));
					transform.SetLocalSpace();
				}

				auto sphereView = activeScene->GetAllEntitiesWith<Transform, RigidBody, SphereCollider>();
				for (auto& entity : sphereView)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();
					SphereCollider& sphereCollider = gameObject.GetComponent<SphereCollider>();

					// Get the post-physics simulation position and rotation in world space
					Vec3 simPosition;
					Quat simRotation;
					GetBodyInterface().GetPositionAndRotation(rigidBody.GetBodyID(), simPosition, simRotation);

					transform.SetPosition(ToFloat3(simPosition) - sphereCollider.GetCenter());
					transform.SetRotation(ToQuat(simRotation));
					transform.SetLocalSpace();
				}
			}
		}
	}
}