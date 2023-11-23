#pragma once
#include "ResourceHandle.h"
#include "RenderGraphNodes.h"
#include "Drawcall.h"

namespace Odyssey
{
	class RenderGraphNode;
	class VulkanContext;
	struct PerFrameRenderingData;
	class VulkanCommandBuffer;

	class RenderGraph
	{
	public:
		void Setup();
		void Compile();
		void Execute();

		template<typename T, typename... Args>
		T* CreateNode(Args... args)
		{
			m_AllNodes.push_back(std::make_unique<T>(args...));
			return static_cast<T*>(m_AllNodes[m_AllNodes.size() - 1].get());
		}

		void SetRootNode(RenderGraphNode* root) { m_RootNode = root; }

	public:
		void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle);
		void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle);
	
	private:
		RenderGraphNode* m_RootNode;
		std::vector<std::unique_ptr<RenderGraphNode>> m_AllNodes;
	};
}