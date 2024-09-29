#include "VulkanIndexBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
namespace Odyssey
{
	VulkanIndexBuffer::VulkanIndexBuffer(std::shared_ptr<VulkanContext> context, std::vector<uint32_t>& indices)
	{
		m_Context = context;

		uint32_t dataSize = (uint32_t)(indices.size() * sizeof(indices[0]));

		m_StagingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, dataSize);
		m_IndexBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Index, dataSize);

		auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StagingBuffer);
		stagingBuffer->AllocateMemory();
		stagingBuffer->SetMemory(dataSize, indices.data());

		auto indexBuffer = ResourceManager::GetResource<VulkanBuffer>(m_IndexBuffer);
		indexBuffer->AllocateMemory();

		// Allocate a command buffer
		ResourceID commandPoolID = m_Context->GetGraphicsCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);

		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Copy the staging data into the index buffer
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_IndexBuffer, (uint32_t)dataSize);
		commandBuffer->EndCommands();

		// Submit and release
		commandBuffer->SubmitGraphics();
		commandPool->ReleaseBuffer(commandBufferID);
	}

	void VulkanIndexBuffer::Destroy()
	{
		ResourceManager::Destroy(m_StagingBuffer);
		ResourceManager::Destroy(m_IndexBuffer);
	}
}