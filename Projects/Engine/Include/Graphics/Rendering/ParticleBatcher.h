#pragma once
#include "ParticleSystem.h"
#include "Resource.h"

namespace Odyssey
{
	class VulkanPushDescriptors;

	class ParticleBatcher
	{
	public:
		static void Init();
		static void Shutdown();

	public:
		static void Update(const std::vector<ParticleSystem>& systems);

	public:
		static ResourceID GetStorageBuffer() { return m_StorageBuffer; }

	private:
		inline static constexpr size_t MAX_PARTICLES = 16384;
		inline static std::array<Particle, MAX_PARTICLES> m_Particles;
		inline static ResourceID m_StorageBuffer;
		inline static ResourceID m_StagingBuffer;
		inline static size_t m_BufferSize = 0;
	};
}