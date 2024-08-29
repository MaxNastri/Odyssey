#include "Shader.h"
#include "VulkanShaderModule.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "SourceShader.h"

namespace Odyssey
{
	Shader::Shader(const Path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);

		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			if (shaderData.CodeBuffer)
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
		}
	}

	Shader::Shader(const Path& assetPath, std::shared_ptr<SourceShader> source)
		: Asset(assetPath)
	{
		auto shaderTypes = source->GetShaderTypes();
		for (auto shaderType : shaderTypes)
		{
			auto& shaderData = m_Shaders[shaderType];
			if (source->Compile(shaderType, shaderData.CodeBuffer))
			{
				shaderData.CodeGUID = AssetManager::CreateBinaryAsset(shaderData.CodeBuffer);
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
			}
		}

		SetSourceAsset(source->GetGUID());
	}


	void Shader::Recompile()
	{
		if (std::shared_ptr<SourceShader> source = AssetManager::LoadSourceShader(m_SourceAsset))
		{
			auto shaderTypes = source->GetShaderTypes();
			for (auto shaderType : shaderTypes)
			{
				BinaryBuffer tempBuffer;
				auto& shaderData = m_Shaders[shaderType];

				if (source->Compile(shaderType, tempBuffer))
				{
					shaderData.CodeBuffer = tempBuffer;
					AssetManager::WriteBinaryAsset(shaderData.CodeGUID, shaderData.CodeBuffer);

					if (shaderData.ShaderModule)
						ResourceManager::Destroy(shaderData.ShaderModule);

					shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
				}
			}
		}
	}

	void Shader::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Shader::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	std::map<ShaderType, ResourceID> Shader::GetResourceMap()
	{
		std::map<ShaderType, ResourceID> resourceMap;

		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			resourceMap[shaderType] = shaderData.ShaderModule;
		}

		return resourceMap;
	}


	void Shader::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			std::string modulePath;


			SerializationNode shadersNode = root.GetNode("m_Shaders");
			assert(shadersNode.IsSequence());
			assert(shadersNode.HasChildren());

			for (size_t i = 0; i < shadersNode.ChildCount(); ++i)
			{
				SerializationNode shaderNode = shadersNode.GetChild(i);
				assert(shaderNode.IsMap());

				// Read in the shader type
				uint32_t shaderType = 0;
				shaderNode.ReadData("ShaderType", shaderType);

				// Get the code GUID
				auto& shaderData = m_Shaders[(ShaderType)shaderType];
				shaderNode.ReadData("ShaderCode", shaderData.CodeGUID.Ref());

				if (shaderData.CodeGUID)
					shaderData.CodeBuffer = AssetManager::LoadBinaryAsset(shaderData.CodeGUID);
			}
		}
	}

	void Shader::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);

		SerializationNode shaders = root.CreateSequenceNode("m_Shaders");
		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			SerializationNode shaderNode = shaders.AppendChild();
			shaderNode.SetMap();
			shaderNode.WriteData("ShaderType", (uint32_t)shaderType);
			shaderNode.WriteData("ShaderCode", shaderData.CodeGUID.CRef());
		}

		serializer.WriteToDisk(path);
	}
}