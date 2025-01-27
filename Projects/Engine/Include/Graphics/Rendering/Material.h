#pragma once
#include "Asset.h"
#include "Ref.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Enums.h"
#include "Enum.h"

namespace Odyssey
{
	enum class RenderQueue : uint32_t
	{
		None = 0,
		Opaque = 1,
		Transparent = 2,
	};

	struct MaterialBufferData
	{
	public:
		MaterialBufferData() = default;

		MaterialBufferData(const std::vector<MaterialProperty>& properties, size_t size)
		{
			Set(properties, size);
		}

		void Set(const std::vector<MaterialProperty>& properties, size_t size)
		{
			Size = size;
			Properties = properties;

			// Fill the map
			for (size_t i = 0; i < Properties.size(); i++)
				PropertyMap[Properties[i].Name] = i;

			Buffer.Free();
			Buffer.Allocate(Size);
		}

		template<typename T>
		T GetValue(const std::string& name)
		{
			if (PropertyMap.contains(name))
			{
				size_t index = PropertyMap[name];
				size_t size = Properties[index].Size;
				size_t offset = Properties[index].Offset;
				return Buffer.Read<T>(offset, size);
			}

			return (T)0;
		}

		void SetValue(const std::string& name, const void* value)
		{
			if (PropertyMap.contains(name))
			{
				size_t index = PropertyMap[name];
				size_t size = Properties[index].Size;
				size_t offset = Properties[index].Offset;
				return Buffer.Write(value, size, offset);
			}
		}

	public:
		std::unordered_map<std::string, size_t> PropertyMap;
		std::vector<MaterialProperty> Properties;
		size_t Size;
		RawBuffer Buffer;
	};

	class Material : public Asset
	{
		CLASS_DECLARATION(Odyssey, Material)
	public:
		Material() = default;
		Material(const Path& assetPath);

	public:
		virtual void Save() override;
		void Load();

	private:
		void SaveToDisk(const Path& path);
		void LoadFromDisk(const Path& path);
		void CreatePipeline();
		void OnShaderModified();

	public:
		Ref<Shader> GetShader() { return m_Shader; }
		RenderQueue GetRenderQueue() const { return m_RenderQueue; }
		BlendMode GetBlendMode() { return m_BlendMode; }
		bool GetDepthWrite() { return m_DepthWrite; }
		const std::vector<MaterialProperty>& GetMaterialProperties() { return m_MaterialData.Properties; }

	public:
		ResourceID GetPipeline();
		ResourceID GetMaterialBuffer();

	public:
		void SetShader(Ref<Shader> shader);
		void SetTexture(std::string propertyName, GUID texture);
		void SetRenderQueue(RenderQueue queue) { m_RenderQueue = queue; }
		void SetBlendMode(BlendMode blendMode);
		void SetDepthWrite(bool write);

	public:
		float GetFloat(const std::string& propertyName);
		float2 GetFloat2(const std::string& propertyName);
		float3 GetFloat3(const std::string& propertyName);
		float4 GetFloat4(const std::string& propertyName);
		bool GetBool(const std::string& propertyName);

	public:
		void SetFloat(const std::string& propertyName, float value);
		void SetFloat2(const std::string& propertyName, float2 value);
		void SetFloat3(const std::string& propertyName, float3 value);
		void SetFloat4(const std::string& propertyName, float4 value);
		void SetBool(const std::string& propertyName, bool value);

	public:
		std::map<std::string, Ref<Texture2D>> GetTextures() { return m_Textures; }

	private:
		MaterialBufferData m_MaterialData;

	private:
		std::map<std::string, Ref<Texture2D>> m_Textures;
		ResourceID m_GraphicsPipeline;
		ResourceID m_MaterialBuffer;
		bool m_RemakePipeline = false;
		bool m_UpdateBuffer = false;

		Ref<Shader> m_Shader;
		uint32_t m_ListenerID = 0;

	private: // Serialized
		RenderQueue m_RenderQueue = RenderQueue::None;
		BlendMode m_BlendMode = BlendMode::None;
		bool m_DepthWrite = true;
	};
}