#pragma once
#include "GameObject.h"

namespace Odyssey
{
	struct Particle
	{
		glm::vec4 Position;
		glm::vec4 Color;
		glm::vec4 Velocity;
		float Lifetime;
		float Size;
		float SizeOverLifetime;
	};

	struct ParticleData
	{
		std::vector<Particle> Particles;
		uint32_t ParticleCount = 0;
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