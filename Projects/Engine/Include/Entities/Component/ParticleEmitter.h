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
		void SetLifetime(float2 lifetime) { emitterData.Lifetime = lifetime; }
		void SetSize(float2 size) { emitterData.Size = size; }
		void SetSpeed(float2 speed) { emitterData.Speed = speed; }
		void SetColor(float3 color) { emitterData.Color = float4(color, 1.0f); }

	public:
		float3 GetColor() { return emitterData.Color; }
		float2 GetLifetime() { return emitterData.Lifetime; }
		float2 GetSize() { return emitterData.Size; }
		float2 GetSpeed() { return emitterData.Speed; }

	public:
		ParticleEmitterData emitterData;
		float Duration = 1.0f;
		// Particles spawned per sec
		uint32_t EmissionRate = 100;
		bool Loop = false;

	private:
		float m_EmissionTime;
		uint32_t m_EmissionCount;
	private:
		GameObject m_GameObject;
		uint32_t m_ParticleCount;
		bool m_Playing;
	};
}