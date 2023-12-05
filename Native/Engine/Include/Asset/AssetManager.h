#pragma once
#include "uuid_v4.h"
#include "AssetHandle.h"
#include "DynamicList.h"

namespace Odyssey
{
	class Mesh;
	class Shader;

	class AssetManager
	{
	public:
		static void CreateDatabase();

	public:
		static AssetHandle<Mesh> LoadMesh(const std::string& path);
		static AssetHandle<Shader> LoadShader(const std::string& path);

	private:
		static std::string GenerateUUID();
	private:
		inline static DynamicList<Mesh> m_Meshes;
		inline static DynamicList<Shader> m_Shaders;
		inline static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

		// Asset Database
		inline static std::unordered_map<std::string, std::filesystem::path> s_AssetDatabase;
	};
}