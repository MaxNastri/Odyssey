#pragma once
#include "ParticleEmitter.h"
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
		static void Update();

	private:
		static void InitEmitResources();
		static void InitSimulationResources();

		static void SwapBuffers();

	public:
		static ResourceID GetParticleBuffer() { return m_ParticleBuffer; }
		static ResourceID GetCounterBuffer() { return m_CounterBuffer; }
		static ResourceID GetAlivePreSimBuffer() { return m_AlivePreSimBuffer; }
		static ResourceID GetAlivePostSimBuffer() { return m_AlivePostSimBuffer; }
		static ResourceID GetDeadBuffer() { return m_DeadBuffer; }
		static uint32_t AliveCount() { return m_CurrentFrameAlive; }

	private:
		struct ParticleCounts
		{
			uint32_t DeadCount = 0;
			uint32_t AlivePreSimCount = 0;
			uint32_t AlivePostSimCount = 0;
			uint32_t TestCount = 0;
		};

	private: // Shared
		inline static ResourceID s_CommandPool;
		inline static std::shared_ptr<VulkanPushDescriptors> s_PushDescriptors;
		inline static std::shared_ptr<VulkanPushDescriptors> s_SimPushDescriptors;

	private: // Emit pass
		inline static const GUID& s_EmitShaderGUID = 8940240242710108428;
		inline static ResourceID s_EmitDescriptorLayout;
		inline static ResourceID s_EmitComputePipeline;
		inline static std::shared_ptr<Shader> s_EmitShader;
		inline static ResourceID s_EmitterBuffer;
		inline static size_t s_EmitterBufferSize;

	private: // Simulation pass
		inline static const GUID& s_SimShaderGUID = 7831351134810913572;
		inline static ResourceID s_SimDescriptorLayout;
		inline static ResourceID s_SimComputePipeline;
		inline static std::shared_ptr<Shader> s_SimShader;

	private:
		inline static constexpr size_t MAX_PARTICLES = 16384;
		inline static std::array<Particle, MAX_PARTICLES> m_Particles;
		inline static ResourceID m_ParticleBuffer;
		inline static size_t m_ParticleBufferSize = 0;

	private:
		inline static ParticleCounts s_ParticleCounts;
		inline static ResourceID m_CounterBuffer;
		inline static size_t m_CounterBufferSize = 0;
		inline static uint32_t m_CurrentFrameAlive = 0;

	private:
		inline static size_t m_ListBufferSize = 0;

		inline static std::array<uint32_t, MAX_PARTICLES> m_AlivePreSimList;
		inline static ResourceID m_AlivePreSimBuffer;
		
		inline static std::array<uint32_t, MAX_PARTICLES> m_AlivePostSimList;
		inline static ResourceID m_AlivePostSimBuffer;

		inline static std::array<uint32_t, MAX_PARTICLES> m_DeadList;
		inline static ResourceID m_DeadBuffer;

	private:
		inline static bool s_SwapBuffers = false;
	};
}