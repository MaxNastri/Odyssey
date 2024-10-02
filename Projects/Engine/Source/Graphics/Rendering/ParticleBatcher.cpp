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
		// We can share these resources between Emit and Sim passes
		s_CommandPool = ResourceManager::Allocate<VulkanCommandPool>(VulkanQueueType::Compute);
		s_PushDescriptors = std::make_shared<VulkanPushDescriptors>();

		// Calculate the size of the shared buffers
		m_ParticleBufferSize = sizeof(Particle) * MAX_PARTICLES;
		m_ListBufferSize = sizeof(uint32_t) * MAX_PARTICLES;
		m_CounterBufferSize = sizeof(ParticleCounts);

		// Create the shared buffers (Alive, Dead, Particle Counter)
		m_AlivePreSimBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_AlivePostSimBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_DeadBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
		m_ParticleBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ParticleBufferSize);
		m_CounterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_CounterBufferSize);

		// Initialize all particle indices to be dead by default
		for (size_t i = 0; i < m_DeadList.size(); i++)
		{
			m_DeadList[i] = (uint32_t)i;
		}

		// Initialize the alive counts to 0 and dead count to all particles
		s_ParticleCounts.AlivePreSimCount = 0;
		s_ParticleCounts.AlivePostSimCount = 0;
		s_ParticleCounts.DeadCount = MAX_PARTICLES;

		// Update all buffers with initial data
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

		// Initialize the emit and simulation resources
		InitEmitResources();
		InitSimulationResources();
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

		// Don't run without any emitters
		if (emitterEntities.size() == 0)
			return;

		// Allocate a compute command buffer from the pool
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(s_CommandPool);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Start the emit compute pass
		commandBuffer->BeginCommands();
		commandBuffer->BindComputePipeline(s_EmitComputePipeline);

		for (auto& entity : emitterEntities)
		{
			// Get the particle emitter
			GameObject gameObject = GameObject(scene, entity);
			ParticleEmitter& emitter = gameObject.GetComponent<ParticleEmitter>();

			// Update the emitter and generate some randomness
			emitter.Update(Time::DeltaTime());
			auto& emitterData = emitter.GetEmitterData();
			emitterData.Rnd.x = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.y = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.z = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.w = Random::Float01();

			// Update the emitter buffer
			auto emitterBuffer = ResourceManager::GetResource<VulkanBuffer>(s_EmitterBuffer);
			emitterBuffer->CopyData(s_EmitterBufferSize, &emitterData);

			// Push the buffer descriptors
			s_PushDescriptors->Clear();
			s_PushDescriptors->AddBuffer(m_ParticleBuffer, 2);
			s_PushDescriptors->AddBuffer(m_CounterBuffer, 3);
			s_PushDescriptors->AddBuffer(m_AlivePreSimBuffer, 4);
			s_PushDescriptors->AddBuffer(m_AlivePostSimBuffer, 5);
			s_PushDescriptors->AddBuffer(m_DeadBuffer, 6);
			s_PushDescriptors->AddBuffer(s_EmitterBuffer, 7);

			// Push the descriptors and dispatch
			uint32_t groups = (uint32_t)(std::ceil(emitterData.EmitCount / 64.0f));
			commandBuffer->PushDescriptorsCompute(s_PushDescriptors.get(), s_EmitComputePipeline);
			commandBuffer->Dispatch(groups, 1, 1);
		}

		// End commands and release
		commandBuffer->EndCommands();
		commandBuffer->SubmitCompute();
		commandPool->ReleaseBuffer(commandBufferID);

		// Update the particle counts from the gpu
		auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(ParticleBatcher::GetCounterBuffer());
		counterBuffer->CopyBufferMemory(&s_ParticleCounts);

		if (s_ParticleCounts.AlivePreSimCount > 0)
		{
			// Get a new compute command buffer
			commandBufferID = commandPool->AllocateBuffer();
			commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

			// Start the simulation compute pass
			commandBuffer->BeginCommands();
			commandBuffer->BindComputePipeline(s_SimComputePipeline);

			// Push the buffer descriptors
			s_PushDescriptors->Clear();
			s_PushDescriptors->AddBuffer(ParticleBatcher::GetParticleBuffer(), 2);
			s_PushDescriptors->AddBuffer(ParticleBatcher::GetCounterBuffer(), 3);
			s_PushDescriptors->AddBuffer(ParticleBatcher::GetAlivePreSimBuffer(), 4);
			s_PushDescriptors->AddBuffer(ParticleBatcher::GetAlivePostSimBuffer(), 5);
			s_PushDescriptors->AddBuffer(ParticleBatcher::GetDeadBuffer(), 6);

			// Push the descriptors and dispatch
			uint32_t groups = (uint32_t)(std::ceil(s_ParticleCounts.AlivePreSimCount / 256.0f));
			commandBuffer->PushDescriptorsCompute(s_PushDescriptors.get(), s_SimComputePipeline);
			commandBuffer->Dispatch(groups, 1, 1);

			// End commands and release
			commandBuffer->EndCommands();
			commandBuffer->SubmitCompute();
			commandPool->ReleaseBuffer(commandBufferID);

			// Swap the buffers for the next frame
			SwapBuffers();
		}
	}

	void ParticleBatcher::InitEmitResources()
	{
		// Create the shader layout
		s_EmitDescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(s_EmitDescriptorLayout);
		descriptorLayout->AddBinding("Particle Buffer", DescriptorType::Storage, ShaderStage::Compute, 2);
		descriptorLayout->AddBinding("Counter Buffer", DescriptorType::Storage, ShaderStage::Compute, 3);
		descriptorLayout->AddBinding("Alive Pre-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 4);
		descriptorLayout->AddBinding("Alive Post-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 5);
		descriptorLayout->AddBinding("Dead Buffer", DescriptorType::Storage, ShaderStage::Compute, 6);
		descriptorLayout->AddBinding("Emitter Data", DescriptorType::Uniform, ShaderStage::Compute, 7);
		descriptorLayout->Apply();

		// Load the emit shader by GUID
		s_EmitShader = AssetManager::LoadAsset<Shader>(s_EmitShaderGUID);

		// Create the compute pipeline
		VulkanPipelineInfo info;
		info.Shaders = s_EmitShader->GetResourceMap();
		info.DescriptorLayout = s_EmitDescriptorLayout;
		info.BindVertexAttributeDescriptions = false;
		s_EmitComputePipeline = ResourceManager::Allocate<VulkanComputePipeline>(info);

		// Allocate the emitter data uniform buffer
		s_EmitterBufferSize = sizeof(ParticleEmitterData);
		s_EmitterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, s_EmitterBufferSize);
	}

	void ParticleBatcher::InitSimulationResources()
	{
		// Create the shader layout
		s_SimDescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(s_SimDescriptorLayout);
		descriptorLayout->AddBinding("Particle Buffer", DescriptorType::Storage, ShaderStage::Compute, 2);
		descriptorLayout->AddBinding("Counter Buffer", DescriptorType::Storage, ShaderStage::Compute, 3);
		descriptorLayout->AddBinding("Alive Pre-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 4);
		descriptorLayout->AddBinding("Alive Post-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 5);
		descriptorLayout->AddBinding("Dead Buffer", DescriptorType::Storage, ShaderStage::Compute, 6);
		descriptorLayout->Apply();

		// Load the emit shader by GUID
		s_SimShader = AssetManager::LoadAsset<Shader>(s_SimShaderGUID);

		// Create the compute pipeline
		VulkanPipelineInfo info;
		info.Shaders = s_SimShader->GetResourceMap();
		info.DescriptorLayout = s_SimDescriptorLayout;
		info.BindVertexAttributeDescriptions = false;
		s_SimComputePipeline = ResourceManager::Allocate<VulkanComputePipeline>(info);
	}

	void ParticleBatcher::SwapBuffers()
	{
		// Get the buffers
		auto alivePreSimBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AlivePreSimBuffer);
		auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(ParticleBatcher::GetCounterBuffer());
		auto alivePostSimBuffer = ResourceManager::GetResource<VulkanBuffer>(m_AlivePostSimBuffer);
		
		// Copy the buffers into cpu memory
		alivePreSimBuffer->CopyBufferMemory(m_AlivePreSimList.data());
		counterBuffer->CopyBufferMemory(&s_ParticleCounts);
		alivePostSimBuffer->CopyBufferMemory(m_AlivePostSimList.data());

		// Store the number of alive particles for the current frame before swapping
		m_CurrentFrameAlive = s_ParticleCounts.AlivePreSimCount;

		// Swap the alive pre and post sim counts/lists
		std::swap(m_AlivePreSimList, m_AlivePostSimList);

		// Swap the pre/post sim counts and reset the post sim count to 0
		s_ParticleCounts.AlivePreSimCount = s_ParticleCounts.AlivePostSimCount;
		s_ParticleCounts.AlivePostSimCount = 0;

		// Upload the swapped data back to the gpu
		counterBuffer->UploadData(&s_ParticleCounts, m_CounterBufferSize);
		alivePreSimBuffer->UploadData(m_AlivePreSimList.data(), m_ListBufferSize);
		alivePostSimBuffer->UploadData(m_AlivePostSimList.data(), m_ListBufferSize);
	}
}