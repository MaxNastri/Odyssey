#include "ParticleBatcher.h"
#include "ResourceManager.h"
#include "AssetManager.h"
#include "Shader.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffer.h"
#include "VulkanPushDescriptors.h"
#include "VulkanComputePipeline.h"
#include "VulkanDescriptorLayout.h"
#include "OdysseyTime.h"

namespace Odyssey
{
	void ParticleBatcher::Init()
	{
		m_ParticleBufferSize = sizeof(Particle) * MAX_PARTICLES;
		m_ListBufferSize = sizeof(uint32_t) * MAX_PARTICLES;
		m_CounterBufferSize = sizeof(ParticleCounts);
		s_EmitterBufferSize = sizeof(ParticleEmitterData);

		s_EmitShader = AssetManager::LoadAsset<Shader>(s_EmitShaderGUID);
		s_CommandPool = ResourceManager::Allocate<VulkanCommandPool>(VulkanQueueType::Compute);

		s_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(s_DescriptorLayout);
		descriptorLayout->AddBinding("Alive Buffer", DescriptorType::Storage, ShaderStage::Compute, 0);
		descriptorLayout->AddBinding("Dead Buffer", DescriptorType::Storage, ShaderStage::Compute, 1);
		descriptorLayout->AddBinding("Particle Buffer", DescriptorType::Storage, ShaderStage::Compute, 2);
		descriptorLayout->AddBinding("Counter Buffer", DescriptorType::Storage, ShaderStage::Compute, 3);
		descriptorLayout->AddBinding("Emitter Data", DescriptorType::Uniform, ShaderStage::Compute, 4);
		descriptorLayout->Apply();

		VulkanPipelineInfo info;
		info.Shaders = s_EmitShader->GetResourceMap();
		info.DescriptorLayout = s_DescriptorLayout;
		info.BindVertexAttributeDescriptions = false;

		s_ComputePipeline = ResourceManager::Allocate<VulkanComputePipeline>(info);

		m_AliveBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_DeadBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_ParticleBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ParticleBufferSize);
		m_CounterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_CounterBufferSize);
		s_EmitterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, s_EmitterBufferSize);

		for (size_t i = 0; i < 100; i++)
		{
			m_Particles[i].Position = glm::vec4(i * 2, 0.0f, 0.0f, 1.0f);
			m_Particles[i].Velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		std::array<uint32_t, MAX_PARTICLES> deadList;

		for (size_t i = 0; i < deadList.size(); i++)
		{
			deadList[i] = (uint32_t)i;
		}

		s_ParticleCounts.AliveCount = 0;
		s_ParticleCounts.DeadCount = MAX_PARTICLES;

		auto aliveBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AliveBuffer);
		aliveBuffer->UploadData(deadList.data(), m_ListBufferSize);

		auto deadBuffer = ResourceManager::GetResource<VulkanBuffer>(m_DeadBuffer);
		deadBuffer->UploadData(deadList.data(), m_ListBufferSize);

		auto particleBuffer = ResourceManager::GetResource<VulkanBuffer>(m_ParticleBuffer);
		particleBuffer->UploadData(m_Particles.data(), m_ParticleBufferSize);

		auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(m_CounterBuffer);
		counterBuffer->UploadData(&s_ParticleCounts, m_CounterBufferSize);
	}

	void ParticleBatcher::Shutdown()
	{
		if (m_ParticleBuffer)
			ResourceManager::Destroy(m_ParticleBuffer);
	}

	void ParticleBatcher::Update(std::vector<ParticleSystem>& systems)
	{
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(s_CommandPool);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();
		commandBuffer->BindComputePipeline(s_ComputePipeline);

		for (auto& particleSystem : systems)
		{
			uint32_t spawn = (uint32_t)(std::floor(particleSystem.EmissionRate * Time::DeltaTime()));
			auto emitterBuffer = ResourceManager::GetResource<VulkanBuffer>(s_EmitterBuffer);
			emitterBuffer->CopyData(s_EmitterBufferSize, &particleSystem.GetEmitterData());

			s_PushDescriptors->Clear();
			s_PushDescriptors->AddBuffer(m_AliveBuffer, 0);
			s_PushDescriptors->AddBuffer(m_DeadBuffer, 1);
			s_PushDescriptors->AddBuffer(m_ParticleBuffer, 2);
			s_PushDescriptors->AddBuffer(m_CounterBuffer, 3);
			s_PushDescriptors->AddBuffer(s_EmitterBuffer, 4);

			commandBuffer->PushDescriptorsCompute(s_PushDescriptors.get(), s_ComputePipeline);
			commandBuffer->Dispatch(spawn, 1, 1);
		}

		// End commands
		commandBuffer->EndCommands();
		commandBuffer->SubmitCompute();
		commandPool->ReleaseBuffer(commandBufferID);

		auto aliveBuffer = ResourceManager::GetResource<VulkanBuffer>(ParticleBatcher::GetCounterBuffer());
		aliveBuffer->CopyBufferMemory(&s_ParticleCounts);
	}
}