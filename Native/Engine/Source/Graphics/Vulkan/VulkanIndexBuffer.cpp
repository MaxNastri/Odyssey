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

		m_StagingBuffer = ResourceManager::AllocateBuffer(BufferType::Staging, dataSize);
		m_StagingBuffer.Get()->AllocateMemory();
		m_StagingBuffer.Get()->SetMemory(dataSize, indices.data());

		m_IndexBuffer = ResourceManager::AllocateBuffer(BufferType::Index, dataSize);
		m_IndexBuffer.Get()->AllocateMemory();

		// Allocate a command buffer
		ResourceHandle<VulkanCommandPool> commandPool = m_Context->GetCommandPool();
		ResourceHandle<VulkanCommandBuffer> bufferHandle = commandPool.Get()->AllocateBuffer();

		// Copy the staging data into the index buffer
		VulkanCommandBuffer* commandBuffer = bufferHandle.Get();
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer, m_IndexBuffer, (uint32_t)dataSize);
		commandBuffer->EndCommands();

		// Submit and release
		m_Context->SubmitCommandBuffer(bufferHandle);
		commandPool.Get()->ReleaseBuffer(bufferHandle);
	}

	void VulkanIndexBuffer::Destroy()
	{
		ResourceManager::DestroyBuffer(m_StagingBuffer);
		ResourceManager::DestroyBuffer(m_IndexBuffer);
	}

	ResourceHandle<VulkanBuffer> VulkanIndexBuffer::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	const VkBuffer VulkanIndexBuffer::GetIndexBufferVK()
	{
		return m_IndexBuffer.Get()->buffer;
	}

	const VkBuffer* VulkanIndexBuffer::GetIndexBufferVKRef()
	{
		return &(m_IndexBuffer.Get()->buffer);
	}
}