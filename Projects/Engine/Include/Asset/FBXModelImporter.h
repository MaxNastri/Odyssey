#pragma once
#include "Vertex.h"

namespace fbxsdk
{
	class FbxIOSettings;
	class FbxManager;
	class FbxMesh;
	class FbxNode;
	class FbxScene;
}

namespace Odyssey
{
	using namespace fbxsdk;


	class FBXModelImporter
	{
	public:
		struct MeshImportData
		{
			size_t ObjectCount;
			std::vector<size_t> HashIDs;
			std::vector<std::string> Names;
			std::vector<glm::mat4> WorldMatrices;
			std::vector<std::vector<Vertex>> VertexLists;
			std::vector<std::vector<uint32_t>> IndexLists;
		};

		struct FBXBone
		{
			FbxNode* Node;
			std::string Name;
			int32_t ParentIndex;
			int32_t Index;
			glm::mat4 bindpose;
			glm::mat4 inverseBindpose;
		};

		struct BoneInfluence
		{
			glm::vec4 Indices;
			glm::vec4 Weights;
		};

		struct RigData
		{
			std::vector<FBXBone> FBXBones;
			std::vector<BoneInfluence> ControlPointInfluences;
		};

	public:
		FBXModelImporter();

	public:
		void Import(const Path& filePath);

	public:
		const MeshImportData& GetMeshData() { return m_MeshImportData; }

	private:
		bool ValidateFile(const Path& filePath);
		void LoadMeshNodeData(FbxNode* node);
		void LoadMeshData(FbxNode* meshNode);
		void ProcessBoneHierarchy(FbxNode* sceneRoot);
		void ProcessBoneHierarchy(FbxNode* node, int32_t boneIndex, int32_t parentIndex);

	private:
		glm::vec3 GetNormal(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint);
		glm::vec3 GetTangent(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint);
		glm::vec2 GetTexcoord(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint, int32_t polygonPos);
		void GetBoneInfluences(FbxMesh* mesh);

	private:
		FbxManager* m_SDKManager;
		FbxIOSettings* m_Settings;
		FbxScene* m_CurrentScene;
		bool m_LoggingEnabled = false;

	private:
		MeshImportData m_MeshImportData;
		RigData m_RigData;
	};
}