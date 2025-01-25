#include "PhysicsListeners.h"
#include "PhysicsSystem.h"

namespace Odyssey
{
	void CharacterPhysicsListener::OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
	{
		if (BodyProperties* properties = PhysicsSystem::Instance().GetBodyProperties(inBodyID2))
		{
			ioSettings.mCanPushCharacter = properties->PushCharacter;
			ioSettings.mCanReceiveImpulses = properties->ReceiveForce;
		}
	}

	void CharacterPhysicsListener::OnAdjustBodyVelocity(const CharacterVirtual* inCharacter, const Body& inBody2, Vec3& ioLinearVelocity, Vec3& ioAngularVelocity)
	{
		// Apply the surface velocity to the character
		if (BodyProperties* properties = PhysicsSystem::Instance().GetBodyProperties(inBody2.GetID()))
			ioLinearVelocity += ToJoltVec3(properties->SurfaceVelocity);
	}

	void CharacterPhysicsListener::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
	{

	}

	void CharacterPhysicsListener::OnContactSolve(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, Vec3Arg inContactVelocity, const PhysicsMaterial* inContactMaterial, Vec3Arg inCharacterVelocity, Vec3& ioNewCharacterVelocity)
	{

	}
}