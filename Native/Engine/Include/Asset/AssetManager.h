#pragma once
#include "AssetHandle.h"
#include "DynamicList.h"
#include "Mesh.h"

namespace Odyssey
{
	class AssetManager
	{
	public:
		static void Initialize();

	public:
		static AssetHandle<Mesh> AllocateMesh(std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices);
		static AssetHandle<Mesh> LoadMesh(std::string_view path);

	private:
		inline static DynamicList<Mesh> m_Meshes;
	};
}