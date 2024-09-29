#pragma once
#include "GameObject.h"

namespace Odyssey
{
	struct alignas(16) Particle
	{
		glm::vec4 Position = glm::vec4(0.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec4 Velocity = glm::vec4(0.0f);
		float Lifetime = 0.0f;
		float Size = 1.0f;
		float SizeOverLifetime = 0.0f;
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
		float Duration = 0.0f;
		// Particles spawned per sec
		uint32_t EmissionRate = 0;
		bool Loop = false;

	private:
		uint32_t m_ParticleCount;
		bool m_Playing;
	};
}