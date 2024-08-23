#pragma once
#include "Enums.h"
#include "VulkanTypes.h"
#include "ResourceList.hpp"
#include "Vertex.h"
#include "VulkanImage.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	class Resource;
	class VulkanBuffer;
	class VulkanUniformBuffer;
	class VulkanContext;
	class VulkanGraphicsPipeline;
	class VulkanIndexBuffer;
	class VulkanShaderModule;
	class VulkanRenderTexture;
	class VulkanVertexBuffer;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanDescriptorLayout;
	class VulkanImage;
	class VulkanTextureSampler;
	class VulkanDescriptorPool;
	class VulkanDescriptorSet;
	class VulkanTexture;

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

	public:
		template<typename T, typename... Args>
		static ResourceID Allocate(Args... args)
		{
			return s_Resources.Add<T>(s_Context, std::forward<Args>(args)...);
		}

		template<typename T>
		static std::shared_ptr<T> GetResource(ResourceID resourceID)
		{
			return s_Resources.Get<T>(resourceID);
		}

		static void Destroy(ResourceID resourceID)
		{
			if (!resourceID.IsValid())
				return;

			auto func = [](ResourceID resourceID)
				{
					if (std::shared_ptr<Resource> resource = s_Resources.Get(resourceID))
					{
						resource->Destroy();
						s_Resources.Remove(resourceID);
					}
				};
			s_PendingDestroys.push_back({ resourceID, func });
		}

	public:
		static void Flush();

	private: // Vulkan members
		inline static std::shared_ptr<VulkanContext> s_Context = nullptr;
		inline static ResourceList s_Resources;

	private:
		struct ResourceDeallocation
		{
			uint64_t ID;
			std::function<void(uint64_t)> Func;

			void Execute()
			{
				Func(ID);
			}
		};

		inline static std::vector<ResourceDeallocation> s_PendingDestroys;
	};

}