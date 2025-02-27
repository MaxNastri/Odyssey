#pragma once
#include "ModelAssetImporter.h"

namespace tinygltf
{
	class Node;
	class Model;
	struct Skin;
}

namespace Odyssey
{
	struct MyNode;

	using namespace tinygltf;

	class GLTFAssetImporter : public ModelAssetImporter
	{
	public:
		struct Settings
		{
		public:
			bool ConvertLH = true;
			bool LoggingEnabled = false;
			float Scale = 0.01f;
			bool BakeGlobalTransform = false;
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
		void BuildBoneMap(const Model* model, const Skin* skin, const Node* node, int32_t nodeIndex);
		void LoadAnimationData(const Model* model);
		void LoadMaterialData(const Model* model);

	private:
		Settings m_Settings;
		std::vector<MyNode*> m_Nodes;
	};
}