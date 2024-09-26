#pragma once
#include "Enums.h"
#include "Asset.h"
#include "Resource.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	class VulkanShaderModule;
	class SourceShader;

	class Shader : public Asset
	{
	public:
		CLASS_DECLARATION(Odyssey, Shader)
	public:
		Shader() = default;
		Shader(const Path& assetPath);
		Shader(const Path& assetPath, std::shared_ptr<SourceShader> source);

	public:
		void Recompile();

	public:
		void Save();
		void Load();

	public:
		std::map<ShaderType, ResourceID> GetResourceMap();

	private:
		void LoadFromDisk(const Path& path);
		void SaveToDisk(const Path& path);

	private:
		struct ShaderData
		{
			GUID CodeGUID;
			BinaryBuffer CodeBuffer;
			ResourceID ShaderModule;
		};
		std::map<ShaderType, ShaderData> m_Shaders;
	};
}