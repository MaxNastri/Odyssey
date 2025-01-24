#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"
#include "Jolt.h"

namespace Odyssey
{
	class CharacterController
	{
		CLASS_DECLARATION(Odyssey, CharacterController)
	public:
		CharacterController() = default;
		CharacterController(const GameObject& gameObject);
		CharacterController(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake();
		void Destroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetLinearVelocity(float3 velocity);
		float3 GetLinearVelocity();

	public:
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		void SetInertiaEnabled(bool enabled) { m_EnableInertia = enabled; }
		void SetDebugEnabled(bool enabled);
		void SetCenter(float3 center) { m_Center = center; }
		void SetRadius(float radius) { m_Radius = radius; }
		void SetHeight(float height) { m_Height = height; }

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsDebugEnabled() { return m_DebugEnabled; }
		bool IsInertiaEnabled() { return m_EnableInertia; }
		float3 GetCenter() { return m_Center; }
		float GetRadius() { return m_Radius; }
		float GetHeight() { return m_Height; }
		Ref<CharacterVirtual> GetCharacter() { return m_Character; }

	private:
		void CreateShape();
		void DebugDraw();

	private:
		bool m_Enabled = true;
		bool m_DebugEnabled = false;
		uint32_t m_DebugID;
		GameObject m_GameObject;
		Ref<CharacterVirtual> m_Character;

	public:
		float3 PrevFrameLinearVelocity = float3(0.0f);

	private:
		float3 m_Center = float3(0.0f);
		float m_Radius = 1.0f;
		float m_Height = 2.0f;
		bool m_EnableInertia = true;
		float m_MaxSlopeAngle = glm::radians(50.0f);
		float m_MaxStrength = 100.0f;
		float m_CharacterPadding = 0.02f;
		float m_PenetrationRecoverySpeed = 1.0f;
		float m_PredictiveContactDistance = 1.1f;
		bool m_CreateInnerBody = false;
		int m_BackFaceMode = 1;

	private:
		static constexpr float Inner_Shape_Fraction = 0.9f;
	};
}