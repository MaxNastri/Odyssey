#include "ShaderCompiler.h"
#include <shaderc/shaderc.hpp>
#include <fstream>
#include <filesystem>
#include "Logger.h"

namespace Odyssey
{
	ShaderCompiler::ShaderCompiler(Options options)
	{
		m_Options = options;
	}

	void ShaderCompiler::Compile(std::string_view shaderName, ShaderType shaderType, std::filesystem::path shaderPath)
	{
		// HLSL Shader
		shaderc::Compiler compiler;
		shaderc::CompileOptions compilerOptions;

		if (shaderPath.extension() == HLSL_EXTENSION)
		{
			// Set HLSL as the language
			compilerOptions.SetSourceLanguage(shaderc_source_language_hlsl);
		}
		else if (shaderPath.extension() == GLSL_EXTENSION)
		{
			// GLSL Shader
			compilerOptions.SetSourceLanguage(shaderc_source_language_glsl);
		}
		else
		{
			Logger::LogError("[ShaderCompiler] Invalid shader file format detected: " + shaderPath.extension().string());
			return;
		}

		if (m_Options.Optimize)
			compilerOptions.SetOptimizationLevel(shaderc_optimization_level_size);

		std::vector<char> fileContents = ReadShaderFile(shaderPath);

		if (fileContents.size() == 0)
		{
			Logger::LogError("[ShaderCompiler] Failed to compile shader: " + shaderPath.string());
			return;
		}
		auto type = shaderType == ShaderType::Vertex ? shaderc_vertex_shader : shaderc_fragment_shader;

		std::string shaderCode(fileContents.data());

		shaderc::SpvCompilationResult result =
			compiler.CompileGlslToSpv(shaderCode, type, shaderName.data(), compilerOptions);

		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			Logger::LogError("[ShaderCompiler] Failed to compile shader: " + shaderPath.string());
			return;
		}

		std::vector<uint32_t> shaderBinary(result.begin(), result.end());
	}
	std::vector<char> ShaderCompiler::ReadShaderFile(std::filesystem::path& path)
	{
		std::vector<char> buffer;

		// Open the file as text
		std::ifstream file(path, std::ios::ate);
		if (!file.is_open())
		{
			Logger::LogError("[ShaderCompiler] Unable to open shader file: " + path.string());
			return buffer;
		}

		// Resized the buffer to the file size
		size_t fileSize = (size_t)file.tellg();
		buffer.resize(fileSize);

		// Read from the beginning into the buffer
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}
}