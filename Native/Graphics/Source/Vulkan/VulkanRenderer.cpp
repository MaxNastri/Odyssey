#include "VulkanRenderer.h"
#include "VulkanGlobals.h"
#include <assert.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace Odyssey::Graphics
{
	VulkanRenderer::VulkanRenderer()
	{
		window = std::make_unique<Window>();
		GatherExtensions();
		CreateInstance();
		device = std::make_unique<VulkanDevice>(instance);

		VkDevice vkDevice = device->GetLogicalDevice();
		uint32_t graphicsIndex = device->GetFamilyIndex(VulkanQueueType::Graphics);

		graphicsQueue = std::make_unique<VulkanQueue>(VulkanQueueType::Graphics, vkDevice, graphicsIndex);
		descriptorPool = std::make_unique<VulkanDescriptorPool>(vkDevice);
		surface = std::make_unique<VulkanSurface>(instance, device->GetPhysicalDevice(), graphicsIndex, window->GetWindowHandle());
		renderPass = std::make_unique<VulkanRenderPass>(vkDevice, surface->GetFormat().format);
		swapchain = std::make_unique<VulkanSwapchain>(vkDevice, device->GetPhysicalDevice(), surface.get());

		VulkanImgui::InitInfo info = CreateImguiInitInfo();
		imgui = std::make_unique<VulkanImgui>(info);

		SetupFrameData();
	}

	bool VulkanRenderer::Update()
	{
		return window->Update();
	}

	bool VulkanRenderer::Render(std::vector<std::shared_ptr<GUIElement>> guiElements)
	{
		// if rebuild swapchain, do it
		if (rebuildSwapchain)
		{
			RebuildSwapchain();
		}

		imgui->SubmitDraws(guiElements);
		RenderFrame();
		imgui->PostRender();
		Present();
		return true;
	}

	void VulkanRenderer::RenderFrame()
	{
		VkResult err;
		VkDevice vkDevice = device->GetLogicalDevice();
		VkClearValue ClearValue;
		ClearValue.color.float32[0] = 0.0f;
		ClearValue.color.float32[1] = 0.0f;
		ClearValue.color.float32[2] = 0.0f;
		ClearValue.color.float32[3] = 0.0f;

		VkSemaphore image_acquired_semaphore = frames[frameIndex]->GetImageAcquiredSemaphore();
		VkSemaphore render_complete_semaphore = frames[frameIndex]->GetRenderCompleteSemaphore();
		err = vkAcquireNextImageKHR(vkDevice, swapchain->GetVK(), UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &frameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return;
		}

		check_vk_result(err);

		VulkanFrame* frame = frames[frameIndex].get();

		// Wait for the initial fences to clear
		err = vkWaitForFences(vkDevice, 1, &(frame->fence), VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		check_vk_result(err);

		err = vkResetFences(vkDevice, 1, &frame->fence);
		check_vk_result(err);

		frame->commandPool->Reset(vkDevice);

		// Command buffer begin
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(frame->commandBuffer, &info);
		check_vk_result(err);

		// RenderPass begin
		renderPass->Begin(frame->commandBuffer, frame->framebuffer, surface->GetWidth(), surface->GetHeight(), ClearValue);

		// TODO: DRAW
		imgui->Render(frame->commandBuffer);

		// RenderPass end
		renderPass->End(frame->commandBuffer);

		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &image_acquired_semaphore;
		submitInfo.pWaitDstStageMask = &wait_stage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frame->commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(frame->commandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(graphicsQueue->queue, 1, &submitInfo, frame->fence);
		check_vk_result(err);
	}

	void VulkanRenderer::Present()
	{
		if (rebuildSwapchain)
			return;

		VkSemaphore render_complete_semaphore = frames[frameIndex]->GetRenderCompleteSemaphore();
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &swapchain->swapchain;
		info.pImageIndices = &frameIndex;
		VkResult err = vkQueuePresentKHR(graphicsQueue->queue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return;
		}
		check_vk_result(err);
		frameIndex = (frameIndex + 1) % swapchain->imageCount; // Now we can use the next set of semaphores
	}

	void VulkanRenderer::SetupFrameData()
	{
		VkDevice vkDevice = device->GetLogicalDevice();
		uint32_t graphicsIndex = device->GetFamilyIndex(VulkanQueueType::Graphics);
		VkFormat format = surface->GetFormat().format;

		std::vector<VkImage> backbuffers = swapchain->GetBackbuffers(vkDevice);
		uint32_t imageCount = swapchain->GetImageCount();
		frames.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			frames[i] = std::make_unique<VulkanFrame>(vkDevice, graphicsIndex);
			frames[i]->SetBackbuffer(vkDevice, backbuffers[i], format);
			frames[i]->CreateFramebuffer(vkDevice, renderPass->GetVK(), surface->GetWidth(), surface->GetHeight());
		}
	}

	void VulkanRenderer::RebuildSwapchain()
	{
		VkDevice vkDevice = device->GetLogicalDevice();
		VkResult err = vkDeviceWaitIdle(vkDevice);
		check_vk_result(err);

		for (auto& frame : frames)
		{
			frame->Destroy(vkDevice);
			frame.reset();
		}
		frames.clear();

		int width, height;
		glfwGetFramebufferSize(window->GetWindowHandle(), &width, &height);
		surface->SetFrameBufferSize(width, height);
		if (width > 0 && height > 0)
		{
			swapchain->Destroy(device->GetLogicalDevice());
			swapchain.reset();
			swapchain = std::make_unique<VulkanSwapchain>(device->GetLogicalDevice(), device->GetPhysicalDevice(), surface.get());

			ImGui_ImplVulkan_SetMinImageCount(swapchain->minImageCount);
			frameIndex = 0;
			rebuildSwapchain = false;

			SetupFrameData();
		}
	}

	void VulkanRenderer::GatherExtensions()
	{
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
		for (uint32_t i = 0; i < extensionsCount; ++i)
		{
			extensions.push_back(glfwExtensions[i]);
		}
	}

	void VulkanRenderer::CreateInstance()
	{
		VkResult err;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		// Enumerate available extensions
		uint32_t properties_count;
		std::vector<VkExtensionProperties> properties;
		vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.data());
		check_vk_result(err);

		// Enable required extensions
		if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		}
#endif

		if (enableValidation)
		{
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = layers;
			extensions.push_back("VK_EXT_debug_report");
		}

		// Create Vulkan Instance
		create_info.enabledExtensionCount = (uint32_t)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();
		err = vkCreateInstance(&create_info, allocator, &instance);
		check_vk_result(err);

		// Setup the debug report callback
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		assert(vkCreateDebugReportCallbackEXT != nullptr);

		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = nullptr;
		err = vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocator, &debugReport);
		check_vk_result(err);
	}

	VulkanImgui::InitInfo VulkanRenderer::CreateImguiInitInfo()
	{
		VulkanImgui::InitInfo info;
		info.window = window->GetWindowHandle();
		info.instance = instance;
		info.physicalDevice = device->GetPhysicalDevice();
		info.logicalDevice = device->GetLogicalDevice();
		info.queueIndex = device->GetFamilyIndex(VulkanQueueType::Graphics);
		info.queue = graphicsQueue->queue;
		info.descriptorPool = descriptorPool->descriptorPool;
		info.renderPass = renderPass->GetVK();
		info.minImageCount = swapchain->minImageCount;
		info.imageCount = swapchain->imageCount;
		return info;
	}

	bool VulkanRenderer::IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension)
	{
		for (const VkExtensionProperties& p : properties)
		{
			if (strcmp(p.extensionName, extension) == 0)
			{
				return true;
			}
		}

		return false;
	}
}