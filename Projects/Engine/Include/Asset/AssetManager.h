#pragma once
#include "AssetHandle.h"
#include "DynamicList.h"
#include "Enums.h"
#include "AssetDatabase.h"
#include "BinaryCache.h"
#include "GUID.h"

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
	class SourceModel;

	class AssetManager
	{
	public:
		static void CreateDatabase(const Path& assetsDirectory, const Path& cacheDirectory);

	public:
		static AssetHandle<SourceShader> CreateSourceShader(const Path& sourcePath);
		static AssetHandle<SourceModel> CreateSourceModel(const Path& sourcePath);

	public:
		static AssetHandle<Material> CreateMaterial(const Path& assetPath);
		static AssetHandle<Mesh> CreateMesh();
		static AssetHandle<Mesh> CreateMesh(const Path& assetPath);
		static AssetHandle<Mesh> CreateMesh(const Path& assetPath, AssetHandle<SourceModel> source);
		static AssetHandle<Shader> CreateShader(const Path& assetPath);
		static AssetHandle<Shader> CreateShader(const Path& assetPath, AssetHandle<SourceShader> source);

	public:
		static AssetHandle<SourceShader> LoadSourceShader(GUID guid);
		static AssetHandle<SourceModel> LoadSourceModel(GUID guid);

	public:
		static AssetHandle<Material> LoadMaterial(const Path& assetPath);
		static AssetHandle<Mesh> LoadMesh(const Path& assetPath);
		static AssetHandle<Shader> LoadShader(const Path& assetPath);
		static AssetHandle<Texture2D> LoadTexture2D(const Path& assetPath);

	public:
		static AssetHandle<Material> LoadMaterialByGUID(GUID guid);
		static AssetHandle<Mesh> LoadMeshByGUID(GUID guid);
		static AssetHandle<Shader> LoadShaderByGUID(GUID guid);
		static AssetHandle<Texture2D> LoadTexture2DByGUID(GUID guid);

	public:
		static GUID CreateBinaryAsset(BinaryBuffer& buffer);
		static BinaryBuffer LoadBinaryAsset(GUID guid);
		static void WriteBinaryAsset(GUID guid, BinaryBuffer& buffer);

	public:
		static std::vector<GUID> GetAssetsOfType(const std::string& assetType);

	public:
		static GUID PathToGUID(const Path& path);
		static std::string GUIDToName(GUID guid);
		static std::string GUIDToAssetType(GUID guid);

	public:
		static bool IsSourceAsset(const Path& path);

	private: // Assets
		inline static Path s_AssetsDirectory;
		inline static std::unique_ptr<AssetDatabase> s_AssetDatabase;
		inline static DynamicList<Asset> s_Assets;
		inline static std::map<GUID, uint64_t> s_LoadedAssets;

	private: // Source Assets
		inline static std::unique_ptr<AssetDatabase> s_SourceAssetDatabase;
		inline static DynamicList<SourceAsset> s_SourceAssets;
		inline static std::map<GUID, uint64_t> s_LoadedSourceAssets;

	private: // Binary Assets
		inline static std::unique_ptr<BinaryCache> s_BinaryCache;

	private: // Const
		inline static std::string s_AssetExtension = ".asset";
		inline static std::string s_MetaFileExtension = ".meta";

	};
}