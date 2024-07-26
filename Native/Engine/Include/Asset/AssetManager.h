#pragma once
#include "uuid_v4.h"
#include "AssetHandle.h"
#include "DynamicList.h"
#include "Enums.h"
#include "AssetDatabase.h"
#include "BinaryCache.h"

namespace Odyssey
{
	class Asset;
	class Material;
	class Mesh;
	class Shader;
	class Scene;
	class Texture2D;

	class SourceAsset;
	class SourceShader;

	class AssetManager
	{
	public:
		static void CreateDatabase();

	public:
		static AssetHandle<SourceShader> CreateSourceShader(const std::filesystem::path& sourcePath);
	public:
		static AssetHandle<Material> CreateMaterial(const std::filesystem::path& assetPath);
		static AssetHandle<Mesh> CreateMesh();
		static AssetHandle<Mesh> CreateMesh(const std::filesystem::path& assetPath);
		static AssetHandle<Shader> CreateShader(const std::filesystem::path& assetPath);
		static AssetHandle<Scene> CreateScene(const std::filesystem::path& assetPath);

	public:
		static AssetHandle<SourceShader> LoadSourceShader(const std::string& guid);

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
		static BinaryCache& GetBinaryCache() { return s_BinaryCache; }
		static BinaryBuffer LoadBinaryAsset(const std::string& guid);
		static std::string CreateBinaryAsset(BinaryBuffer& buffer);

	public:
		static std::vector<std::string> GetAssetsOfType(const std::string& assetType);

	public:
		static void UnloadScene(AssetHandle<Scene> scene);

	public:
		static std::string PathToGUID(const std::filesystem::path& path);
		static std::string GUIDToName(const std::string& guid);
		static std::string GUIDToAssetType(const std::string& guid);

	private:
		static std::string GenerateGUID();
		static void ScanForSourceAssets();

	private: // Assets
		inline static AssetDatabase s_AssetDatabase;
		inline static DynamicList<Asset> s_Assets;
		inline static std::map<std::string, uint32_t> s_LoadedAssets;

	private: // Source Assets
		inline static AssetDatabase s_SourceAssetDatabase;
		inline static DynamicList<SourceAsset> s_SourceAssets;
		inline static std::map<std::string, uint32_t> s_LoadedSourceAssets;

	private: // Binary Assets
		inline static BinaryCache s_BinaryCache;

	private:
		inline static UUIDv4::UUIDGenerator<std::mt19937_64> s_GUIDGenerator;
		inline static AssetHandle<Shader> s_DefaultVertexShader;
		inline static AssetHandle<Shader> s_DefaultFragmentShader;

	private: // Const
		inline static std::string s_AssetExtension = ".asset";
		inline static std::string s_SceneExtension = ".scene";
		inline static std::string s_MetaFileExtension = ".meta";

		inline static std::map<std::string, std::string> s_SourceAssetExtensionsToType =
		{
			{".glsl", "SourceShader"},
			{".hlsl", "SourceShader"},
			{".fbx", "SourceMesh"},
			{".gltf", "SourceMesh"},
			{".png", "SourceTexture"},
			{".jpg", "SourceTexture"},
		};

		inline static std::string s_DefaultVertexShaderPath = "Assets/Shaders/Vert.shader";
		inline static std::string s_DefaultFragmentShaderPath = "Assets/Shaders/Frag.shader";
	};
}