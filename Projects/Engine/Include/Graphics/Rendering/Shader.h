#pragma once
#include "Enums.h"
#include "Asset.h"
#include "Resource.h"
#include "BinaryBuffer.h"
#include "SourceShader.h"

namespace Odyssey
{
	class VulkanShaderModule;
	class SourceShader;

	class ShaderBinding
	{
	public:
		std::string Name;
		DescriptorType DescriptorType;
		uint8_t Index;
	};

	class Shader : public Asset
	{
	public:
		CLASS_DECLARATION(Odyssey, Shader)
	public:
		Shader() = default;
		Shader(const Path& assetPath);
		Shader(const Path& assetPath, Ref<SourceShader> source);

	public:
		void Recompile();

	public:
		virtual void Save() override;
		void Load();

	public:
		std::map<ShaderType, ResourceID> GetResourceMap();
		ResourceID GetDescriptorLayout() { return m_DescriptorLayout; }
		std::vector<ShaderBinding>& GetShaderBindings() { return m_Bindings; }

	public:
		void ApplyShaderBindings();
		void AddOnModifiedListener(std::function<void()> callback) { m_OnModifiedListeners.push_back(callback); }
	
	private:
		void LoadAssetData();
		void LoadFromSource(Ref<SourceShader> source);
		void SaveToDisk(const Path& path);

	private:
		void OnSourceModified();

	private:
		struct ShaderData
		{
			GUID CodeGUID;
			BinaryBuffer CodeBuffer;
			ResourceID ShaderModule;
		};
		
		std::map<ShaderType, ShaderData> m_Shaders;
		std::vector<ShaderBinding> m_Bindings;

	private:
		ResourceID m_DescriptorLayout;
		Ref<SourceShader> m_Source;
		std::vector<std::function<void()>> m_OnModifiedListeners;
	};
}