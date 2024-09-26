#pragma once
#include "AssetList.hpp"
#include "Enums.h"
#include "AssetDatabase.h"
#include "BinaryCache.h"
#include "GUID.h"
#include "TextureImportSettings.h"

namespace Odyssey
{
	class Asset;
	class AnimationRig;
	class Material;
	class Mesh;
	class Shader;
	class Scene;
	class Texture2D;
	class AnimationClip;
	class Cubemap;

	class SourceAsset;
	class SourceShader;
	class SourceModel;
	class SourceTexture;

	class AssetManager
	{
	public:
		static void CreateDatabase(const Path& assetsDirectory, const Path& cacheDirectory);

	public:
		static std::shared_ptr<SourceShader> CreateSourceShader(const Path& sourcePath);
		static std::shared_ptr<SourceModel> CreateSourceModel(const Path& sourcePath);

	public:
		static std::shared_ptr<Material> CreateMaterial(const Path& assetPath);
		static std::shared_ptr<Mesh> CreateMesh();
		static std::shared_ptr<Mesh> CreateMesh(const Path& assetPath);
		static std::shared_ptr<Mesh> CreateMesh(const Path& assetPath, std::shared_ptr<SourceModel> source);
		static std::shared_ptr<Shader> CreateShader(const Path& assetPath);
		static std::shared_ptr<Shader> CreateShader(const Path& assetPath, std::shared_ptr<SourceShader> source);
		static std::shared_ptr<Texture2D> CreateTexture2D(const Path& assetPath, std::shared_ptr<SourceTexture> sourceTexture);
		static std::shared_ptr<Cubemap> CreateCubemap(const Path& assetPath, TextureImportSettings& settings);
		static std::shared_ptr<AnimationRig> CreateAnimationRig(const Path& assetPath, std::shared_ptr<SourceModel> sourceModel);
		static std::shared_ptr<AnimationClip> CreateAnimationClip(const Path& assetPath, std::shared_ptr<SourceModel> sourceModel);
	public:
		static std::shared_ptr<SourceShader> LoadSourceShader(GUID guid);
		static std::shared_ptr<SourceModel> LoadSourceModel(GUID guid);
		static std::shared_ptr<SourceTexture> LoadSourceTexture(GUID guid);

	public:
		static std::shared_ptr<Material> LoadMaterial(const Path& assetPath);
		static std::shared_ptr<Mesh> LoadMesh(const Path& assetPath);
		static std::shared_ptr<Shader> LoadShader(const Path& assetPath);
		static std::shared_ptr<Texture2D> LoadTexture2D(const Path& assetPath);
		//static std::shared_ptr<Cubemap> LoadCubemap(const Path& assetPath);
	public:
		static std::shared_ptr<Material> LoadMaterialByGUID(GUID guid);
		static std::shared_ptr<Mesh> LoadMeshByGUID(GUID guid);
		static std::shared_ptr<Shader> LoadShaderByGUID(GUID guid);
		static std::shared_ptr<Texture2D> LoadTexture2DByGUID(GUID guid);
		static std::shared_ptr<Cubemap> LoadCubemap(GUID guid);
		static std::shared_ptr<AnimationRig> LoadAnimationRig(GUID guid);
		static std::shared_ptr<AnimationClip> LoadAnimationClip(GUID guid);

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
		inline static AssetList s_Assets;
		inline static std::set<GUID> s_LoadedAssets;

	private: // Source Assets
		inline static std::unique_ptr<AssetDatabase> s_SourceAssetDatabase;
		inline static SourceAssetList s_SourceAssets;
		inline static std::set<GUID> s_LoadedSourceAssets;

	private: // Binary Assets
		inline static std::unique_ptr<BinaryCache> s_BinaryCache;

	private: // Const
		inline static std::string s_AssetExtension = ".asset";
		inline static std::string s_MetaFileExtension = ".meta";

	};
}