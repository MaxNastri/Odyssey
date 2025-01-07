#pragma once
#include "GameObject.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
	struct alignas(16) Particle
	{
		float4 Position;
		float4 Color;
		float4 Velocity;
		float2 Lifetime;
		float Size;
		float Speed;
	};

	enum class EmitterShape : uint32_t
	{
		Circle = 0,
		Cone = 1,
	};
	struct ParticleEmitterData
	{
		float4 Position = glm::vec4(0,0,0,1);
		float4 StartColor = glm::vec4(1,0,0,1);
		float4 EndColor = glm::vec4(1,0,0,1);
		float4 Velocity = glm::vec4(0,0.1f,0,0);
		float2 Lifetime = float2(1.0f, 5.0f);
		float2 Size = float2(0.25f, 1.0f);
		float2 Speed = float2(0.25f, 1.0f);
		uint32_t EmitCount = 1;
		uint32_t EmitterIndex = 0;
		uint32_t FrameIndex = 0;
		uint32_t Shape = 0;
		float Radius = 1.0f;
		float Angle = 0.0f;
	};

	class ParticleEmitter
	{
		CLASS_DECLARATION(Odyssey, ParticleEmitter)
	public:
		ParticleEmitter() = default;
		ParticleEmitter(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		GameObject& GetGameObject() { return m_GameObject; }

	public:
		ParticleEmitterData& GetEmitterData();
		void Update(float deltaTime);

	public:
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		void SetLooping(bool looping) { m_Looping = looping; }
		void SetEmissionRate(uint32_t emissionRate) { m_EmissionRate = emissionRate; }
		void SetRadius(float radius) { emitterData.Radius = radius; }
		void SetAngle(float angle) { emitterData.Angle = angle; }
		void SetDuration(float duration) { m_Duration = duration; }
		void SetLifetime(float2 lifetime) { emitterData.Lifetime = lifetime; }
		void SetMaterial(GUID material) { m_Material = material; }
		void SetSize(float2 size) { emitterData.Size = size; }
		void SetSpeed(float2 speed) { emitterData.Speed = speed; }
		void SetStartColor(float4 color) { emitterData.StartColor = color; }
		void SetEndColor(float4 color) { emitterData.EndColor = color; }

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsActive() { return m_Active; }
		float GetDuration() { return m_Duration; }
		bool IsLooping() { return m_Looping; }
		uint32_t GetEmissionRate() { return m_EmissionRate; }
		float4 GetStartColor() { return emitterData.StartColor; }
		float4 GetEndColor() { return emitterData.EndColor; }
		float2 GetLifetime() { return emitterData.Lifetime; }
		GUID GetMaterial() { return m_Material; }
		float2 GetSize() { return emitterData.Size; }
		float2 GetSpeed() { return emitterData.Speed; }
		float GetRadius() { return emitterData.Radius; }
		float GetAngle() { return emitterData.Angle; }

	public:
		ParticleEmitterData emitterData;

	private:
		bool m_Enabled;
		GameObject m_GameObject;
		bool m_Looping = true;
		float m_Duration = 1.0f;
		uint32_t m_EmissionRate = 100;
		GUID m_Material;

	private:
		bool m_Active = true;
		float m_ActiveTime = 0.0f;
		float m_EmissionTime = 0.0f;
		uint32_t m_EmissionCount;
		uint32_t m_ParticleCount;
	};
}