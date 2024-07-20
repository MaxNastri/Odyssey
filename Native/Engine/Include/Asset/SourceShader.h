#pragma once
#include "Asset.h"

namespace Odyssey
{
	class SourceShader : public SourceAsset
	{
	public:
		SourceShader() = default;
		SourceShader(const std::filesystem::path& sourcePath);

	public:
		bool Compile();

	public:
		const std::string& GetShaderLanguage() { return m_ShaderLanguage; }
		bool IsCompiled() { return m_Compiled; }

	private:
		std::string ReadShaderFile(const std::filesystem::path& path);
		
	private:
		std::string m_ShaderName;
		std::string m_ShaderLanguage;
		std::string m_ShaderCode;
		std::vector<uint32_t> m_ByteCode;
		bool m_Compiled = false;
	};
}