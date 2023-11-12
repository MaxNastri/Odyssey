#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

// FWD Declarations
struct GLFWwindow;

namespace Odyssey::Graphics
{
	class VulkanDevice;
	class GUIElement;

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
		};

	public:
		VulkanImgui(const InitInfo& initInfo);
		void SubmitDraws();
		void Render(VkCommandBuffer commandBuffer);
		void PostRender();

	private:
		bool showDemoWindow = true;
	};
}