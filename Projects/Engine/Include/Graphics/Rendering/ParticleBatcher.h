#pragma once
#include "ParticleSystem.h"
#include "Resource.h"

namespace Odyssey
{
	class Shader;
	class VulkanPushDescriptors;

	class ParticleBatcher
	{
	public:
		static void Init();
		static void Shutdown();

	public:
		static void Update(std::vector<ParticleSystem>& systems);

	public:
		static ResourceID GetParticleBuffer() { return m_ParticleBuffer; }
		static ResourceID GetCounterBuffer() { return m_CounterBuffer; }
		static ResourceID GetAliveBuffer() { return m_AliveBuffer; }
		static uint32_t AliveCount() { return s_ParticleCounts.AliveCount; }
	private:
		struct ParticleCounts
		{
			uint32_t AliveCount = 0;
			uint32_t DeadCount = 0;
		};

	private:
		inline static const GUID& s_EmitShaderGUID = 8940240242710108428;
		inline static ResourceID s_DescriptorLayout;
		inline static ResourceID s_ComputePipeline;
		inline static std::shared_ptr<Shader> s_EmitShader;
		inline static ResourceID s_CommandPool;
		inline static std::shared_ptr<VulkanPushDescriptors> s_PushDescriptors;

	private:
		inline static constexpr size_t MAX_PARTICLES = 16384;
		inline static std::array<Particle, MAX_PARTICLES> m_Particles;
		inline static ResourceID m_ParticleBuffer;
		inline static size_t m_ParticleBufferSize = 0;

	private:
		inline static ParticleCounts s_ParticleCounts;
		inline static ResourceID m_CounterBuffer;
		inline static size_t m_CounterBufferSize = 0;

	private:
		inline static ResourceID m_AliveBuffer;
		inline static ResourceID m_DeadBuffer;
		inline static size_t m_ListBufferSize = 0;

	private:
		inline static ResourceID s_EmitterBuffer;
		inline static size_t s_EmitterBufferSize;

		// Shader
		// Compute pipeline
		// Push descriptors
		// Descriptor layout
		// Some way to get compute command buffer

	};
}