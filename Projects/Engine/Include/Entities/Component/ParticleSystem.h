#pragma once
#include "GameObject.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
	struct alignas(16) Particle
	{
		glm::vec4 Position = glm::vec4(0.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec4 Velocity = glm::vec4(0.0f, 0.1f, 0.0f, 0.0f);
		float Lifetime = 0.0f;
		float Size = 0.25f;
		float Speed = 0.25f;
	};

	struct ParticleEmitterData
	{
		float4 Position = glm::vec4(0,0,0,1);
		float4 Color = glm::vec4(1,0,0,1);
		float4 Velocity = glm::vec4(0,0.1f,0,0);
		float3 Rnd = glm::vec3(0.0f);
		float Lifetime = 3.0f;
		float Size = 1.0f;
		float Speed = 3.0f;
		uint32_t EmitCount = 1;
		uint32_t EmitterIndex = 0;
	};

	class ParticleSystem
	{
		CLASS_DECLARATION(Odyssey, ParticleSystem)
	public:
		ParticleSystem() = default;
		ParticleSystem(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		ParticleEmitterData& GetEmitterData() { return emitterData; }

	public:
		ParticleEmitterData emitterData;
		float Duration = 0.0f;
		// Particles spawned per sec
		uint32_t EmissionRate = 10;
		bool Loop = false;

	private:
		uint32_t m_ParticleCount;
		bool m_Playing;
	};
}