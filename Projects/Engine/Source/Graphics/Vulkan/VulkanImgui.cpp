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
#include "RenderTarget.h"

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
		init_info.MSAASamples = (VkSampleCountFlagBits)m_Context->GetSampleCount();
		init_info.Allocator = allocator;
		init_info.CheckVkResultFn = nullptr;
		init_info.UseDynamicRendering = true;
		init_info.ColorAttachmentFormat = initInfo.colorFormat;

		ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* vulkan_instance) {
			return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name);
			}, &init_info.Instance);
		ImGui_ImplVulkan_Init(&init_info, initInfo.renderPass);
	}

	void VulkanImgui::SubmitDraws()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		// Invoke the draw listener callback
		if (m_DrawGUIListener)
			m_DrawGUIListener();
	}

	void VulkanImgui::Update()
	{
		for (auto& destroy : m_PendingDestroys)
			destroy();

		m_PendingDestroys.clear();
	}

	void VulkanImgui::Render(ResourceID commandBufferID)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		ImGui::Render();
		ImDrawData* main_draw_data = ImGui::GetDrawData();

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer->GetCommandBuffer());
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

	uint64_t VulkanImgui::AddTexture(ResourceID textureID)
	{
		auto texture = ResourceManager::GetResource<VulkanTexture>(textureID);
		auto image = ResourceManager::GetResource<VulkanImage>(texture->GetImage());
		auto sampler = ResourceManager::GetResource<VulkanTextureSampler>(texture->GetSampler());

		VkSampler samplerVk = sampler->GetSamplerVK();
		VkImageView view = image->GetImageView();
		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		return reinterpret_cast<uint64_t>(ImGui_ImplVulkan_AddTexture(samplerVk, view, layout));
	}

	uint64_t VulkanImgui::AddRenderTexture(ResourceID renderTextureID, ResourceID samplerID)
	{
		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(renderTextureID);

		Ref<VulkanImage> image;

		if (renderTarget->GetColorResolveTexture().IsValid())
		{
			Ref<VulkanTexture> texture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorResolveTexture());
			image = ResourceManager::GetResource<VulkanImage>(texture->GetImage());
		}
		else
		{
			Ref<VulkanTexture> texture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorTexture());
			image = ResourceManager::GetResource<VulkanImage>(texture->GetImage());
		}

		Ref<VulkanTextureSampler> sampler = ResourceManager::GetResource<VulkanTextureSampler>(samplerID);

		VkSampler samplerVk = sampler->GetSamplerVK();
		VkImageView view = image->GetImageView();
		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		return reinterpret_cast<uint64_t>(ImGui_ImplVulkan_AddTexture(samplerVk, view, layout));
	}

	void VulkanImgui::RemoveTexture(uint64_t id)
	{
		uint64_t cached = id;
		m_PendingDestroys.push_back(
			[this, cached]()
			{
				ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(cached));
			}
		);
	}

	void VulkanImgui::SetFont(Path fontFile, float fontSize)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(fontFile.string().c_str(), fontSize);
		UploadFont();
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
			Log::Error("(imgui 3)");
		}
	}

	void VulkanImgui::UploadFont()
	{
		// Use any command queue
		ResourceID commandPoolID = m_Context->GetGraphicsCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer->GetCommandBuffer());

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffer->GetCommandBufferRef();

		commandBuffer->EndCommands();

		VkResult err = vkQueueSubmit(m_Context->GetGraphicsQueueVK(), 1, &submitInfo, VK_NULL_HANDLE);

		if (!check_vk_result(err))
		{
			Log::Error("(imgui 1)");
		}

		err = vkDeviceWaitIdle(m_Context->GetDeviceVK());
		if (!check_vk_result(err))
		{
			Log::Error("(imgui 2)");
		}

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		commandPool->ReleaseBuffer(commandBufferID);
	}
}