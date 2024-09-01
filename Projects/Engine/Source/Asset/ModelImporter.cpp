#include "ModelImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

namespace Odyssey
{
	static const uint32_t s_AnimationImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |        // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
		aiProcess_GlobalScale |             // e.g. convert cm to m for fbx import (and other formats where cm is native)
		aiProcess_ValidateDataStructure;    // Validation 

	namespace Utils
	{
		inline static glm::mat4x4 AssimpToGLM(aiMatrix4x4 mat)
		{
			return glm::mat4x4(
				(float)mat.a1, (float)mat.b1, (float)mat.c1, (float)mat.d1,
				(float)mat.a2, (float)mat.b2, (float)mat.c2, (float)mat.d2,
				(float)mat.a3, (float)mat.b3, (float)mat.c3, (float)mat.d3,
				(float)mat.a4, (float)mat.b4, (float)mat.c4, (float)mat.d4
			);
		}

		inline static glm::vec3 AssimpToGLM(aiVector3D vec)
		{
			return glm::vec3(vec.x, vec.y, vec.z);
		}

		inline static glm::quat AssimpToGLM(aiQuaternion quat)
		{
			return glm::quat(quat.w, quat.x, quat.y, quat.z);
		}
	}
	class BoneHierarchy
	{
	public:
		BoneHierarchy(const aiScene* scene, MeshImportData& meshImportData) : m_MeshImportData(meshImportData) { m_Scene = scene; }

	public:
		void ExtractBones()
		{
			for (uint32_t meshIndex = 0; meshIndex < m_Scene->mNumMeshes; ++meshIndex)
			{
				const aiMesh* mesh = m_Scene->mMeshes[meshIndex];
				for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
				{
					std::string_view boneName = mesh->mBones[boneIndex]->mName.C_Str();
					m_Bones.emplace(boneName);
					m_BoneMap[boneName] = mesh->mBones[boneIndex];
				}
			}

			// Extract any nodes that are animated (but don't have any skin bound to them)
			//for (uint32_t animationIndex = 0; animationIndex < m_Scene->mNumAnimations; ++animationIndex)
			//{
			//	const aiAnimation* animation = m_Scene->mAnimations[animationIndex];
			//	for (uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
			//	{
			//		const aiNodeAnim* nodeAnim = animation->mChannels[channelIndex];
			//		m_Bones.emplace(nodeAnim->mNodeName.C_Str());
			//	}
			//}
		}

		void TraverseNode(aiNode* node, const glm::mat4& parentTransform = glm::identity<glm::mat4>())
		{
			if (m_Bones.find(node->mName.C_Str()) != m_Bones.end())
			{
				m_ImportData.GlobalTransform = parentTransform;

				aiNode* parent = node->mParent;
				while (parent)
				{
					// Reset the parent node transform to empty
					parent->mTransformation = aiMatrix4x4();
					parent = parent->mParent;
				}

				// Build the bone catalog
				TraverseBone(node, -1, parentTransform);
			}
			else
			{
				auto transform = parentTransform * Utils::AssimpToGLM(node->mTransformation);
				for (uint32_t nodeIndex = 0; nodeIndex < node->mNumChildren; ++nodeIndex)
				{
					TraverseNode(node->mChildren[nodeIndex], transform);
				}
			}
		}

		void TraverseBone(aiNode* node, int32_t parentIndex, glm::mat4 parentGlobalTransform)
		{
			std::string boneName = node->mName.C_Str();

			Bone bone;
			bone.Name = boneName;
			bone.ParentIndex = parentIndex;
			bone.Index = (int32_t)(m_ImportData.m_BoneMap.size());
			bone.InverseBindpose = Utils::AssimpToGLM(m_BoneMap[boneName]->mOffsetMatrix);
			//bone.Transform = Utils::AssimpToGLM(node->mTransformation);

			//parentGlobalTransform = parentGlobalTransform * bone.Transform;

			// Iterate through the bone's weights and indices
			for (uint32_t j = 0; j < m_BoneMap[boneName]->mNumWeights; j++)
			{
				// Get the vertex ID of this weight
				uint32_t vertexID = m_BoneMap[boneName]->mWeights[j].mVertexId;
				float weight = m_BoneMap[boneName]->mWeights[j].mWeight;

				// Apply the weight to the vertex
				if (weight > 0.0f)
					m_MeshImportData.SetBoneInfluence(vertexID, bone.Index, weight);
			}

			m_ImportData.m_BoneMap[boneName] = bone;

			for (uint32_t nodeIndex = 0; nodeIndex < node->mNumChildren; ++nodeIndex)
			{
				if (m_Bones.find(node->mChildren[nodeIndex]->mName.C_Str()) != m_Bones.end())
				{
					TraverseBone(node->mChildren[nodeIndex], bone.Index, parentGlobalTransform);
				}
				else
				{
					// do not traverse any further.
					// It is not supported to have a non-bone and then more bones below it.
				}
			}
		}

		RigImportData CreateSkeleton()
		{
			ExtractBones();
			TraverseNode(m_Scene->mRootNode);
			return m_ImportData;
		}

	private:
		std::set<std::string_view> m_Bones;
		std::map<std::string_view, aiBone*> m_BoneMap;
		RigImportData m_ImportData;
		MeshImportData& m_MeshImportData;
		const aiScene* m_Scene;
	};

