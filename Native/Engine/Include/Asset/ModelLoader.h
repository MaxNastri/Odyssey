#pragma once
#include "AssetHandle.h"

struct aiMesh;
struct aiNode;
struct aiScene;

namespace Odyssey
{
	class Mesh;
	class Material;

	struct ModelAsset
	{
		AssetHandle<Mesh> Mesh;
		AssetHandle<Material> Material;
	};

	class ModelLoader
	{
	public:
		ModelLoader() = default;

	public:
		bool LoadModel(const std::filesystem::path& assetPath, ModelAsset& outModel);
		void ProcessNode(aiNode* node, const aiScene* scene);
		AssetHandle<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<AssetHandle<Mesh>> m_Meshes;
	};
}