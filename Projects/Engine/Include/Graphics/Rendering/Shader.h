#pragma once
#include "Enums.h"
#include "Asset.h"
#include "Resource.h"
#include "BinaryBuffer.h"
#include "SourceShader.h"
#include "RawBuffer.h"

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

	enum class PropertyType
	{
		Unknown = 0,
		Float = 1,
		Float2 = 2,
		Float3 = 3,
		Float4 = 4,
		Bool = 5,
	};

	struct MaterialProperty
	{
		std::string Name;
		size_t Offset;
		size_t Size;
		PropertyType Type;
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
		BinaryBuffer& GetVertexAttributes() { return m_VertexAttributes; }
		std::map<std::string, ShaderBinding>& GetBindings() { return m_Bindings; }

	public:
		bool HasBinding(std::string bindingName, uint32_t& index);
		const std::vector<MaterialProperty>& GetMaterialProperties() { return m_MaterialBufferProperties; }
		size_t GetMaterialPropertiesSize() { return m_MaterialBufferSize; }

	public:
		uint32_t AddOnModifiedListener(std::function<void()> callback);
		void RemoveOnModifiedListener(uint32_t listenerID);

	private:
		void LoadFromSource(Ref<SourceShader> source);
		void SaveToDisk(const Path& path);

	private:
		void GenerateShaderResources();
		void OnSourceModified();

	private:
		struct Listener
		{
			uint32_t ID = 0;
			std::function<void()> Callback = nullptr;
		};

		struct ShaderData
		{
			GUID CodeGUID;
			BinaryBuffer CodeBuffer;
			ResourceID ShaderModule;
		};
		
		std::map<ShaderType, ShaderData> m_Shaders;
		std::map<std::string, ShaderBinding> m_Bindings;
		BinaryBuffer m_VertexAttributes;

	private:
		std::vector<MaterialProperty> m_MaterialBufferProperties;
		size_t m_MaterialBufferSize = 0;

	private:
		ResourceID m_DescriptorLayout;
		Ref<SourceShader> m_Source;
		uint32_t m_NextID = 0;
		std::vector<Listener> m_OnModifiedListeners;
	};
}