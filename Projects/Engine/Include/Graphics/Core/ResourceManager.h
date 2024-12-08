#pragma once
#include "Enums.h"
#include "FreeList.h"
#include "Ref.h"
#include "Resource.h"

namespace Odyssey
{
	class VulkanContext;

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

	public:
		template<typename T, typename... Args>
		static ResourceID Allocate(Args... args)
		{
			static_assert(std::is_base_of_v<Resource, T>, "T is not a dervied class of Resource.");
			ResourceID id = s_Resources.Peek();
			s_Resources.Add<T>(id, s_Context, std::forward<Args>(args)...);
			return id;
		}

		template<typename T>
		static Ref<T> GetResource(ResourceID resourceID)
		{
			
			static_assert(std::is_base_of_v<Resource, T>, "T is not a dervied class of Resource.");
			return s_Resources[resourceID].As<T>();
		}

		static void Destroy(ResourceID resourceID)
		{
			if (!resourceID.IsValid())
				return;

			auto func = [](ResourceID resourceID)
				{
					if (Ref<Resource> resource = s_Resources[resourceID])
					{
						resource->Destroy();
						s_Resources.Remove(resourceID);
					}
				};
			s_PendingDestroys.push_back({ resourceID, func });
		}

	public:
		static void Flush();
		static std::shared_ptr<VulkanContext> GetContext() { return s_Context; }

	private: // Vulkan members
		inline static std::shared_ptr<VulkanContext> s_Context = nullptr;
		inline static FreeList<Resource> s_Resources;

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