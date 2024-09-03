#pragma once
#include "ModelAssetImporter.hpp"

namespace tinygltf
{
	class Node;
	class Model;
	struct Skin;
}

namespace Odyssey
{
	class MyNode;

	using namespace tinygltf;

	class GLTFAssetImporter : public ModelAssetImporter
	{
	public:
		struct Settings
		{
		public:
			bool ConvertLH = true;
			bool LoggingEnabled = false;
		};
	public:
		GLTFAssetImporter();
		GLTFAssetImporter(Settings settings);

	public:
		virtual bool Import(const Path& modelPath) override;

	private:
		void LoadNode(MyNode* parent, const Node* node, uint32_t nodeIndex, const Model* model, float globalScale);
		void LoadMeshData(const Model* model);
		void LoadRigData(const Model* model);
		void BuildBoneMap(const Model* model, const Skin* skin, const Node* node, int32_t index, int32_t parentIndex, glm::mat4 parentTransform);
		void LoadAnimationData(const Model* model);

	private:
		Settings m_Settings;
		std::vector<MyNode*> m_Nodes;
	};
}