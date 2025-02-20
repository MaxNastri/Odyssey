#include "GLTFAssetImporter.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_CPP14
#include "tiny_gltf.h"
#include "Log.h"
#include "Vertex.h"
#include "GeometryUtil.h"

namespace Odyssey
{
	using namespace tinygltf;

	GLTFAssetImporter::GLTFAssetImporter()
	{
		m_Settings.ConvertLH = true;
		m_Settings.LoggingEnabled = false;
	}

	GLTFAssetImporter::GLTFAssetImporter(Settings settings)
		: m_Settings(settings)
	{
	}

	bool GLTFAssetImporter::Import(const Path& filePath)
	{
		const Path& extension = filePath.extension();
		assert(extension == ".glb" || extension == ".gltf");

		Model model;
		TinyGLTF context;
		std::string error;
		std::string warning;

		bool status = false;

		// Note: We set a custom (empty) function as the image loader to save performance
		// We handle image loading ourselves outside of gltf importing
		LoadImageDataFunction func = [this](
			Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*) { return true; };
		context.SetImageLoader(func, nullptr);

		if (extension == ".glb")
			status = context.LoadBinaryFromFile(&model, &error, &warning, filePath.string());
		else
			status = context.LoadASCIIFromFile(&model, &error, &warning, filePath.string());

		if (!warning.empty())
		{
			Log::Warning(std::format("Warning detected while importing GLTF asset: {}", filePath.string()));
			Log::Warning(warning);
		}

		if (!error.empty())
		{
			Log::Warning(std::format("Error detected while importing GLTF asset: {}", filePath.string()));
			Log::Warning(error);
			return false;
		}

		LoadMeshData(&model);
		LoadRigData(&model);
		LoadAnimationData(&model);
		return true;
	}

	struct AttributeData
	{
	public:
		bool IsValid() { return BufferView && Accessor && Buffer && Stride; }
		template<typename T>
		const T* GetData(size_t index)
		{
			const T* castBuffer = static_cast<const T*>(Buffer);
			return &(castBuffer[index * Stride]);
		}

	public:
		std::string Name;
		const BufferView* BufferView = nullptr;
		const Accessor* Accessor = nullptr;
		const void* Buffer = nullptr;
		int32_t ComponentType;
		uint32_t Stride = 0;
	};

	AttributeData GetAttributeData(const Model* model, const Primitive* primitive, const std::string& attributeName, int32_t expectedType)
	{
		AttributeData data;
		data.Name = attributeName;

		if (primitive->attributes.contains(attributeName))
		{
			// Get the position data accessor
			data.Accessor = &(model->accessors[primitive->attributes.find(data.Name)->second]);
			// Get the buffer view into the position data buffer
			data.BufferView = &(model->bufferViews[data.Accessor->bufferView]);
			// Get the buffer location of the position data
			data.Buffer = &(model->buffers[data.BufferView->buffer].data[data.Accessor->byteOffset + data.BufferView->byteOffset]);
			// Get the stride of the attribute data type
			data.ComponentType = data.Accessor->componentType;
			data.Stride = data.Accessor->ByteStride(*(data.BufferView)) ? (data.Accessor->ByteStride(*(data.BufferView)) / tinygltf::GetComponentSizeInBytes(data.ComponentType)) : tinygltf::GetNumComponentsInType(expectedType);
		}

		return data;
	}

	struct MySkin
	{
		std::string name;
		std::vector<glm::mat4> inverseBindposes;
		std::vector<MyNode*> joints;
	};

	struct MyNode
	{
		MyNode* parent;
		int32_t index;
		std::vector<MyNode*> children;
		glm::mat4 matrix;
		std::string name;
		Mesh* mesh;
		Skin* skin;
		int32_t skinIndex = -1;
		glm::vec3 translation;
		glm::vec3 scale{ 1.0f };
		glm::quat rotation;

	public:
		glm::mat4 LocalMatrix()
		{
			return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
		}
		glm::mat4 GlobalMatrix()
		{
			glm::mat4 mat = LocalMatrix();
			MyNode* p = parent;
			while (p)
			{
				mat = p->LocalMatrix() * mat;
				p = p->parent;
			}
			return mat;
		}
	};

