#pragma once
#include "ParticleEmitter.h"
#include "Resource.h"
#include "GameObject.h"
#include "Shader.h"
#include "VulkanPushDescriptors.h"

namespace Odyssey
{
	class ParticleBatcher
	{
	public:
		static void Init();
		static void Shutdown();

	public:
		static void Update();

		static void RegisterEmitter(ParticleEmitter* emitter);
		static void DeregisterEmtter(ParticleEmitter* emitter);

	public:
		static const std::vector<size_t>& GetDrawList() { return s_DrawList; }
		static GUID GetMaterial(size_t index);
		static uint32_t GetAliveCount(size_t index);
		static ResourceID GetParticleBuffer(size_t index);
		static ResourceID GetAliveBuffer(size_t index);

	private:
		static void InitEmitResources();
		static void InitSimulationResources();
		static void SwapBuffers(size_t index);

	private:
		static void OnEmitShaderModified();
		static void OnSimShaderModified();

	private:
		inline static constexpr size_t MAX_PARTICLES = 16384;
		inline static constexpr size_t MAX_EMITTERS = 124;

		struct ParticleCounts
		{
			uint32_t DeadCount = 0;
			uint32_t AlivePreSimCount = 0;
			uint32_t AlivePostSimCount = 0;
			uint32_t TestCount = 0;
		};

		struct PerEmitterResources
		{
			// Data storage
			std::array<Particle, MAX_PARTICLES> ParticleData;
			std::array<uint32_t, MAX_PARTICLES> AlivePreSimList;
			std::array<uint32_t, MAX_PARTICLES> AlivePostSimList;
			std::array<uint32_t, MAX_PARTICLES> DeadList;
			ParticleCounts ParticleCounts;

			// Buffers
			ResourceID EmitterBuffer;
			ResourceID ParticleBuffer;
			ResourceID AlivePreSimBuffer;
			ResourceID AlivePostSimBuffer;
			ResourceID DeadBuffer;
			ResourceID CounterBuffer;

			// Material
			GUID Material;

			// Frame data
			uint32_t CurrentFrameAliveCount = 0;
		};

	private: // Emitter resources
		inline static std::map<GameObject, size_t> s_EntityToResourceIndex;
		inline static std::queue<size_t> s_ResourceIndices;
		inline static std::array<PerEmitterResources, MAX_EMITTERS> s_EmitterResources;
		inline static std::vector<size_t> s_DrawList;
		inline static uint8_t s_CurrentFrame = 0;

	private: // Shared
		inline static ResourceID s_CommandPool;
		inline static Ref<VulkanPushDescriptors> s_PushDescriptors;
		inline static Ref<VulkanPushDescriptors> s_SimPushDescriptors;

	private: // Emit pass
		inline static const GUID& s_EmitShaderGUID = 8940240242710108428;
		inline static ResourceID s_EmitDescriptorLayout;
		inline static ResourceID s_EmitComputePipeline;
		inline static Ref<Shader> s_EmitShader;

	private: // Simulation pass
		inline static const GUID& s_SimShaderGUID = 7831351134810913572;
		inline static ResourceID s_SimDescriptorLayout;
		inline static ResourceID s_SimComputePipeline;
		inline static Ref<Shader> s_SimShader;

	private: // Buffer sizes
		inline static size_t s_EmitterBufferSize;
		inline static size_t m_ParticleBufferSize = 0;
		inline static size_t m_CounterBufferSize = 0;
		inline static size_t m_ListBufferSize = 0;
	};
}