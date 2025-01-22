#include "PhysicsSystem.h"
#include "SceneManager.h"

#include "Transform.h"
#include "RigidBody.h"
#include "OdysseyTime.h"

namespace Odyssey
{
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

		// A body activation listener gets notified when bodies activate and go to sleep
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		PhysicsBodyActivationListener body_activation_listener;
		//m_PhysicsSystem.SetBodyActivationListener(&body_activation_listener);

		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		PhysicsContactListener contact_listener;
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

	void PhysicsSystem::Init()
	{
		s_Instance = new PhysicsSystem();
	}

	void PhysicsSystem::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	BodyID PhysicsSystem::Register(float3 position, quat rotation, float3 extents, PhysicsLayer layer)
	{
		return s_Instance->RegisterBody(position, rotation, extents, layer);
	}

	void PhysicsSystem::Deregister(BodyID id)
	{
		s_Instance->DeregisterBody(id);
	}

	BodyInterface& PhysicsSystem::GetBodyInterface()
	{
		return s_Instance->GetPhysicsBodyInterface();
	}

	void PhysicsSystem::FixedUpdate()
	{
		// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
		const float cDeltaTime = 1.0f / 60.0f;

		// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
		const int cCollisionSteps = 1;

		// Broadphase results, will apply buoyancy to any body that intersects with the water volume
		// ADDS A COOL WATER FLOATING EFFECT
		class MyCollector : public CollideShapeBodyCollector
		{
		public:
			MyCollector(JPH::PhysicsSystem* inSystem, RVec3Arg inSurfacePosition, Vec3Arg inSurfaceNormal, float inDeltaTime) : mSystem(inSystem), mSurfacePosition(inSurfacePosition), mSurfaceNormal(inSurfaceNormal), mDeltaTime(inDeltaTime) { }

			virtual void			AddHit(const BodyID& inBodyID) override
			{
				BodyLockWrite lock(mSystem->GetBodyLockInterface(), inBodyID);
				Body& body = lock.GetBody();
				if (body.IsActive())
					body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, 1.9f, 0.75f, 0.05f, Vec3(0.2f, 0.0f, 0.0f), mSystem->GetGravity(), mDeltaTime);
			}

		private:
			JPH::PhysicsSystem* mSystem;
			RVec3					mSurfacePosition;
			Vec3					mSurfaceNormal;
			float					mDeltaTime;
		};

		MyCollector collector(&m_PhysicsSystem, Vec3(0, 1.0f, 0), Vec3::sAxisY(), Time::DeltaTime());

		// Apply buoyancy to all bodies that intersect with the water
		AABox water_box(-Vec3(100, 100, 100), Vec3(100, 0, 100));
		water_box.Translate(Vec3(Vec3(0, 1.0f, 0)));
		m_PhysicsSystem.GetBroadPhaseQuery().CollideAABox(water_box, collector, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::Dynamic), SpecifiedObjectLayerFilter(PhysicsLayers::Dynamic));

		// Step the world
		m_PhysicsSystem.Update(Time::DeltaTime(), cCollisionSteps, m_Allocator, m_JobSystem);

		// Writeback to the transform
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			if (activeScene->IsRunning())
			{
				auto view = activeScene->GetAllEntitiesWith<Transform, RigidBody>();
				for (auto& entity : view)
				{
					GameObject gameObject = GameObject(activeScene, entity);
					Transform& transform = gameObject.GetComponent<Transform>();
					RigidBody& rigidBody = gameObject.GetComponent<RigidBody>();

					// Get the post-physics simulation position and rotation in world space
					Vec3 simPosition;
					Quat simRotation;
					GetBodyInterface().GetPositionAndRotation(rigidBody.GetBodyID(), simPosition, simRotation);

					transform.SetPosition(ToFloat3(simPosition));
					transform.SetRotation(ToQuat(simRotation));
					transform.SetLocalSpace();
				}
			}
		}
	}

	BodyID PhysicsSystem::RegisterBody(float3 position, quat rotation, float3 extents, PhysicsLayer layer)
	{
		// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
		// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
		BodyInterface& body_interface = m_PhysicsSystem.GetBodyInterface();

		// Next we can create a rigid body to serve as the floor, we make a large box
		// Create the settings for the collision volume (the shape).
		// Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
		BoxShapeSettings floor_shape_settings(Vec3(extents.x, extents.y, extents.z));
		floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

		// Create the shape
		ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
		ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		EMotionType motion = layer == PhysicsLayer::Static ? EMotionType::Static : EMotionType::Dynamic;
		Quat rotationQ = Quat(rotation.x, rotation.y, rotation.z, rotation.w);
		BodyCreationSettings floor_settings(floor_shape, RVec3(position.x, position.y, position.z), rotationQ, motion, (uint32_t)layer);

		// Create the actual rigid body
		Body* floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

		// Add it to the world
		body_interface.AddBody(floor->GetID(), EActivation::Activate);

		//{
		//	// Now create a dynamic body to bounce on the floor
		//	// Note that this uses the shorthand version of creating and adding a body to the world
		//	BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0f, 2.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, PhysicsLayers::Dynamic);
		//	BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);
		//
		//	// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
		//	// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
		//	body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));
		//}

		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
		// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
		// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		m_PhysicsSystem.OptimizeBroadPhase();

		return floor->GetID();
	}

	void PhysicsSystem::DeregisterBody(BodyID id)
	{
		BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		bodyInterface.RemoveBody(id);
		bodyInterface.DestroyBody(id);
	}

	BodyInterface& PhysicsSystem::GetPhysicsBodyInterface()
	{
		return m_PhysicsSystem.GetBodyInterface();
	}

	void PhysicsSystem::Update()
	{
		s_Instance->FixedUpdate();
	}
}