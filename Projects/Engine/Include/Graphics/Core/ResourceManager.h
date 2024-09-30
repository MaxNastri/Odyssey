#pragma once
#include "Enums.h"
#include "Resource.h"
#include "FreeList.hpp"

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
			static_assert(std::is_base_of<Resource, T>::value, "T is not a dervied class of Resource.");
			ResourceID id = s_Resources.Peek();
			s_Resources.Add<T>(id, s_Context, std::forward<Args>(args)...);
			return id;
		}

		template<typename T>
		static std::shared_ptr<T> GetResource(ResourceID resourceID)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T is not a dervied class of Resource.");
			return std::static_pointer_cast<T>(s_Resources[resourceID]);
		}

		static void Destroy(ResourceID resourceID)
		{
			if (!resourceID.IsValid())
				return;

			auto func = [](ResourceID resourceID)
				{
					if (std::shared_ptr<Resource> resource = s_Resources[resourceID])
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