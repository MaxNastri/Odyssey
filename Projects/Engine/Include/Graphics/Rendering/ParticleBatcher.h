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
		static void SwapBuffers();

	public:
		static ResourceID GetParticleBuffer() { return m_ParticleBuffer; }
		static ResourceID GetCounterBuffer() { return m_CounterBuffer; }
		static ResourceID GetAlivePreSimBuffer() { return m_AlivePreSimBuffer; }
		static ResourceID GetAlivePostSimBuffer() { return m_AlivePostSimBuffer; }
		static ResourceID GetDeadBuffer() { return m_DeadBuffer; }
		static uint32_t AliveCount() { return s_ParticleCounts.AlivePreSimCount; }

	private:
		struct ParticleCounts
		{
			uint32_t DeadCount = 0;
			uint32_t AlivePreSimCount = 0;
			uint32_t AlivePostSimCount = 0;
			uint32_t TestCount = 0;
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
		inline static size_t m_ListBufferSize = 0;

		inline static std::array<uint32_t, MAX_PARTICLES> m_AlivePreSimList;
		inline static ResourceID m_AlivePreSimBuffer;
		
		inline static std::array<uint32_t, MAX_PARTICLES> m_AlivePostSimList;
		inline static ResourceID m_AlivePostSimBuffer;

		inline static std::array<uint32_t, MAX_PARTICLES> m_DeadList;
		inline static ResourceID m_DeadBuffer;

	private:
		inline static ResourceID s_EmitterBuffer;
		inline static size_t s_EmitterBufferSize;

	private:
		inline static bool s_Run = false;
		// Shader
		// Compute pipeline
		// Push descriptors
		// Descriptor layout
		// Some way to get compute command buffer

	};
}