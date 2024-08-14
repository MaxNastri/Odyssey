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

		uint32_t dataSize = (uint32_t)(vertices.size() * sizeof(vertices[0]));

		m_StagingBuffer = ResourceManager::AllocateBuffer(BufferType::Staging, dataSize);
		m_StagingBuffer.Get()->AllocateMemory();
		m_StagingBuffer.Get()->SetMemory(dataSize, vertices.data());

		m_VertexBuffer = ResourceManager::AllocateBuffer(BufferType::Vertex, dataSize);
		m_VertexBuffer.Get()->AllocateMemory();

		// Copy the staging buffer into the vertex buffer
		ResourceHandle<VulkanCommandPool> commandPool = m_Context->GetCommandPool();
		ResourceHandle<VulkanCommandBuffer> bufferHandle = commandPool.Get()->AllocateBuffer();

		VulkanCommandBuffer* commandBuffer = bufferHandle.Get();
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_VertexBuffer, (uint32_t)dataSize);
		commandBuffer->EndCommands();

		m_Context->SubmitCommandBuffer(bufferHandle);
		commandPool.Get()->ReleaseBuffer(bufferHandle);
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