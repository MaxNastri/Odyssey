#pragma once

struct aiMesh;
struct aiNode;
struct aiScene;

namespace Odyssey
{
	class Mesh;
	class Material;

	struct ModelAsset
	{
		std::shared_ptr<Mesh> Mesh;
		std::shared_ptr<Material> Material;
	};

	class ModelLoader
	{
	public:
		ModelLoader() = default;

	public:
		bool LoadModel(const std::filesystem::path& assetPath, ModelAsset& outModel);
		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
	};
}