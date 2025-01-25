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

	class CharacterPhysicsListener : public JPH::CharacterContactListener
	{
	public:
		CharacterPhysicsListener() = default;

	public:
		// Called whenever the character collides with a body.
		virtual void OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

		// Callback to adjust the velocity of a body as seen by the character. Can be adjusted to e.g. implement a conveyor belt or an inertial dampener system of a sci-fi space ship.
		virtual void OnAdjustBodyVelocity(const CharacterVirtual* inCharacter, const Body& inBody2, Vec3& ioLinearVelocity, Vec3& ioAngularVelocity) override;

		// Called whenever the character collides with a virtual character.
		virtual void OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

		// Called whenever the character movement is solved and a constraint is hit. Allows the listener to override the resulting character velocity (e.g. by preventing sliding along certain surfaces).
		virtual void OnContactSolve(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, Vec3Arg inContactVelocity, const PhysicsMaterial* inContactMaterial, Vec3Arg inCharacterVelocity, Vec3& ioNewCharacterVelocity) override;
	};
}