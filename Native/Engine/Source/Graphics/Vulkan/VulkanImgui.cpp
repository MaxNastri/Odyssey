#include "VulkanImgui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "ImGuizmo.h"
#include "VulkanGlobals.h"
#include "VulkanCommandPool.h"
#include "Events.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	VulkanImgui::VulkanImgui(std::shared_ptr<VulkanContext> context, const InitInfo& initInfo)
	{
		m_Context = context;

		CreateDescriptorPool();

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
		init_info.DescriptorPool = descriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = initInfo.minImageCount;
		init_info.ImageCount = initInfo.imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = allocator;
		init_info.CheckVkResultFn = nullptr;
		init_info.UseDynamicRendering = true;
		init_info.ColorAttachmentFormat = initInfo.colorFormat;

		ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* vulkan_instance) {
			return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name);
			}, &init_info.Instance);
		ImGui_ImplVulkan_Init(&init_info, initInfo.renderPass);


		// Upload Fonts
		{
			// Use any command queue
			ResourceHandle<VulkanCommandPool> commandPool = m_Context->GetCommandPool();
			ResourceHandle<VulkanCommandBuffer> bufferHandle = commandPool.Get()->AllocateBuffer();
			VulkanCommandBuffer* commandBuffer = bufferHandle.Get();

			commandBuffer->BeginCommands();

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer->GetCommandBuffer());

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = commandBuffer->GetCommandBufferRef();

			commandBuffer->EndCommands();

			VkResult err = vkQueueSubmit(initInfo.queue, 1, &submitInfo, VK_NULL_HANDLE);

			if (!check_vk_result(err))
			{
				Logger::LogError("(imgui 1)");
			}

			err = vkDeviceWaitIdle(initInfo.logicalDevice);
			if (!check_vk_result(err))
			{
				Logger::LogError("(imgui 2)");
			}

			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	void VulkanImgui::SubmitDraws()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		if (showDemoWindow)
		{
			ImGui::ShowDemoWindow(&showDemoWindow);
		}

		EventSystem::Dispatch<OnGUIRenderEvent>();
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

	uint64_t VulkanImgui::AddTexture(ResourceHandle<VulkanTexture> handle)
	{
		VulkanTexture* texture = handle.Get();
		VkSampler sampler = texture->GetSampler()->GetSamplerVK();
		VkImageView view = texture->GetImage()->GetImageView();
		VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
		return reinterpret_cast<uint64_t>(ImGui_ImplVulkan_AddTexture(sampler, view, layout));
	}

	void VulkanImgui::RemoveTexture(uint64_t id)
	{
		ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(id));
	}

	void VulkanImgui::CreateDescriptorPool()
	{
		VkDescriptorPoolSize pool_sizes[] = {
	   {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
	   {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
	   {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
	   {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
	   {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
	   {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
	   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
	   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
	   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
	   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
	   {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
		};

		uint32_t arrayLength = (uint32_t)(sizeof(pool_sizes) / sizeof(pool_sizes[0]));

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * arrayLength;
		pool_info.poolSizeCount = arrayLength;
		pool_info.pPoolSizes = pool_sizes;
		VkResult err = vkCreateDescriptorPool(m_Context->GetDeviceVK(), &pool_info, allocator, &descriptorPool);
		if (!check_vk_result(err))
		{
			Logger::LogError("(imgui 3)");
		}
	}
}