	void GLTFAssetImporter::LoadNode(MyNode* parent, const Node* node, uint32_t nodeIndex, const Model* model, float globalScale)
	{
		MyNode* newNode = new MyNode{};
		newNode->index = nodeIndex;
		newNode->parent = parent;
		newNode->name = node->name;
		newNode->skinIndex = node->skin;
		newNode->matrix = glm::mat4(1.0f);

		// Generate local node matrix
		glm::vec3 translation = glm::vec3(0.0f);
		if (node->translation.size() == 3) {
			translation = glm::make_vec3(node->translation.data());
			newNode->translation = translation;
		}
		glm::mat4 rotation = glm::mat4(1.0f);
		if (node->rotation.size() == 4) {
			glm::quat q = glm::make_quat(node->rotation.data());
			newNode->rotation = glm::mat4(q);
		}
		glm::vec3 scale = glm::vec3(1.0f);
		if (node->scale.size() == 3) {
			scale = glm::make_vec3(node->scale.data());
			newNode->scale = scale;
		}
		if (node->matrix.size() == 16) {
			newNode->matrix = glm::make_mat4x4(node->matrix.data());
		};

		// Node with children
		if (node->children.size() > 0) {
			for (size_t i = 0; i < node->children.size(); i++) {
				LoadNode(newNode, &model->nodes[node->children[i]], node->children[i], model, globalScale);
			}
		}

		if (node->mesh > -1)
		{
			LoadMeshData(model);
		}

		if (parent)
			parent->children.push_back(newNode);
		else
			m_Nodes.push_back(newNode);
	}
	struct TempNode
	{
		const tinygltf::Node* node;
		TempNode* parent;
		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 scale;
		glm::mat4 mat;

	public:
		void Construct()
		{
			mat4x4 t = glm::translate(glm::identity<mat4x4>(), pos);
			mat4x4 r = glm::toMat4(rot);
			mat4x4 s = glm::scale(glm::identity<mat4x4>(), scale);
			mat = t * r * s;
		}
		glm::mat4 GetWorld()
		{
			Construct();

			glm::mat world = mat;
			TempNode* lParent = parent;

			while (lParent)
			{
				lParent->Construct();
				world = lParent->mat * world;
				lParent = lParent->parent;
			}

			return world;
		}
	};

	inline static std::vector<TempNode*> tempNodes;

	void CreateNodeRecursive(const Model* model, const tinygltf::Node* node, TempNode* parent)
	{
		TempNode* tempNode = new TempNode();
		tempNode->node = node;
		tempNode->parent = parent;
		tempNode->mat = glm::mat4(1.0f);
		parent = tempNode;

		tempNodes.push_back(tempNode);

		glm::vec3 translation = glm::vec3(0.0f);
		if (node->translation.size() == 3)
		{
			translation = glm::make_vec3(node->translation.data());
			tempNode->pos = translation;
		}
		glm::quat rotation = glm::mat4(1.0f);
		if (node->rotation.size() == 4) {
			rotation = glm::make_quat(node->rotation.data());
		}
		tempNode->rot = rotation;
		glm::vec3 scale = glm::vec3(1.0f);
		if (node->scale.size() == 3) {
			scale = glm::make_vec3(node->scale.data());
		}
		tempNode->scale = scale;
		if (node->matrix.size() == 16) {
			tempNode->mat = glm::make_mat4x4(node->matrix.data());
		};

		// Node with children
		if (node->children.size() > 0)
		{
			for (size_t i = 0; i < node->children.size(); i++)
			{
				CreateNodeRecursive(model, &model->nodes[node->children[i]], tempNode);
			}
		}
	}

