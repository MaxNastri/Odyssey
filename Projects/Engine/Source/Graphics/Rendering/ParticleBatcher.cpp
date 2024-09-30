#include "ParticleBatcher.h"
#include "ResourceManager.h"
#include "VulkanStorageBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPushDescriptors.h"

namespace Odyssey
{
	void ParticleBatcher::Init()
	{
		m_BufferSize = sizeof(Particle) * MAX_PARTICLES;
		m_StorageBuffer = ResourceManager::Allocate<VulkanStorageBuffer>(BufferType::Storage, 0, m_BufferSize);
		m_StagingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, m_BufferSize);

		for (size_t i = 0; i < 100; i++)
		{
			m_Particles[i].Position = glm::vec4(i * 2, 0.0f, 0.0f, 1.0f);
			m_Particles[i].Velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StagingBuffer);
		stagingBuffer->CopyData(m_BufferSize, m_Particles.data());

		auto storageBuffer = ResourceManager::GetResource<VulkanStorageBuffer>(m_StorageBuffer);

		ResourceID commandPoolID = ResourceManager::Allocate<VulkanCommandPool>(VulkanQueueType::Compute);
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);

		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_StorageBuffer, (uint32_t)m_BufferSize);
		commandBuffer->EndCommands();

		commandBuffer->SubmitGraphics();
		commandPool->ReleaseBuffer(commandBufferID);

	}

	void ParticleBatcher::Shutdown()
	{
		if (m_StorageBuffer)
			ResourceManager::Destroy(m_StorageBuffer);

		if (m_StagingBuffer)
			ResourceManager::Destroy(m_StagingBuffer);
	}

	void ParticleBatcher::Update(const std::vector<ParticleSystem>& systems)
	{

	}
}