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
		if (std::shared_ptr<SourceShader> source = AssetManager::LoadSourceAsset<SourceShader>(m_SourceAsset))
		{
			source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadFromSource(source);
		}

		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			if (shaderData.CodeBuffer)
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
		}
	}

	Shader::Shader(const Path& assetPath, std::shared_ptr<SourceShader> source)
		: Asset(assetPath)
	{
		std::vector<ShaderType> shaderTypes = source->GetShaderTypes();
		for (ShaderType shaderType : shaderTypes)
		{
			auto& shaderData = m_Shaders[shaderType];
			if (source->Compile(shaderType, shaderData.CodeBuffer))
			{
				//shaderData.CodeGUID = AssetManager::CreateBinaryAsset(shaderData.CodeBuffer);
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
			}
		}

		SetSourceAsset(source->GetGUID());
	}


	void Shader::Recompile()
	{
		if (std::shared_ptr<SourceShader> source = AssetManager::LoadSourceAsset<SourceShader>(m_SourceAsset))
		{
			auto shaderTypes = source->GetShaderTypes();
			for (auto shaderType : shaderTypes)
			{
				BinaryBuffer tempBuffer;
				auto& shaderData = m_Shaders[shaderType];

				if (source->Compile(shaderType, tempBuffer))
				{
					shaderData.CodeBuffer = tempBuffer;

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
		if (std::shared_ptr<SourceShader> source = AssetManager::LoadSourceAsset<SourceShader>(m_SourceAsset))
			LoadFromSource(source);
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


	void Shader::LoadFromSource(std::shared_ptr<SourceShader> source)
	{
		// Clear any existing shaders
		for (auto& [shaderType, shaderData] : m_Shaders)
		{
			shaderData.CodeBuffer.Clear();
			ResourceManager::Destroy(shaderData.ShaderModule);
		}
		m_Shaders.clear();

		auto shaderTypes = source->GetShaderTypes();
		for (auto shaderType : shaderTypes)
		{
			BinaryBuffer tempBuffer;
			auto& shaderData = m_Shaders[shaderType];

			if (source->Compile(shaderType, tempBuffer))
			{
				shaderData.CodeBuffer = tempBuffer;
				shaderData.ShaderModule = ResourceManager::Allocate<VulkanShaderModule>(shaderType, shaderData.CodeBuffer);
			}
		}
	}

	void Shader::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize metadata first
		SerializeMetadata(serializer);
		serializer.WriteToDisk(path);
	}

	void Shader::OnSourceModified()
	{
		if (std::shared_ptr<SourceShader> source = AssetManager::LoadSourceAsset<SourceShader>(m_SourceAsset))
			LoadFromSource(source);
	}
}