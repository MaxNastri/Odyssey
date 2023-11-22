#include "VulkanVertexBuffer.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "ResourceManager.h"

namespace Odyssey
{
	VulkanVertexBuffer::VulkanVertexBuffer(std::shared_ptr<VulkanContext> context, std::vector<VulkanVertex>& vertices)
	{
		m_Context = context;

		VkDeviceSize dataSize = vertices.size() * sizeof(vertices[0]);

		m_StagingBuffer = ResourceManager::AllocateBuffer(BufferType::Staging, dataSize);
		m_StagingBuffer.Get()->AllocateMemory();
		m_StagingBuffer.Get()->SetMemory(dataSize, vertices.data());

		m_VertexBuffer = ResourceManager::AllocateBuffer(BufferType::Vertex, dataSize);
		m_VertexBuffer.Get()->AllocateMemory();

		// Copy the staging buffer into the vertex buffer
		VulkanCommandBuffer* commandBuffer = m_Context->GetCommandPool()->AllocateBuffer();
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_VertexBuffer, (uint32_t)dataSize);
		commandBuffer->EndCommands();

		m_Context->SubmitCommandBuffer(commandBuffer);
		m_Context->GetCommandPool()->ReleaseBuffer(commandBuffer);
	}
	void VulkanVertexBuffer::Destroy()
	{
		ResourceManager::DestroyBuffer(m_StagingBuffer);
		ResourceManager::DestroyBuffer(m_VertexBuffer);
	}

	ResourceHandle<VulkanBuffer> VulkanVertexBuffer::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}
	const VkBuffer VulkanVertexBuffer::GetVertexBufferVK()
	{
		return m_VertexBuffer.Get()->buffer;
	}
	const VkBuffer* VulkanVertexBuffer::GetVertexBufferVKRef()
	{
		return &(m_VertexBuffer.Get()->buffer);
	}
}