#include "SourceShader.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	SourceShader::SourceShader(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		std::filesystem::path filename = sourcePath.filename();
		// Substring(1) to remove the .
		m_ShaderLanguage = filename.extension().string().substr(1);
		Name = filename.replace_extension("").string();
		ParseShaderFile(sourcePath);

		TrackingOptions options;
		options.TrackingPath = sourcePath;
		options.Callback = [this](const Path& path, FileActionType fileAction) { OnFileModified(path, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);
	}

	bool SourceShader::Compile()
	{
		bool compile = true;

		for (auto [shaderType, shaderCode] : m_ShaderCode)
		{
			BinaryBuffer tempBuffer;
			ShaderCompiler::CompilerSettings options;
			options.ShaderName = Name;
			options.ShaderLanguage = m_ShaderLanguage == "hlsl" ? ShaderLanguage::HLSL : ShaderLanguage::GLSL;
			options.ShaderType = shaderType;
			options.ShaderCode = shaderCode;
			options.Optimize = false;
			compile = compile && ShaderCompiler::Compile(options, tempBuffer);
		}

		return compile;
	}

	bool SourceShader::Compile(ShaderType shaderType, BinaryBuffer& codeBuffer)
	{
		if (m_ShaderCode.contains(shaderType))
		{
			ShaderCompiler::CompilerSettings options;
			options.ShaderName = Name;
			options.ShaderLanguage = m_ShaderLanguage == "hlsl" ? ShaderLanguage::HLSL : ShaderLanguage::GLSL;
			options.ShaderType = shaderType;
			options.ShaderCode = m_ShaderCode[shaderType];
			options.Optimize = false;
			return ShaderCompiler::Compile(options, codeBuffer);
		}

		return false;
	}

	std::vector<ShaderType> SourceShader::GetShaderTypes()
	{
		auto keyView = std::views::keys(m_ShaderCode);
		std::vector<ShaderType> keys{ keyView.begin(), keyView.end() };
		return keys;
	}

	void SourceShader::ParseShaderFile(const Path& path)
	{
		m_ShaderCode.clear();

		std::vector<char> buffer;

		// Open the file as text
		std::ifstream file(path, std::ios::ate);
		if (!file.is_open())
		{
			Logger::LogError("[SourceShader] Unable to open shader file: " + path.string());
			return;
		}

		// Resize the buffer to the file size
		size_t fileSize = (size_t)file.tellg();
		buffer.resize(fileSize);

		// Read from the beginning into the buffer
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		// Read the buffer into a string
		std::string fileContents(buffer.data());
		std::string pragmaVertex = "#pragma Vertex\n";
		std::string pragmaFragment = "#pragma Fragment\n";

		// Search for the custom pragmas denoting the start of the vertex/fragment shader
		auto vertexPos = fileContents.find(pragmaVertex);
		auto fragmentPos = fileContents.find(pragmaFragment);

		if (vertexPos != std::string::npos)
		{
			// If we have a fragment pragma, end the vertex shader right before that
			// Otherwise use the end of the file
			size_t end = fragmentPos != std::string::npos ? fragmentPos - pragmaFragment.length() : std::string::npos;
			m_ShaderCode[ShaderType::Vertex] = fileContents.substr(vertexPos + pragmaVertex.length(), end);
		}

		if (fragmentPos != std::string::npos)
		{
			m_ShaderCode[ShaderType::Fragment] = fileContents.substr(fragmentPos + pragmaFragment.length());
		}
	}
	void SourceShader::OnFileModified(const Path& path, FileActionType fileAction)
	{
		if (fileAction == FileActionType::Modified)
		{
			ParseShaderFile(m_SourcePath);

			for (auto& callback : m_OnSourceModified)
				callback();
		}
	}
}