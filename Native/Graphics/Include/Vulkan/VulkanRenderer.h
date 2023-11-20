#pragma once
#include <vulkan/vulkan.h>
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanImgui.h"
#include "VulkanSwapchain.h"
#include "VulkanFrame.h"
#include "VulkanCommandPool.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertex.h"

namespace Odyssey
{
	class VulkanContext;
	class VulkanBuffer;
	class VulkanWindow;
	class VulkanFrame;
	class VulkanCommandBuffer;
	class VulkanVertexBuffer;

	struct DrawCall
	{
	public:
		std::shared_ptr<VulkanVertexBuffer> VertexBuffer;
		std::shared_ptr<VulkanBuffer> IndexBuffer;
		uint32_t VertexCount;
	};

	struct RenderObject
	{
	public:
		RenderObject(std::vector<VulkanVertex> vertices, std::vector<uint32_t> indices)
		{
			m_Vertices = vertices;
			m_Indices = indices;
		}

		std::vector<VulkanVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};

	class VulkanRenderer
	{
	public:
		VulkanRenderer();
		void Destroy();

	public:
		bool Update();
		bool Render();
		bool Present();

	private:
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();
		void InitDrawCalls();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();
		VkRenderingInfo GetRenderingInfo(VulkanFrame* frame);

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;

	private: // Commands
		std::vector<std::unique_ptr<VulkanCommandPool>> commandPool;
		std::vector<VulkanCommandBuffer*> commandBuffers;

	private: // Pipeline
		std::unique_ptr<VulkanGraphicsPipeline> graphicsPipeline;
		std::unique_ptr<VulkanShader> fragmentShader;
		std::unique_ptr<VulkanShader> vertexShader;

	private: // Draws
		std::vector<DrawCall> m_DrawCalls;
		std::vector<RenderObject> m_RenderObjects;
		std::vector<std::shared_ptr<VulkanVertexBuffer>> m_VertexBuffers;
		std::vector<std::shared_ptr<VulkanBuffer>> m_IndexBuffers;

	private: // IMGUI
		std::unique_ptr<VulkanImgui> imgui;

	private: // Swapchain
		std::unique_ptr<VulkanSwapchain> swapchain;
		bool rebuildSwapchain = false;

	private: // Frame data
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
	};
}