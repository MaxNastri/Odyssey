#include "CharacterController.h"
#include "DebugRenderer.h"
#include "Transform.h"
#include "PhysicsLayers.h"
#include "PhysicsSystem.h"

namespace Odyssey
{
	CharacterController::CharacterController(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	CharacterController::CharacterController(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void CharacterController::Awake()
	{
		CreateShape();
	}

	void CharacterController::OnDestroy()
	{
		if (m_Character)
			PhysicsSystem::DeregisterCharacter(m_Character);

		m_Character.Reset();
		SetDebugEnabled(false);
	}

	void CharacterController::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", CharacterController::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Radius", m_Mass);
		componentNode.WriteData("Radius", m_Radius);
		componentNode.WriteData("Height", m_Height);
		componentNode.WriteData("Inertia Enabled", m_EnableInertia);
		componentNode.WriteData("Max Slope", m_MaxSlopeAngle);
		componentNode.WriteData("Step Up", m_StepUp);
		componentNode.WriteData("Step Down", m_StepDown);
		componentNode.WriteData("Max Strength", m_MaxStrength);
		componentNode.WriteData("Character Padding", m_CharacterPadding);
		componentNode.WriteData("Create Inner Body", m_CreateInnerBody);
	}

	void CharacterController::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Center", m_Center);
		node.ReadData("Mass", m_Mass);
		node.ReadData("Radius", m_Radius);
		node.ReadData("Height", m_Height);
		node.ReadData("Inertia Enabled", m_EnableInertia);
		node.ReadData("Max Slope", m_MaxSlopeAngle);
		node.ReadData("Step Up", m_StepUp);
		node.ReadData("Step Down", m_StepDown);
		node.ReadData("Max Strength", m_MaxStrength);
		node.ReadData("Character Padding", m_CharacterPadding);
		node.ReadData("Create Inner Body", m_CreateInnerBody);
	}

	void CharacterController::UpdateVelocity(Vec3 gravity, float dt)
	{
		Vec3 prevFrameVelocity = ToJoltVec3(m_PrevFrameLinearVelocity);
		Vec3 lastVerticalVelocity = prevFrameVelocity.Dot(m_Character->GetUp()) * m_Character->GetUp();

		// What the user input for velocity
		Vec3 inputVelocity = m_Character->GetLinearVelocity();
		Vec3 totalVelocity = Vec3::sZero();

		// Velocity of the ground
		m_Character->UpdateGroundVelocity();
		Vec3 groundVelocity = m_Character->GetGroundVelocity();

		if (m_Character->GetGroundState() == CharacterVirtual::EGroundState::OnGround	// If on ground
			&& !m_Character->IsSlopeTooSteep(m_Character->GetGroundNormal()))			// Inertia disabled: And not on a slope that is too steep
		{
			// Assume velocity of ground when on ground
			totalVelocity = groundVelocity;
		}
		else
			totalVelocity = lastVerticalVelocity;

		// Apply gravity
		totalVelocity += gravity * dt;

		// Apply user input velocity
		totalVelocity += inputVelocity;

		// Set the new combined velocity
		m_Character->SetLinearVelocity(totalVelocity);
		m_PrevFrameLinearVelocity = ToFloat3(totalVelocity);
	}

	void CharacterController::ResetVelocity()
	{
		m_PrevFrameLinearVelocity = ToFloat3(m_Character->GetLinearVelocity());
		m_Character->SetLinearVelocity(Vec3::sZero());
	}

	void CharacterController::SetLinearVelocity(float3 velocity)
	{
		m_Character->SetLinearVelocity(ToJoltVec3(velocity));
	}

	float3 CharacterController::GetLinearVelocity()
	{
		return ToFloat3(m_Character->GetLinearVelocity());
	}

	void CharacterController::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}

	bool CharacterController::IsGrounded()
	{
		return m_Character->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround;
	}

	void CharacterController::CreateShape()
	{
		Vec3 position = ToJoltVec3(float3(m_Center.x, 0.5f * m_Height + m_Radius, m_Center.z));

		ShapeRefC shape = RotatedTranslatedShapeSettings(position, Quat::sIdentity(),
			new CapsuleShape(m_Height * 0.5f, m_Radius)).Create().Get();
		ShapeRefC innerShape = RotatedTranslatedShapeSettings(position, Quat::sIdentity(),
			new CapsuleShape(m_Height * 0.5f * Inner_Shape_Fraction, m_Radius * Inner_Shape_Fraction)).Create().Get();

		Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();
		settings->mMaxSlopeAngle = m_MaxSlopeAngle;
		settings->mMaxStrength = m_MaxStrength;
		settings->mMass = m_Mass;
		settings->mShape = shape;
		settings->mBackFaceMode = (EBackFaceMode)m_BackFaceMode;
		settings->mCharacterPadding = m_CharacterPadding;
		settings->mPenetrationRecoverySpeed = m_PenetrationRecoverySpeed;
		settings->mPredictiveContactDistance = m_PredictiveContactDistance;
		settings->mSupportingVolume = Plane(Vec3::sAxisY(), -m_Radius);
		settings->mEnhancedInternalEdgeRemoval = false;
		settings->mInnerBodyShape = innerShape;
		settings->mInnerBodyLayer = PhysicsLayers::Dynamic;
		m_Character = PhysicsSystem::RegisterCharacter(m_GameObject, settings);
	}

	void CharacterController::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			DebugRenderer::AddCapsule(transform->GetWorldPosition() + m_Center, m_Radius, m_Height * 0.5f, float3(0.0f, 1.0f, 0.0f));
	}
}