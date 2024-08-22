#include "SourceShader.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	SourceShader::SourceShader(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		std::filesystem::path filename = sourcePath.filename();
		m_ShaderLanguage = filename.extension().string();
		Name = filename.replace_extension("").string();
		m_ShaderCode = ReadShaderFile(sourcePath);

		// TODO: Replace this once shaders are unified
		m_ShaderType = ShaderType::Vertex;
	}

	bool SourceShader::Compile()
	{
		BinaryBuffer tempBuffer;
		return Compile(tempBuffer);
	}

	bool SourceShader::Compile(BinaryBuffer& codeBuffer)
	{
		ShaderCompiler::CompilerSettings options;
		options.ShaderName = Name;
		options.ShaderLanguage = m_ShaderLanguage == "hlsl" ? ShaderLanguage::HLSL : ShaderLanguage::GLSL;
		options.ShaderType = m_ShaderType;
		options.ShaderCode = m_ShaderCode;
		options.Optimize = false;

		return ShaderCompiler::Compile(options, codeBuffer);
	}

	std::string SourceShader::ReadShaderFile(const std::filesystem::path& path)
	{
		std::vector<char> buffer;

		// Open the file as text
		std::ifstream file(path, std::ios::ate);
		if (!file.is_open())
		{
			Logger::LogError("[SourceShader] Unable to open shader file: " + path.string());
			return std::string();
		}

		// Resized the buffer to the file size
		size_t fileSize = (size_t)file.tellg();
		buffer.resize(fileSize);

		// Read from the beginning into the buffer
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return std::string(buffer.data());
	}
}