#pragma once
#include "Jolt.h"

namespace Odyssey
{
	// An example contact listener
	class PhysicsContactListener : public ContactListener
	{
	public:
		// See: ContactListener
		virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
		{
			Log::Info("Contact validate callback invoked.");

			// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
			return ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
		{
			Log::Info("Physics contact added.");
		}

		virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
		{
			Log::Info("Physics contact persisted.");
		}

		virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
		{
			Log::Info("Physics contact removed.");
		}
	};

	// An example activation listener
	class PhysicsBodyActivationListener : public BodyActivationListener
	{
	public:
		virtual void OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
		{
			Log::Info("A body was activated.");
		}

		virtual void OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
		{
			Log::Info("A body was deactivated.");
		}
	};
}