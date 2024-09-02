#include "VulkanVertexBuffer.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "ResourceManager.h"

namespace Odyssey
{
	VulkanVertexBuffer::VulkanVertexBuffer(std::shared_ptr<VulkanContext> context, std::vector<Vertex>& vertices)
	{
		m_Context = context;

		uint32_t dataSize = (uint32_t)(vertices.size() * sizeof(vertices[0]));

		// Allocate the buffers
		m_StagingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, dataSize);
		m_VertexBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Vertex, dataSize);

		// Write the vertices into the staging buffer
		auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StagingBuffer);
		stagingBuffer->AllocateMemory();
		stagingBuffer->SetMemory(dataSize, vertices.data());

		// Allocate memory for the vertex buffer
		auto vertexBuffer = ResourceManager::GetResource<VulkanBuffer>(m_VertexBuffer);
		vertexBuffer->AllocateMemory();

		// Copy the staging buffer into the vertex buffer
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetCommandPool());

		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_VertexBuffer, (uint32_t)dataSize);
		commandBuffer->EndCommands();

		m_Context->SubmitCommandBuffer(commandBufferID);
		commandPool->ReleaseBuffer(commandBufferID);
	}

	void VulkanVertexBuffer::Destroy()
	{
		ResourceManager::Destroy(m_StagingBuffer);
		ResourceManager::Destroy(m_VertexBuffer);
	}

	ResourceID VulkanVertexBuffer::GetBuffer()
	{
		return m_VertexBuffer;
	}

	void VulkanVertexBuffer::UploadData(const std::vector<Vertex>& vertices)
	{
		// Upload the vertex data into the staging buffer
		uint32_t dataSize = (uint32_t)(vertices.size() * sizeof(Vertex));
		auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StagingBuffer);
		stagingBuffer->SetMemory(dataSize, vertices.data());

		// Allocate a commandbuffer
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetCommandPool());
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Copy the staging buffer into the vertex buffer
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_VertexBuffer, (uint32_t)dataSize);
		commandBuffer->EndCommands();

		m_Context->SubmitCommandBuffer(commandBufferID);
		commandPool->ReleaseBuffer(commandBufferID);
	}
}