	void GLTFAssetImporter::LoadMeshData(const Model* model)
	{
		const Scene& scene = model->scenes[model->defaultScene ? model->defaultScene : 0];

		TempNode* parent = nullptr;

		for (auto node : scene.nodes)
		{
			CreateNodeRecursive(model, &model->nodes[node], nullptr);
		}

		m_MeshDatas.clear();

		for (size_t m = 0; m < model->meshes.size(); m++)
		{
			const Mesh& mesh = model->meshes[m];
			MeshImportData& meshData = m_MeshDatas.emplace_back();
			meshData.Name = mesh.name;

			for (size_t p = 0; p < mesh.primitives.size(); p++)
			{
				const Primitive& primitive = mesh.primitives[p];
				SubmeshImportData& submeshData = meshData.Submeshes.emplace_back();

				// Vertices
				{
					AttributeData positionData = GetAttributeData(model, &primitive, "POSITION", TINYGLTF_TYPE_VEC3);
					AttributeData normalData = GetAttributeData(model, &primitive, "NORMAL", TINYGLTF_TYPE_VEC3);
					AttributeData colorData = GetAttributeData(model, &primitive, "COLOR_0", TINYGLTF_TYPE_VEC3);
					AttributeData texCoord0Data = GetAttributeData(model, &primitive, "TEXCOORD_0", TINYGLTF_TYPE_VEC2);
					AttributeData weightsData = GetAttributeData(model, &primitive, "WEIGHTS_0", TINYGLTF_TYPE_VEC4);
					AttributeData indicesData = GetAttributeData(model, &primitive, "JOINTS_0", TINYGLTF_TYPE_VEC4);

					uint32_t vertexCount = (uint32_t)positionData.Accessor->count;
					uint32_t vertexStart = 0;

					submeshData.Vertices.resize(vertexCount);

					for (size_t v = 0; v < vertexCount; v++)
					{
						Vertex& vertex = submeshData.Vertices[v];

						vertex.Position = glm::make_vec3(positionData.GetData<float>(v));
						vertex.Normal = normalData.IsValid() ? glm::make_vec3(normalData.GetData<float>(v)) : glm::vec3(0.0f);
						vertex.Color = colorData.IsValid() ? glm::vec4(glm::make_vec3(colorData.GetData<float>(v)), 1.0f) : glm::vec4(1.0f);

						vertex.TexCoord0 = texCoord0Data.IsValid() ? glm::make_vec2(texCoord0Data.GetData<float>(v)) : glm::vec2(0.0f);

						vertex.BoneWeights = weightsData.IsValid() ? glm::make_vec4(weightsData.GetData<float>(v)) : glm::vec4(0.0f);
						if (glm::length(vertex.BoneWeights) == 0.0f)
							vertex.BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

						// IMPORTANT: Flip the z component to convert from RH to LH
						// IMPORTANT: We flip the UV to convert RH to LH
						if (m_Settings.ConvertLH)
						{
							vertex.Position.z = -vertex.Position.z;
							vertex.Normal.z = -vertex.Normal.z;
							//vertex.TexCoord0.y = 1.0f - vertex.TexCoord0.y;
						}

						if (indicesData.IsValid())
						{
							switch (indicesData.ComponentType)
							{
								case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
								{
									vertex.BoneIndices = glm::uvec4(glm::make_vec4(indicesData.GetData<uint16_t>(v)));
									break;
								}
								case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
								{
									vertex.BoneIndices = glm::make_vec4(indicesData.GetData<uint8_t>(v));
									break;
								}
							}
						}
						else
						{
							vertex.BoneIndices = glm::vec4(0.0f);
						}
						vertexStart++;
					}
				}

				// Indices
				const tinygltf::Accessor& accessor = model->accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];
				const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

				uint32_t indexCount = static_cast<uint32_t>(accessor.count);

				uint32_t indexStart = 0;
				submeshData.Indices.resize(indexCount);

				switch (accessor.componentType)
				{
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
					{
						const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							submeshData.Indices[indexStart] = buf[index];
							++indexStart;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
					{
						const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							submeshData.Indices[indexStart] = buf[index];
							++indexStart;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
					{
						const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);

						for (size_t index = 0; index < accessor.count; index++)
						{
							submeshData.Indices[indexStart] = buf[index];
							++indexStart;
						}
						break;
					}
				}

				// IMPORTANT: We reverse the winding order to convert RH to LH coord system
				if (m_Settings.ConvertLH)
					std::reverse(submeshData.Indices.begin(), submeshData.Indices.end());

				GeometryUtil::GenerateTangents(submeshData.Vertices, submeshData.Indices);
			}
		}
	}

