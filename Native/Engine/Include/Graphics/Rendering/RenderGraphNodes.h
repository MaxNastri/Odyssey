#pragma once
#include "ResourceHandle.h"
#include "Drawcall.h"
#include "glm.h"
#include "PerFrameRenderingData.h"
#include <bitset>

namespace Odyssey
{
	class VulkanCommandBuffer;
	class VulkanContext;
	class VulkanImgui;
	class VulkanGraphicsPipeline;
	class VulkanShader;
	class VulkanTexture;

	class RenderGraphNode
	{
	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) = 0;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) = 0;
		virtual void Destroy() = 0;

	public:
		std::string_view GetName() { return m_Name; }
		void SetNext(RenderGraphNode* next) { m_Next = next; }
		RenderGraphNode* GetNext() { return m_Next; }

	protected:
		std::string m_Name;
		RenderGraphNode* m_Next = nullptr;
	};

	class BeginPassNode : public RenderGraphNode
	{
	public:
		BeginPassNode() = default;
		BeginPassNode(const std::string& name);
		BeginPassNode(const std::string& name, ResourceHandle<VulkanTexture> renderTarget);
		virtual void Destroy() override;

	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;

	public:
		ResourceHandle<VulkanTexture> GetRenderTarget() { return m_RenderTarget; }

	private:
		ResourceHandle<VulkanTexture> m_RenderTarget;
		glm::vec4 m_ClearValue;
	};

	class SetPipelineNode : public RenderGraphNode
	{
	public:
		SetPipelineNode() = default;
		SetPipelineNode(const std::string& name, ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts);

	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Destroy() override;

	public:
		ResourceHandle<VulkanShader> GetVertexShader() { return m_VertexShader; }
		ResourceHandle<VulkanShader> GetFragmentShader() { return m_FragmentShader; }

	private:
		ResourceHandle<VulkanShader> m_VertexShader;
		ResourceHandle<VulkanShader> m_FragmentShader;
		ResourceHandle<VulkanGraphicsPipeline> m_GraphicsPipeline;
	};

	class DrawNode : public RenderGraphNode
	{
	public:
		DrawNode() = default;
		DrawNode(const std::string& name);
		DrawNode(const std::string& name, Drawcall& drawcall);
		virtual void Destroy() override;

	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;

	private:
		std::bitset<16> m_RenderingLayers;
		Drawcall m_Drawcall;
	};

	class EndPassNode : public RenderGraphNode
	{
	public:
		EndPassNode() = default;
		EndPassNode(const std::string& name);
		EndPassNode(const std::string& name, VkImageLayout oldLayout, VkImageLayout newLayout);
		EndPassNode(const std::string& name, ResourceHandle<VulkanTexture> renderTarget, VkImageLayout oldLayout, VkImageLayout newLayout);
		virtual void Destroy() override;

	public:
		void SetLayouts(VkImageLayout oldLayout, VkImageLayout newLayout);

	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;

	private:
		VkImageLayout m_OldLayout;
		VkImageLayout m_NewLayout;
		ResourceHandle<VulkanTexture> m_RenderTarget;
	};

	class ImguiDrawNode : public RenderGraphNode
	{
	public:
		ImguiDrawNode() = default;
		ImguiDrawNode(const std::string& name);
		ImguiDrawNode(const std::string& name, std::shared_ptr<VulkanImgui> imgui);
		virtual void Destroy() override;

	public:
		void AddDescriptorSet(VkDescriptorSet descriptorSet);

	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
	
	private:
		std::shared_ptr<VulkanImgui> m_Imgui;
		VkDescriptorSet m_DescriptorSet;
	};

	class SubmitNode : public RenderGraphNode
	{
	public:
		SubmitNode() = default;
		SubmitNode(const std::string& name);
		virtual void Destroy() override;

	public:
		virtual void Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
		virtual void Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle) override;
	};
}