#pragma once
#include "GameObject.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
	struct alignas(16) Particle
	{
		glm::vec4 Position;
		glm::vec4 Color;
		glm::vec4 Velocity;
		float Lifetime;
		float Size;
		float Speed;
	};

	struct ParticleEmitterData
	{
		float4 Position = glm::vec4(0,0,0,1);
		float4 Color = glm::vec4(1,0,0,1);
		float4 Velocity = glm::vec4(0,0.1f,0,0);
		float4 Rnd = float4(0.0f);
		float2 Lifetime = float2(1.0f, 5.0f);
		float2 Size = float2(0.25f, 1.0f);
		float2 Speed = float2(0.25f, 1.0f);
		uint32_t EmitCount = 1;
		uint32_t EmitterIndex = 0;
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
		ParticleEmitterData& GetEmitterData();
		void Update(float deltaTime);

	public:
		void SetDuration(float duration) { m_Duration = duration; }
		void SetLifetime(float2 lifetime) { emitterData.Lifetime = lifetime; }
		void SetMaterial(GUID material) { m_Material = material; }
		void SetSize(float2 size) { emitterData.Size = size; }
		void SetSpeed(float2 speed) { emitterData.Speed = speed; }
		void SetColor(float4 color) { emitterData.Color = color; }

	public:
		float GetDuration() { return m_Duration; }
		float4 GetColor() { return emitterData.Color; }
		float2 GetLifetime() { return emitterData.Lifetime; }
		GUID GetMaterial() { return m_Material; }
		float2 GetSize() { return emitterData.Size; }
		float2 GetSpeed() { return emitterData.Speed; }

	public:
		ParticleEmitterData emitterData;
		uint32_t EmissionRate = 100;
		bool Loop = false;

	private:
		float m_Duration = 1.0f;
		GUID m_Material;

	private:
		float m_EmissionTime;
		uint32_t m_EmissionCount;
	private:
		GameObject m_GameObject;
		uint32_t m_ParticleCount;
		bool m_Playing;
	};
}