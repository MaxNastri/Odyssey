#include "VulkanImgui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "VulkanGlobals.h"
#include "VulkanCommandPool.h"
#include "GUIElement.h"

namespace Odyssey::Graphics
{
	VulkanImgui::VulkanImgui(const InitInfo& initInfo)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(initInfo.window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = initInfo.instance;
		init_info.PhysicalDevice = initInfo.physicalDevice;
		init_info.Device = initInfo.logicalDevice;
		init_info.QueueFamily = initInfo.queueIndex;
		init_info.Queue = initInfo.queue;
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = initInfo.descriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = initInfo.minImageCount;
		init_info.ImageCount = initInfo.imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = allocator;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, initInfo.renderPass);

		// Upload Fonts
		{
			// Use any command queue
			VulkanCommandPool pool(initInfo.logicalDevice, initInfo.queueIndex);
			VkCommandBuffer commandBuffer = pool.AllocateBuffer(initInfo.logicalDevice);

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			VkResult err = vkBeginCommandBuffer(commandBuffer, &begin_info);
			check_vk_result(err);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &commandBuffer;
			err = vkEndCommandBuffer(commandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(initInfo.queue, 1, &end_info, VK_NULL_HANDLE);
			check_vk_result(err);

			err = vkDeviceWaitIdle(initInfo.logicalDevice);
			check_vk_result(err);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	void VulkanImgui::SubmitDraws(std::vector<std::shared_ptr<GUIElement>> elements)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showDemoWindow)
		{
			ImGui::ShowDemoWindow(&showDemoWindow);
		}

		// Draw the gui elements
		for (const auto& element : elements)
		{
			element->Draw();
		}
	}

	void VulkanImgui::Render(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImDrawData* main_draw_data = ImGui::GetDrawData();

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
	}

	void VulkanImgui::PostRender()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}