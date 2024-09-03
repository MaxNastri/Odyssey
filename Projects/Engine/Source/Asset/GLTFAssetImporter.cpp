#include "GLTFAssetImporter.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "Logger.h"
#include "Vertex.h"

namespace Odyssey
{
	using namespace tinygltf;

	bool GLTFAssetImporter::Import(const Path& filePath)
	{
		const Path& extension = filePath.extension();
		assert(extension == ".glb" || extension == ".gltf");

		Model model;
		TinyGLTF context;
		std::string error;
		std::string warning;

		bool status = false;

		if (extension == ".glb")
			status = context.LoadBinaryFromFile(&model, &error, &warning, filePath.string());
		else
			status = context.LoadASCIIFromFile(&model, &error, &warning, filePath.string());

		if (!warning.empty())
		{
			Logger::LogWarning(std::format("Warning detected while importing GLTF asset: {}", filePath.string()));
			Logger::LogWarning(warning);
		}

		if (!error.empty())
		{
			Logger::LogWarning(std::format("Error detected while importing GLTF asset: {}", filePath.string()));
			Logger::LogWarning(error);
			return false;
		}

		LoadMeshData(&model);
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

	void GLTFAssetImporter::LoadMeshData(const Model* model)
	{
		m_MeshData.ObjectCount = 0;

		for (size_t m = 0; m < model->meshes.size(); m++)
		{
			const Mesh& mesh = model->meshes[m];
			size_t meshIndex = m_MeshData.ObjectCount++;


			for (size_t p = 0; p < mesh.primitives.size(); p++)
			{
				const Primitive& primitive = mesh.primitives[p];

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

					std::vector<Vertex> vertices;
					vertices.resize(vertexCount);

					for (size_t v = 0; v < vertexCount; v++)
					{
						Vertex& vertex = vertices[v];

						// IMPORTANT: Negate the z components to convert RH to LH coord system
						vertex.Position = glm::make_vec3(positionData.GetData<float>(v));
						vertex.Position.z = -vertex.Position.z;
						vertex.Normal = normalData.IsValid() ? glm::make_vec3(normalData.GetData<float>(v)) : glm::vec3(0.0f);
						vertex.Normal.z = -vertex.Normal.z;
						vertex.Color = colorData.IsValid() ? glm::vec4(glm::make_vec3(colorData.GetData<float>(v)), 1.0f) : glm::vec4(1.0f);
						
						// IMPORTANT: We flip the UV to convert RH to LH
						vertex.TexCoord0 = texCoord0Data.IsValid() ? glm::make_vec2(texCoord0Data.GetData<float>(v)) : glm::vec2(0.0f);
						vertex.TexCoord0.y = 1.0f - vertex.TexCoord0.y;
						
						vertex.BoneWeights = weightsData.IsValid() ? glm::make_vec4(weightsData.GetData<float>(v)) : glm::vec4(0.0f);
						if (glm::length(vertex.BoneWeights) == 0.0f)
							vertex.BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

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

					m_MeshData.VertexLists.push_back(vertices);
				}

				// Indices
				const tinygltf::Accessor& accessor = model->accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];
				const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

				uint32_t indexCount = static_cast<uint32_t>(accessor.count);

				uint32_t indexStart = 0;
				std::vector<uint32_t> indices;
				indices.resize(indexCount);

				switch (accessor.componentType)
				{
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
					{
						const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							indices[indexStart] = buf[index];
							++indexStart;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
					{
						const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							indices[indexStart] = buf[index];
							++indexStart;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
					{
						const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);

						for (size_t index = 0; index < accessor.count; index++)
						{
							indices[indexStart] = buf[index];
							++indexStart;
						}
						break;
					}
				}

				// IMPORTANT: We reverse the winding order to convert RH to LH coord system
				std::reverse(indices.begin(), indices.end());
				m_MeshData.IndexLists.push_back(indices);
			}
		}
	}
}