#pragma once
#include "uuid_v4.h"
#include "AssetHandle.h"
#include "DynamicList.h"
#include "Enums.h"
#include "AssetDatabase.h"

namespace Odyssey
{
	class Asset;
	class Material;
	class Mesh;
	class Shader;
	class Scene;
	class Texture2D;

	class AssetManager
	{
	public:
		static void CreateDatabase();

	public:
		static AssetHandle<Mesh> CreateMesh();
	public:
		static AssetHandle<Material> CreateMaterial(const std::filesystem::path& assetPath);
		static AssetHandle<Mesh> CreateMesh(const std::filesystem::path& assetPath);
		static AssetHandle<Shader> CreateShader(const std::filesystem::path& assetPath);
		static AssetHandle<Scene> CreateScene(const std::filesystem::path& assetPath);

	public:
		static AssetHandle<Material> LoadMaterial(const std::filesystem::path& assetPath);
		static AssetHandle<Mesh> LoadMesh(const std::filesystem::path& assetPath);
		static AssetHandle<Shader> LoadShader(const std::filesystem::path& assetPath);
		static AssetHandle<Scene> LoadScene(const std::filesystem::path& assetPath);
		static AssetHandle<Texture2D> LoadTexture2D(const std::filesystem::path& assetPath);

	public:
		static AssetHandle<Material> LoadMaterialByGUID(const std::string& guid);
		static AssetHandle<Mesh> LoadMeshByGUID(const std::string& guid);
		static AssetHandle<Shader> LoadShaderByGUID(const std::string& guid);
		static AssetHandle<Scene> LoadSceneByGUID(const std::string& guid);
		static AssetHandle<Texture2D> LoadTexture2DByGUID(const std::string& guid);

	public:
		static std::vector<std::string> GetAssetsOfType(const std::string& type);

	public:
		static void UnloadScene(AssetHandle<Scene> scene);

	public:
		static std::string PathToGUID(const std::filesystem::path& path);
		static std::string GUIDToName(const std::string& guid);

	private:
		static std::string GenerateGUID();
		static std::filesystem::path GenerateMetaPath(const std::filesystem::path& assetPath);

	private:
		inline static DynamicList<Asset> s_Assets;
		inline static UUIDv4::UUIDGenerator<std::mt19937_64> s_GUIDGenerator;

		inline static std::unordered_map<std::string, uint32_t> s_LoadedAssets;
		inline static AssetDatabase m_AssetDatabase;

		inline static AssetHandle<Shader> s_DefaultVertexShader;
		inline static AssetHandle<Shader> s_DefaultFragmentShader;

		inline static std::string s_DefaultVertexShaderPath = "Assets/Shaders/Vert.shader";
		inline static std::string s_DefaultFragmentShaderPath = "Assets/Shaders/Frag.shader";
	};
}