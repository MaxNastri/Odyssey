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

		VkDeviceSize dataSize = indices.size() * sizeof(indices[0]);

		m_StagingBuffer = std::make_shared<VulkanBuffer>(context, BufferType::Staging, dataSize);
		m_StagingBuffer->SetMemory(dataSize, indices.data());

		m_IndexBuffer = std::make_shared<VulkanBuffer>(context, BufferType::Index, dataSize);

		// Copy the staging buffer into the vertex buffer
		VulkanCommandBuffer* commandBuffer = m_Context->GetCommandPool()->AllocateBuffer();
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(m_StagingBuffer.get(), m_IndexBuffer.get(), (uint32_t)dataSize);
		commandBuffer->EndCommands();

		m_Context->SubmitCommandBuffer(commandBuffer);
		m_Context->GetCommandPool()->ReleaseBuffer(commandBuffer);
	}

	void VulkanIndexBuffer::Destroy()
	{
		m_StagingBuffer->Destroy();
		m_StagingBuffer.reset();

		m_IndexBuffer->Destroy();
		m_IndexBuffer.reset();
	}
	VulkanBuffer* VulkanIndexBuffer::GetIndexBuffer()
	{
		return m_IndexBuffer.get();
	}
	const VkBuffer VulkanIndexBuffer::GetIndexBufferVK()
	{
		return m_IndexBuffer->buffer;
	}
	const VkBuffer* VulkanIndexBuffer::GetIndexBufferVKRef()
	{
		return &(m_IndexBuffer->buffer);
	}
}