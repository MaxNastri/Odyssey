#pragma once
#include "Asset.h"
#include "ResourceHandle.h"
#include "Enums.h"
#include "AssetHandle.h"
#include "BinaryBuffer.h"
#include "SourceShader.h"

namespace Odyssey
{
	class VulkanShaderModule;

	class Shader : public Asset
	{
	public:
		Shader() = default;
		Shader(const Path& assetPath);
		Shader(const Path& assetPath, AssetHandle<SourceShader> source);

	public:
		ResourceHandle<VulkanShaderModule> GetShaderModule() { return m_ShaderModule; }
		GUID GetShaderCodeGUID() { return m_ShaderCodeGUID; }
		ShaderType GetShaderType() { return m_ShaderType; }

	public:
		void Save();
		void Load();

	private:
		void LoadFromDisk(const std::filesystem::path& path);
		void SaveToDisk(const std::filesystem::path& path);

	private:
		ShaderType m_ShaderType = ShaderType::None;
		GUID m_ShaderCodeGUID = 0;
		BinaryBuffer m_ShaderCodeBuffer;
		ResourceHandle<VulkanShaderModule> m_ShaderModule;
	};
}