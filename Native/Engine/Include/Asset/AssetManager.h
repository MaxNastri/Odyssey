#pragma once
#include "uuid_v4.h"
#include "AssetHandle.h"
#include "DynamicList.h"
#include "Enums.h"

namespace Odyssey
{
	class Material;
	class Mesh;
	class Shader;

	class AssetManager
	{
	public:
		static void CreateDatabase();

	public:
		static AssetHandle<Material> CreateMaterial(const std::string& assetPath);
		static AssetHandle<Mesh> CreateMesh(const std::string& assetPath);
		static AssetHandle<Shader> CreateShader(const std::string& assetPath);

	public:
		static AssetHandle<Material> LoadMaterial(const std::string& assetPath);
		static AssetHandle<Mesh> LoadMesh(const std::string& assetPath);
		static AssetHandle<Shader> LoadShader(const std::string& assetPath);

	public:
		static AssetHandle<Material> LoadMaterialByGUID(const std::string& guid);
		static AssetHandle<Mesh> LoadMeshByGUID(const std::string& guid);
		static AssetHandle<Shader> LoadShaderByGUID(const std::string& guid);

	private:
		static std::string GenerateGUID();

	private:
		inline static DynamicList<Mesh> s_Meshes;
		inline static DynamicList<Shader> s_Shaders;
		inline static DynamicList<Material> s_Materials;
		inline static UUIDv4::UUIDGenerator<std::mt19937_64> s_GUIDGenerator;

		// Asset Database
		inline static std::unordered_map<std::string, std::filesystem::path> s_AssetDatabase;
		inline static std::unordered_map<std::string, uint32_t> s_LoadedAssets;

		inline static AssetHandle<Shader> s_DefaultVertexShader;
		inline static AssetHandle<Shader> s_DefaultFragmentShader;

		inline static std::string s_DefaultVertexShaderPath = "Assets/Shaders/Vert.shader";
		inline static std::string s_DefaultFragmentShaderPath = "Assets/Shaders/Frag.shader";
	};
}