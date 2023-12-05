#pragma once
#include "uuid_v4.h"
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
		static AssetHandle<Mesh> LoadMesh(const std::string& path);

	private:
		static std::string GenerateUUID();
	private:
		inline static DynamicList<Mesh> m_Meshes;
		inline static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	};
}