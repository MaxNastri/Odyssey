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

	void CharacterController::Destroy()
	{
		PhysicsSystem::DeregisterCharacter(m_Character);
		m_Character.Reset();
	}

	void CharacterController::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", CharacterController::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Radius", m_Radius);
		componentNode.WriteData("Height", m_Height);
	}

	void CharacterController::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Center", m_Center);
		node.ReadData("Radius", m_Radius);
		node.ReadData("Height", m_Height);
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

	void CharacterController::CreateShape()
	{
		Vec3 position = Vec3(0.0f, m_Height * 0.5f + m_Radius, 0.0f);

		ShapeRefC shape = RotatedTranslatedShapeSettings(position, Quat::sIdentity(),
			new CapsuleShape(m_Height * 0.5f, m_Radius)).Create().Get();
		ShapeRefC innerShape = RotatedTranslatedShapeSettings(position, Quat::sIdentity(),
			new CapsuleShape(m_Height * 0.5f * Inner_Shape_Fraction, m_Radius * Inner_Shape_Fraction)).Create().Get();

		Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();
		settings->mMaxSlopeAngle = m_MaxSlopeAngle;
		settings->mMaxStrength = m_MaxStrength;
		settings->mShape = shape;
		settings->mBackFaceMode = (EBackFaceMode)m_BackFaceMode;
		settings->mCharacterPadding = m_CharacterPadding;
		settings->mPenetrationRecoverySpeed = m_PenetrationRecoverySpeed;
		settings->mPredictiveContactDistance = m_PredictiveContactDistance;
		settings->mSupportingVolume = Plane(Vec3::sAxisY(), -m_Radius);
		settings->mEnhancedInternalEdgeRemoval = false;
		//settings->mInnerBodyShape = innerShape;
		//settings->mInnerBodyLayer = PhysicsLayers::Dynamic;
		m_Character = PhysicsSystem::RegisterCharacter(settings);
	}

	void CharacterController::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			DebugRenderer::AddCapsule(transform->GetWorldPosition() + m_Center, m_Radius, m_Height * 0.5f, float3(0.0f, 1.0f, 0.0f));
	}
}