	glm::mat4 ToGLM(std::vector<double> matrix)
	{
		return glm::mat4(
			(float)matrix[0], (float)matrix[1], (float)matrix[2], (float)matrix[3],
			(float)matrix[4], (float)matrix[5], (float)matrix[6], (float)matrix[7],
			(float)matrix[8], (float)matrix[9], (float)matrix[10], (float)matrix[11],
			(float)matrix[12], (float)matrix[13], (float)matrix[14], (float)matrix[15]
		);
	}

	glm::mat4 GetMatrix(const Node* node)
	{
		if (node->matrix.size() == 0)
		{
			glm::vec3 translation = glm::vec3((float)node->translation[0], (float)node->translation[1], (float)node->translation[2]);
			glm::quat rotation = glm::quat((float)node->rotation[0], (float)node->rotation[1], (float)node->rotation[2], (float)node->rotation[3]);

			return glm::translate(glm::identity<mat4>(), translation) * glm::toMat4(rotation);
		}
		else
		{
			return ToGLM(node->matrix);
		}
	}

	glm::mat4 ConvertLH(glm::mat4 mat)
	{
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(mat, scale, rotation, translation, skew, perspective);

		translation.z = -translation.z;
		rotation.x = -rotation.x;
		rotation.y = -rotation.y;

		return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
	}

