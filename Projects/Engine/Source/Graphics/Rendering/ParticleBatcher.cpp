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
#include "SceneManager.h"
#include "Scene.h"
#include "Random.h"

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
		s_PushDescriptors = std::make_shared<VulkanPushDescriptors>();

		s_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(s_DescriptorLayout);
		descriptorLayout->AddBinding("Particle Buffer", DescriptorType::Storage, ShaderStage::Compute, 2);
		descriptorLayout->AddBinding("Counter Buffer", DescriptorType::Storage, ShaderStage::Compute, 3);
		descriptorLayout->AddBinding("Alive Pre-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 4);
		descriptorLayout->AddBinding("Alive Post-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 5);
		descriptorLayout->AddBinding("Dead Buffer", DescriptorType::Storage, ShaderStage::Compute, 6);
		descriptorLayout->AddBinding("Emitter Data", DescriptorType::Uniform, ShaderStage::Compute, 7);
		descriptorLayout->Apply();

		VulkanPipelineInfo info;
		info.Shaders = s_EmitShader->GetResourceMap();
		info.DescriptorLayout = s_DescriptorLayout;
		info.BindVertexAttributeDescriptions = false;

		s_ComputePipeline = ResourceManager::Allocate<VulkanComputePipeline>(info);

		m_AlivePreSimBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_AlivePostSimBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_DeadBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_ParticleBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ParticleBufferSize);
		m_CounterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_CounterBufferSize);
		s_EmitterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, s_EmitterBufferSize);

		for (size_t i = 0; i < 100; i++)
		{
			m_Particles[i].Position = glm::vec4(i * 2, 0.0f, 0.0f, 1.0f);
			m_Particles[i].Velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}


		for (size_t i = 0; i < m_DeadList.size(); i++)
		{
			m_DeadList[i] = (uint32_t)i;
		}

		s_ParticleCounts.AlivePreSimCount = 0;
		s_ParticleCounts.AlivePostSimCount = 0;
		s_ParticleCounts.DeadCount = MAX_PARTICLES;

		auto alivePreSimBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AlivePreSimBuffer);
		alivePreSimBuffer->UploadData(m_AlivePreSimList.data(), m_ListBufferSize);

		auto alivePostSimBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AlivePostSimBuffer);
		alivePostSimBuffer->UploadData(m_AlivePostSimList.data(), m_ListBufferSize);

		auto deadBuffer = ResourceManager::GetResource<VulkanBuffer>(m_DeadBuffer);
		deadBuffer->UploadData(m_DeadList.data(), m_ListBufferSize);

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

	void ParticleBatcher::Update()
	{
		Scene* scene = SceneManager::GetActiveScene();
		
		auto emitterEntities = scene->GetAllEntitiesWith<ParticleEmitter>();
		if (emitterEntities.size() == 0)
		{
			s_Run = false;
			return;
		}

		s_Run = true;
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(s_CommandPool);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();
		commandBuffer->BindComputePipeline(s_ComputePipeline);

		bool run = false;

		for (auto& entity : emitterEntities)
		{
			GameObject gameObject = GameObject(scene, entity);
			ParticleEmitter& emitter = gameObject.GetComponent<ParticleEmitter>();

			emitter.Update(Time::DeltaTime());
			auto emitterBuffer = ResourceManager::GetResource<VulkanBuffer>(s_EmitterBuffer);
			auto& emitterData = emitter.GetEmitterData();
			emitterData.Rnd.x = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.y = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.z = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.w = Random::Float01();
			emitterBuffer->CopyData(s_EmitterBufferSize, &emitterData);

			s_PushDescriptors->Clear();
			s_PushDescriptors->AddBuffer(m_ParticleBuffer, 2);
			s_PushDescriptors->AddBuffer(m_CounterBuffer, 3);
			s_PushDescriptors->AddBuffer(m_AlivePreSimBuffer, 4);
			s_PushDescriptors->AddBuffer(m_AlivePostSimBuffer, 5);
			s_PushDescriptors->AddBuffer(m_DeadBuffer, 6);
			s_PushDescriptors->AddBuffer(s_EmitterBuffer, 7);

			commandBuffer->PushDescriptorsCompute(s_PushDescriptors.get(), s_ComputePipeline);
			commandBuffer->Dispatch((uint32_t)std::ceil((float)emitterData.EmitCount / 64.0f), 1, 1);
		}

		// End commands
		commandBuffer->EndCommands();
		commandBuffer->SubmitCompute();
		commandPool->ReleaseBuffer(commandBufferID);

		auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(ParticleBatcher::GetCounterBuffer());
		counterBuffer->CopyBufferMemory(&s_ParticleCounts);
	}

	void ParticleBatcher::SwapBuffers()
	{
		if (s_Run)
		{
			auto alivePreSimBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AlivePreSimBuffer);
			alivePreSimBuffer->CopyBufferMemory(m_AlivePreSimList.data());

			auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(ParticleBatcher::GetCounterBuffer());
			counterBuffer->CopyBufferMemory(&s_ParticleCounts);

			auto alivePostSimBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AlivePostSimBuffer);
			alivePostSimBuffer->CopyBufferMemory(m_AlivePostSimList.data());

			std::swap(s_ParticleCounts.AlivePreSimCount, s_ParticleCounts.AlivePostSimCount);
			std::swap(m_AlivePreSimList, m_AlivePostSimList);

			s_ParticleCounts.AlivePostSimCount = 0;
			counterBuffer->UploadData(&s_ParticleCounts, m_CounterBufferSize);
			alivePreSimBuffer->UploadData(m_AlivePreSimList.data(), m_ListBufferSize);
			alivePostSimBuffer->UploadData(m_AlivePostSimList.data(), m_ListBufferSize);
		}
	}
}