	void ModelImporter::Import(const Path& modelPath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(modelPath.string(), s_AnimationImportFlags);

		if (scene->HasMeshes())
		{
			// TODO: Add support for submeshes
			m_MeshCount = scene->mNumMeshes;

			auto mesh = scene->mMeshes[0];
			ImportMesh(mesh);

			// Bone Hierarchy / Rig
			if (mesh->HasBones())
			{
				NormalizeMeshWeights(mesh);
				ImportRig(scene);
			}

			// TODO: Add support for multiple clips
			if (scene->HasAnimations())
				ImportAnimationClip(scene->mAnimations[0]);
		}
	}

	void ModelImporter::NormalizeMeshWeights(const aiMesh* mesh)
	{
		if (mesh->mNumBones == 0)
			return;

		struct BoneWeight {
			uint32_t mBoneIndex; // index of a bone in current mesh
			aiVertexWeight* mVertexWeight; // a pointer to mVertexWeight in meshs[x]->mBones[x]->mWeight for quick visit
		};

		struct VertexBoneWeights {
			float mTotalWeight;
			std::vector<BoneWeight> mBoneWeights;
		};

		std::map<uint32_t, VertexBoneWeights> map;

		for (uint32_t b = 0; b < mesh->mNumBones; b++)
		{
			auto bone = mesh->mBones[b];

			for (uint32_t w = 0; w < bone->mNumWeights; w++)
			{
				auto vertexWeight = &bone->mWeights[w];
				auto key = vertexWeight->mVertexId;

				auto& vertex_BoneWeights = map[key];

				BoneWeight boneWeights;
				boneWeights.mBoneIndex = b;
				boneWeights.mVertexWeight = vertexWeight;

				vertex_BoneWeights.mTotalWeight += vertexWeight->mWeight;
				vertex_BoneWeights.mBoneWeights.push_back(boneWeights);
			}
		}

		uint32_t count = 0;
		// normalize all weights: 
		// every weight for a same vertex divided by totalWeight of this vertex 
		for (auto& item : map)
		{
			auto& vertex_BoneWeights = item.second;
			auto f = 1.0 / vertex_BoneWeights.mTotalWeight;
			if (f < 1.0)
				int debug = 0;
			for (uint32_t i = 0; i < vertex_BoneWeights.mBoneWeights.size(); i++)
			{
				vertex_BoneWeights.mBoneWeights[i].mVertexWeight->mWeight *= f;
				count++;
			}
		}
	}

	void ModelImporter::ImportMesh(const aiMesh* mesh)
	{
		// Add to the vertex count
		m_MeshData.VertexCount += mesh->mNumVertices;

		// Add to the normals count
		if (mesh->HasNormals())
			m_MeshData.NormalsCount += mesh->mNumVertices;

		// Track the uv channel count
		m_MeshData.UVChannelCount = mesh->GetNumUVChannels();

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->HasNormals())
				vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

			// TODO: Add support for 8 texcoord channels
			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord0 = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				m_MeshData.UVChannels.push_back(0);
			}

			// TODO: Add support for up to 8 colors?
			if (mesh->HasVertexColors(0))
			{
				auto color = mesh->mColors[0][i];
				vertex.Color = glm::vec4(color.r, color.g, color.b, color.a);
			}

			m_MeshData.Vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			// Add to the index count
			m_MeshData.IndexCount += face.mNumIndices;

			for (uint32_t index = 0; index < face.mNumIndices; index++)
			{
				m_MeshData.Indices.push_back(face.mIndices[index]);
			}
		}
	}

	void ModelImporter::ImportRig(const aiScene* scene)
	{
		BoneHierarchy boneHierarchy(scene, m_MeshData);
		m_RigData = boneHierarchy.CreateSkeleton();
	}

	void ModelImporter::ImportAnimationClip(const aiAnimation* animation)
	{
		m_AnimationData.Name = animation->mName.C_Str();
		m_AnimationData.Duration = animation->mDuration / animation->mTicksPerSecond;

		// For each bone
		for (uint32_t boneIndex = 0; boneIndex < animation->mNumChannels; boneIndex++)
		{
			// Get the keys stored for the bone
			auto boneKeys = animation->mChannels[boneIndex];

			// Get our keyframe for this bone
			std::string boneName = boneKeys->mNodeName.C_Str();
			auto& boneKeyframe = m_AnimationData.BoneKeyframes[boneName];
			boneKeyframe.SetBoneName(boneName);

			// Store the position keys
			for (uint32_t i = 0; i < boneKeys->mNumPositionKeys; i++)
			{
				auto positionKey = boneKeys->mPositionKeys[i];
				boneKeyframe.AddPositionKey(positionKey.mTime / animation->mTicksPerSecond, Utils::AssimpToGLM(positionKey.mValue));
			}

			// Store the rotation keys
			for (uint32_t i = 0; i < boneKeys->mNumRotationKeys; i++)
			{
				auto rotationKey = boneKeys->mRotationKeys[i];
				boneKeyframe.AddRotationKey(rotationKey.mTime / animation->mTicksPerSecond, Utils::AssimpToGLM(rotationKey.mValue));
			}

			// Store the scale keys
			for (uint32_t i = 0; i < boneKeys->mNumScalingKeys; i++)
			{
				auto scaleKeys = boneKeys->mScalingKeys[i];
				boneKeyframe.AddScaleKey(scaleKeys.mTime / animation->mTicksPerSecond, Utils::AssimpToGLM(scaleKeys.mValue));
			}
		}
	}

}