	void GLTFAssetImporter::LoadRigData(const Model* model)
	{
		m_RigData.ScaleOffset = glm::scale(mat4(1.0f), glm::vec3(m_Settings.Scale));

		if (m_Settings.ConvertLH)
		{
			m_RigData.RotationOffset = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
		}

		for (size_t s = 0; s < model->skins.size(); s++)
		{
			const Skin* skin = &model->skins[s];
			int32_t rootIndex = skin->skeleton;

			for (int32_t nodeIndex : skin->joints)
			{
				const Node* node = &model->nodes[nodeIndex];

				BuildBoneMap(model, skin, node, nodeIndex);
			}

			if (m_RigData.Bones.size() > 0)
			{
				const Accessor& accessor = model->accessors[skin->inverseBindMatrices];
				const BufferView& bufferView = model->bufferViews[accessor.bufferView];
				const Buffer& buffer = model->buffers[bufferView.buffer];

				std::vector<glm::mat4> inverseBindposes;
				inverseBindposes.resize(accessor.count);

				memcpy(inverseBindposes.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));

				for (auto& [boneName, bone] : m_RigData.Bones)
				{
					if (m_Settings.ConvertLH)
						bone.inverseBindpose = ConvertLH(inverseBindposes[bone.Index]);
					else
						bone.inverseBindpose = inverseBindposes[bone.Index];
				}
			}
		}
	}

	void GLTFAssetImporter::BuildBoneMap(const Model* model, const Skin* skin, const Node* node, int32_t nodeIndex)
	{
		// Make sure this is a valid bone index
		bool valid = false;
		for (auto boneIndex : skin->joints)
		{
			if (boneIndex == nodeIndex)
			{
				valid = true;
				break;
			}
		}

		if (valid)
		{
			FBXBone& bone = m_RigData.Bones[node->name];
			bone.Name = node->name;
			bone.Index = (int32_t)m_RigData.BoneCount;
			bone.NodeIndex = nodeIndex;
			m_RigData.BoneCount++;

			// Iterate through this node's children and set only their parent index if they are a valid bone
			// Note: They will have their own bone index + name set later since we are iterating through all bones
			for (const auto& child : node->children)
			{
				bool childValid = false;
				for (auto boneIndex : skin->joints)
				{
					if (boneIndex == nodeIndex)
					{
						childValid = true;
						break;
					}
				}

				if (childValid)
				{
					Node childNode = model->nodes[child];
					auto& childBone = m_RigData.Bones[childNode.name];
					childBone.ParentIndex = bone.Index;
				}
			}
		}
	}

	struct AnimationSampler
	{
		std::vector<float> inputs;
		std::vector<glm::vec4> outputsVec4;
		std::vector<float> outputs;
	};

	struct AnimationChannel
	{
		int32_t TargetNODE = -1;
		int32_t SamplerIndex = -1;
		bool IsPosition = false;
		bool IsRotation = false;
		bool IsScale = false;
	};
	void GLTFAssetImporter::LoadAnimationData(const Model* model)
	{
		AnimationImportData& animationData = m_AnimationData.emplace_back();
		animationData.FramesPerSecond = 30;

		for (const Animation& anim : model->animations)
		{
			std::string name = anim.name;

			std::vector<AnimationSampler> samplers;
			std::vector<AnimationChannel> channels;

			for (auto& sampler : anim.samplers)
			{
				AnimationSampler customSampler;
				// Custom interpolation not supported
				//if (sampler.interpolation == "LINEAR") {}
				//if (sampler.interpolation == "STEP") {}
				//if (sampler.interpolation == "CUBICSPLINE") {}

				// Read sampler input time values
				{
					const tinygltf::Accessor& accessor = model->accessors[sampler.input];
					const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

					assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

					const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
					const float* buf = static_cast<const float*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						customSampler.inputs.push_back(buf[index]);
					}

					for (auto input : customSampler.inputs)
					{
						if (input < animationData.Start)
							animationData.Start = input;
						if (input > animationData.Duration)
							animationData.Duration = input;
					}
				}

				// Read sampler output T/R/S values
				{
					const tinygltf::Accessor& accessor = model->accessors[sampler.output];
					const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

					assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

					const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

					switch (accessor.type) {
						case TINYGLTF_TYPE_VEC3: {
							const glm::vec3* buf = static_cast<const glm::vec3*>(dataPtr);
							for (size_t index = 0; index < accessor.count; index++) {
								customSampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));
								customSampler.outputs.push_back(buf[index][0]);
								customSampler.outputs.push_back(buf[index][1]);
								customSampler.outputs.push_back(buf[index][2]);
							}
							break;
						}
						case TINYGLTF_TYPE_VEC4: {
							const glm::vec4* buf = static_cast<const glm::vec4*>(dataPtr);
							for (size_t index = 0; index < accessor.count; index++) {
								customSampler.outputsVec4.push_back(buf[index]);
								customSampler.outputs.push_back(buf[index][0]);
								customSampler.outputs.push_back(buf[index][1]);
								customSampler.outputs.push_back(buf[index][2]);
								customSampler.outputs.push_back(buf[index][3]);
							}
							break;
						}
						default: {
							std::cout << "unknown type" << std::endl;
							break;
						}
					}
				}

				samplers.push_back(customSampler);
			}

			for (auto& source : anim.channels)
			{
				AnimationChannel myChannel;

				if (source.target_path == "rotation")
					myChannel.IsRotation = true;
				if (source.target_path == "translation")
					myChannel.IsPosition = true;
				if (source.target_path == "scale")
					myChannel.IsScale = true;
				if (source.target_path == "weights")
				{
					std::cout << "weights not yet supported, skipping channel" << std::endl;
					continue;
				}
				myChannel.SamplerIndex = source.sampler;
				myChannel.TargetNODE = source.target_node;

				if (myChannel.TargetNODE == -1)
					continue;

				channels.push_back(myChannel);
			}

			// Convert the channels and samplers into something actually readable
			for (auto& channel : channels)
			{
				std::string targetName;
				for (auto& [boneName, bone] : m_RigData.Bones)
				{
					if (bone.NodeIndex == channel.TargetNODE)
					{
						targetName = boneName;
						break;
					}
				}

				if (!targetName.empty())
				{
					auto& bone = m_RigData.Bones[targetName];
					auto& boneKeyframe = animationData.BoneKeyframes[bone.Name];
					boneKeyframe.SetBoneName(bone.Name);
					AnimationSampler& sampler = samplers[channel.SamplerIndex];

					for (size_t i = 0; i < sampler.inputs.size(); i++)
					{
						if (channel.IsPosition)
						{
							glm::vec3 position = sampler.outputsVec4[i];
							if (m_Settings.ConvertLH)
								position.z = -position.z;
							if (bone.ParentIndex == -1)
							{
								float oldy = position.y;
								position.y = position.z;
								position.z = -oldy;
							}
							boneKeyframe.AddPositionKey(sampler.inputs[i], position);
						}
						else if (channel.IsRotation)
						{
							glm::quat rotation = glm::quat(sampler.outputsVec4[i].w, sampler.outputsVec4[i].x, sampler.outputsVec4[i].y, sampler.outputsVec4[i].z);
							glm::mat mat = glm::toMat4(rotation);

							glm::mat convert = ConvertLH(mat);

							glm::vec3 translation;
							glm::vec3 scale;
							glm::quat rotation2;
							glm::vec3 skew;
							glm::vec4 perspective;
							glm::decompose(convert, scale, rotation2, translation, skew, perspective);

							if (bone.ParentIndex == -1)
							{
								glm::vec3 euler = glm::eulerAngles(rotation2);
								euler.x = glm::degrees(euler.x);
								euler.y = glm::degrees(euler.y);
								euler.z = glm::degrees(euler.z);
								euler.x = euler.x - 90;
								float oldY = euler.y;
								euler.y = euler.z;
								euler.z = -oldY;

								glm::vec3 radians = glm::vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));
								rotation2 = glm::quat(radians);
							}

							boneKeyframe.AddRotationKey(sampler.inputs[i], rotation2);
						}
						else if (channel.IsScale)
						{
							boneKeyframe.AddScaleKey(sampler.inputs[i], sampler.outputsVec4[i]);
						}
					}
				}
			}

			size_t maxFrames = (size_t)std::ceil(animationData.Duration * (double)animationData.FramesPerSecond);
			double step = 1.0 / (double)animationData.FramesPerSecond;

			for (auto& [boneName, boneKeyframe] : animationData.BoneKeyframes)
			{
				// Position Keys
				{
					auto positionKeys = boneKeyframe.GetPositionKeys();
					BoneKeyframe::PositionKey first = positionKeys[0];
					BoneKeyframe::PositionKey last = positionKeys[positionKeys.size() - 1];

					for (int i = 1; i < maxFrames; i++)
					{
						double frameTime = (double)i * step;
						if (!boneKeyframe.HasPositionKey(frameTime))
						{
							double blend = frameTime / animationData.Duration;
							boneKeyframe.AddPositionKey(frameTime, glm::mix(first.Value, last.Value, blend));
						}
					}
				}

				// Rotation Keys
				{
					auto rotationKeys = boneKeyframe.GetRotationKeys();
					BoneKeyframe::RotationKey first = rotationKeys[0];
					BoneKeyframe::RotationKey last = rotationKeys[rotationKeys.size() - 1];

					for (int i = 1; i < maxFrames; i++)
					{
						double frameTime = (double)i * step;
						if (!boneKeyframe.HasRotationKey(frameTime))
						{
							float blend = (float)(frameTime / animationData.Duration);
							boneKeyframe.AddRotationKey(frameTime, glm::slerp(first.Value, last.Value, blend));
						}
					}
				}

				// Scale Keys
				{
					auto scaleKeys = boneKeyframe.GetScaleKeys();
					BoneKeyframe::ScaleKey first = scaleKeys[0];
					BoneKeyframe::ScaleKey last = scaleKeys[scaleKeys.size() - 1];

					for (int i = 1; i < maxFrames; i++)
					{
						double frameTime = (double)i * step;
						if (!boneKeyframe.HasScaleKey(frameTime))
						{
							double blend = frameTime / animationData.Duration;
							boneKeyframe.AddScaleKey(frameTime, glm::mix(first.Value, last.Value, blend));
						}
					}
				}
			}
		}
	}
}