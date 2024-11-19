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
#include "GameObject.h"

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
		s_EmitterBufferSize = sizeof(ParticleEmitterData);

		// Create the shared buffers (Alive, Dead, Particle Counter)
		for (size_t i = 0; i < s_EmitterResources.size(); i++)
		{
			auto& emitterResources = s_EmitterResources[i];
			emitterResources.AlivePreSimBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
			emitterResources.AlivePostSimBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
			emitterResources.DeadBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ListBufferSize);
			emitterResources.ParticleBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_ParticleBufferSize);
			emitterResources.CounterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_CounterBufferSize);
			emitterResources.EmitterBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, s_EmitterBufferSize);

			// Fill the dead list with valid particle indices
			for (size_t d = 0; d < emitterResources.DeadList.size(); d++)
			{
				emitterResources.DeadList[d] = (uint32_t)d;
			}

			// Initialize the alive counts to 0 and dead count to all particles
			emitterResources.ParticleCounts.AlivePreSimCount = 0;
			emitterResources.ParticleCounts.AlivePostSimCount = 0;
			emitterResources.ParticleCounts.DeadCount = MAX_PARTICLES;

			auto alivePreSimBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.AlivePreSimBuffer);
			auto alivePostSimBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.AlivePostSimBuffer);
			auto deadBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.DeadBuffer);
			auto particleBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.ParticleBuffer);
			auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.CounterBuffer);

			// Update all buffers with initial data
			alivePreSimBuffer->UploadData(emitterResources.AlivePreSimList.data(), m_ListBufferSize);
			alivePostSimBuffer->UploadData(emitterResources.AlivePostSimList.data(), m_ListBufferSize);
			deadBuffer->UploadData(emitterResources.DeadList.data(), m_ListBufferSize);
			particleBuffer->UploadData(emitterResources.ParticleData.data(), m_ParticleBufferSize);
			counterBuffer->UploadData(&emitterResources.ParticleCounts, m_CounterBufferSize);
		}

		// Initialize the emit and simulation resources
		InitEmitResources();
		InitSimulationResources();

		// Fill our resource indices with defaults
		for (size_t i = 0; i < MAX_EMITTERS; i++)
		{
			s_ResourceIndices.push(i);
		}
	}

	void ParticleBatcher::Shutdown()
	{
		//if (m_ParticleBuffer)
		//	ResourceManager::Destroy(m_ParticleBuffer);
	}

	void ParticleBatcher::Update()
	{
		// Clear our draw list from the previous frame
		s_DrawList.clear();

		Scene* scene = SceneManager::GetActiveScene();
		auto emitterEntities = scene->GetAllEntitiesWith<ParticleEmitter>();

		// Don't run without any emitters
		if (emitterEntities.size() == 0)
			return;

		// Allocate a compute command buffer from the pool
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(s_CommandPool);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		for (auto& entity : emitterEntities)
		{
			// Get the particle emitter
			GameObject gameObject = GameObject(scene, entity);
			ParticleEmitter& emitter = gameObject.GetComponent<ParticleEmitter>();

			if (!emitter.IsEnabled())
				continue;

			// Start the emit compute pass
			commandBuffer->BeginCommands();
			commandBuffer->BindComputePipeline(s_EmitComputePipeline);

			if (!s_EntityToResourceIndex.contains(gameObject))
			{
				Log::Error("[ParticleBatcher] Attempting to update un-registered emitter: " + gameObject.GetName() + ".");
				return;
			}

			size_t emitterIndex = s_EntityToResourceIndex[gameObject];
			PerEmitterResources& emitterResources = s_EmitterResources[emitterIndex];

			// Update the emitter and generate some randomness
			emitter.Update(Time::DeltaTime());
			auto& emitterData = emitter.GetEmitterData();
			emitterData.Rnd.x = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.y = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.z = (Random::Float01() - 0.5f) * 2.0f;
			emitterData.Rnd.w = Random::Float01();

			// Update the emitter buffer
			auto emitterBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.EmitterBuffer);
			emitterBuffer->CopyData(s_EmitterBufferSize, &emitterData);

			// Push the buffer descriptors
			s_PushDescriptors->Clear();
			s_PushDescriptors->AddBuffer(emitterResources.ParticleBuffer, 2);
			s_PushDescriptors->AddBuffer(emitterResources.CounterBuffer, 3);
			s_PushDescriptors->AddBuffer(emitterResources.AlivePreSimBuffer, 4);
			s_PushDescriptors->AddBuffer(emitterResources.AlivePostSimBuffer, 5);
			s_PushDescriptors->AddBuffer(emitterResources.DeadBuffer, 6);
			s_PushDescriptors->AddBuffer(emitterResources.EmitterBuffer, 7);

			// Push the descriptors and dispatch
			uint32_t groups = (uint32_t)(std::ceil(emitterData.EmitCount / 64.0f));
			commandBuffer->PushDescriptorsCompute(s_PushDescriptors.get(), s_EmitComputePipeline);
			commandBuffer->Dispatch(groups, 1, 1);

			// End and submit the commands
			commandBuffer->EndCommands();
			commandBuffer->SubmitCompute();

			// Get the updated particle counts from the GPU
			auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.CounterBuffer);
			counterBuffer->CopyBufferMemory(&emitterResources.ParticleCounts);

			if (emitterResources.ParticleCounts.AlivePreSimCount > 0)
			{
				// Add this emitter to the current frame's draw list
				s_DrawList.push_back(emitterIndex);

				// Start the simulation compute pass
				commandBuffer->BeginCommands();
				commandBuffer->BindComputePipeline(s_SimComputePipeline);

				// Push the buffer descriptors
				s_PushDescriptors->Clear();
				s_PushDescriptors->AddBuffer(emitterResources.ParticleBuffer, 2);
				s_PushDescriptors->AddBuffer(emitterResources.CounterBuffer, 3);
				s_PushDescriptors->AddBuffer(emitterResources.AlivePreSimBuffer, 4);
				s_PushDescriptors->AddBuffer(emitterResources.AlivePostSimBuffer, 5);
				s_PushDescriptors->AddBuffer(emitterResources.DeadBuffer, 6);
				s_PushDescriptors->AddBuffer(emitterResources.EmitterBuffer, 7);

				// Push the descriptors and dispatch
				uint32_t groups = (uint32_t)(std::ceil(emitterResources.ParticleCounts.AlivePreSimCount / 256.0f));
				commandBuffer->PushDescriptorsCompute(s_PushDescriptors.get(), s_SimComputePipeline);
				commandBuffer->Dispatch(groups, 1, 1);

				// End commands and release
				commandBuffer->EndCommands();
				commandBuffer->SubmitCompute();

				// Swap the buffers for the next frame
				SwapBuffers(emitterIndex);
			}
		}

		commandPool->ReleaseBuffer(commandBufferID);
	}

	void ParticleBatcher::RegisterEmitter(ParticleEmitter* emitter)
	{
		GameObject gameObject = emitter->GetGameObject();
		if (!s_EntityToResourceIndex.contains(gameObject))
		{
			size_t index = s_ResourceIndices.front();
			s_ResourceIndices.pop();
			s_EntityToResourceIndex[gameObject] = index;
		}
	}

	void ParticleBatcher::DeregisterEmtter(ParticleEmitter* emitter)
	{
		GameObject gameObject = emitter->GetGameObject();
		if (s_EntityToResourceIndex.contains(gameObject))
		{
			s_ResourceIndices.push(s_EntityToResourceIndex[gameObject]);
			s_EntityToResourceIndex.erase(gameObject);
		}
	}

	uint32_t ParticleBatcher::GetAliveCount(size_t index)
	{
		return s_EmitterResources[index].CurrentFrameAliveCount;
	}

	ResourceID ParticleBatcher::GetParticleBuffer(size_t index)
	{
		return s_EmitterResources[index].ParticleBuffer;
	}

	ResourceID ParticleBatcher::GetAliveBuffer(size_t index)
	{
		return s_EmitterResources[index].AlivePreSimBuffer;
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
		descriptorLayout->AddBinding("Emitter Data", DescriptorType::Uniform, ShaderStage::Compute, 7);
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

	void ParticleBatcher::SwapBuffers(size_t index)
	{
		PerEmitterResources& emitterResources = s_EmitterResources[index];

		// Get the buffers
		auto alivePreSimBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.AlivePreSimBuffer);
		auto counterBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.CounterBuffer);
		auto alivePostSimBuffer = ResourceManager::GetResource<VulkanBuffer>(emitterResources.AlivePostSimBuffer);
		
		// Copy the buffers into cpu memory
		alivePreSimBuffer->CopyBufferMemory(emitterResources.AlivePreSimList.data());
		counterBuffer->CopyBufferMemory(&emitterResources.ParticleCounts);
		alivePostSimBuffer->CopyBufferMemory(emitterResources.AlivePostSimList.data());

		// Store the number of alive particles for the current frame before swapping
		emitterResources.CurrentFrameAliveCount = emitterResources.ParticleCounts.AlivePreSimCount;

		// Swap the alive pre and post sim counts/lists
		std::swap(emitterResources.AlivePreSimList, emitterResources.AlivePostSimList);

		// Swap the pre/post sim counts and reset the post sim count to 0
		emitterResources.ParticleCounts.AlivePreSimCount = emitterResources.ParticleCounts.AlivePostSimCount;
		emitterResources.ParticleCounts.AlivePostSimCount = 0;

		// Upload the swapped data back to the gpu
		counterBuffer->UploadData(&emitterResources.ParticleCounts, m_CounterBufferSize);
		alivePreSimBuffer->UploadData(emitterResources.AlivePreSimList.data(), m_ListBufferSize);
		alivePostSimBuffer->UploadData(emitterResources.AlivePostSimList.data(), m_ListBufferSize);
	}
}