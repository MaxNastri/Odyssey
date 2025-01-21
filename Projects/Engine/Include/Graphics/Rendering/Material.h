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
		float3 GetEmissiveColor() { return m_EmissiveColor; }
		float GetEmissivePower() { return m_EmissivePower; }
		float GetAlphaClip() { return m_AlphaClip; }
		RenderQueue GetRenderQueue() const { return m_RenderQueue; }
		BlendMode GetBlendMode() { return m_BlendMode; }
		bool GetDepthWrite() { return m_DepthWrite; }
		ResourceID GetPipeline();

	public:
		void SetShader(Ref<Shader> shader);
		void SetTexture(std::string propertyName, GUID texture);
		void SetEmissiveColor(float3 emissive) { m_EmissiveColor = emissive; }
		void SetEmissivePower(float power) { m_EmissivePower = power; }
		void SetAlphaClip(float clip) { m_AlphaClip = clip; }
		void SetRenderQueue(RenderQueue queue) { m_RenderQueue = queue; }
		void SetBlendMode(BlendMode blendMode);
		void SetDepthWrite(bool write);

	public:
		std::map<std::string, Ref<Texture2D>> GetTextures() { return m_Textures; }

	private:
		std::map<std::string, Ref<Texture2D>> m_Textures;
		ResourceID m_GraphicsPipeline;
		bool m_RemakePipeline = false;

		Ref<Shader> m_Shader;
		float3 m_EmissiveColor = float3(0.0f);
		float m_EmissivePower = 1.0f;
		float m_AlphaClip = 0.5f;
		RenderQueue m_RenderQueue = RenderQueue::None;
		BlendMode m_BlendMode = BlendMode::None;
		bool m_DepthWrite = true;
	};
}