#pragma once
#include "Asset.h"
#include "Shader.h"
#include "Texture2D.h"
#include "MaterialProperty.h"

namespace Odyssey
{
	enum class RenderQueue : uint32_t
	{
		Opaque = 0,
		Transparent = 1,
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
		int32_t GetInt32(const std::string& propertyName);

	public:
		void SetFloat(const std::string& propertyName, float value);
		void SetFloat2(const std::string& propertyName, float2 value);
		void SetFloat3(const std::string& propertyName, float3 value);
		void SetFloat4(const std::string& propertyName, float4 value);
		void SetBool(const std::string& propertyName, bool value);
		void SetInt32(const std::string& propertyName, int32_t value);

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
		RenderQueue m_RenderQueue = RenderQueue::Opaque;
		BlendMode m_BlendMode = BlendMode::None;
		bool m_DepthWrite = true;
	};
}