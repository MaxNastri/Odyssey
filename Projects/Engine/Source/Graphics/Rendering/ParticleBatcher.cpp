#include "ParticleBatcher.h"
#include "ResourceManager.h"
#include "VulkanBuffer.h"
#include "VulkanPushDescriptors.h"

namespace Odyssey
{
	void ParticleBatcher::Init()
	{
		m_BufferSize = sizeof(Particle) * MAX_PARTICLES;
		m_StorageBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Storage, m_BufferSize);
		m_StagingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, m_BufferSize);

		for (size_t i = 0; i < 100; i++)
		{
			m_Particles[i].Position = glm::vec4(i * 2, 0.0f, 0.0f, 1.0f);
			m_Particles[i].Velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		auto storageBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StorageBuffer);
		storageBuffer->UploadData(m_Particles.data(), m_BufferSize);
	}

	void ParticleBatcher::Shutdown()
	{
		if (m_StorageBuffer)
			ResourceManager::Destroy(m_StorageBuffer);
	}

	void ParticleBatcher::Update(const std::vector<ParticleSystem>& systems)
	{

	}
}