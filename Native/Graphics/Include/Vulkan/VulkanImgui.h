#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

// FWD Declarations
struct GLFWwindow;

namespace Odyssey
{
	class VulkanDevice;
	class VulkanContext;
	class GUIElement;
	class VulkanTexture;

	class VulkanImgui
	{
	public:
		struct InitInfo
		{
			GLFWwindow* window;
			VkInstance instance;
			VkPhysicalDevice physicalDevice;
			VkDevice logicalDevice;
			uint32_t queueIndex;
			VkQueue queue;
			VkDescriptorPool descriptorPool;
			VkRenderPass renderPass;
			uint32_t minImageCount;
			uint32_t imageCount;
			VkFormat colorFormat;
		};

	public:
		VulkanImgui(std::shared_ptr<VulkanContext> context, const InitInfo& initInfo);
		void SubmitDraws();
		void Render(VkCommandBuffer commandBuffer, VkDescriptorSet id);
		void PostRender();
		VkDescriptorSet AddTexture(VulkanTexture* texture);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		bool showDemoWindow = true;
	};
}