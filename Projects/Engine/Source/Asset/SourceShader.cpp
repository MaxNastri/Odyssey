#include "SourceShader.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	SourceShader::SourceShader(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		Path filename = sourcePath.filename();
		// Substring(1) to remove the .
		m_ShaderLanguage = filename.extension().string().substr(1);
		Name = filename.replace_extension("").string();
		ParseShaderFile(sourcePath);

		TrackingOptions options;
		options.TrackingPath = sourcePath;
		options.Callback = [this](const Path& oldPath, const Path& newPath, FileActionType fileAction) { OnFileAction(oldPath, newPath, fileAction); };
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
			Log::Error("[SourceShader] Unable to open shader file: " + path.string());
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
		ParseShaderCode(fileContents);
	}

	void SourceShader::ParseShaderCode(const std::string& fileContents)
	{
		struct ShaderParseData
		{
		public:
			std::string Pragma;
			size_t FilePos;
			ShaderType ShaderType;
		};

		std::vector<ShaderParseData> parseData;

		// Search for each shader pragma while sorting the entries by position
		for (auto& [shaderType, pragma] : ShaderPragmaMap)
		{
			size_t filePos = fileContents.find(pragma);

			// Skip any shader's that were not present in the file
			if (filePos == std::string::npos)
				continue;

			ShaderParseData shaderParse{ pragma, filePos, shaderType };
			bool inserted = false;

			for (size_t i = 0; i < parseData.size(); i++)
			{
				if (parseData[i].FilePos > shaderParse.FilePos)
				{
					parseData.insert(parseData.begin() + i, shaderParse);
					inserted = true;
					break;
				}
			}

			if (!inserted)
				parseData.push_back(shaderParse);
		}

		// Parse the file contents to extract each shader's code
		for (size_t i = 0; i < parseData.size(); i++)
		{
			ShaderParseData& shaderParse = parseData[i];
			size_t start = shaderParse.FilePos + shaderParse.Pragma.length();
			size_t end = std::string::npos;

			if (i < parseData.size() - 1)
			{
				ShaderParseData& nextParse = parseData[i + 1];

				if (nextParse.FilePos != std::string::npos)
				{
					// Move the end to the beginning of the next pragma and calculate the length from the start
					end = nextParse.FilePos - start;
				}
			}

			m_ShaderCode[shaderParse.ShaderType] = fileContents.substr(start, end);
		}
	}

	void SourceShader::OnFileAction(const Path& oldFilename, const Path& newFilename, FileActionType fileAction)
	{
		if (fileAction == FileActionType::Modified)
		{
			ParseShaderFile(m_SourcePath);
			OnSourceModified();
		}
	}
}