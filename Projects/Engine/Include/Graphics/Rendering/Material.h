#pragma once
#include "Asset.h"
#include "Ref.h"
#include "Shader.h"
#include "Texture2D.h"

namespace Odyssey
{
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

	public:
		Ref<Shader> GetShader() { return m_Shader; }
		Ref<Texture2D> GetColorTexture() { return m_ColorTexture; }
		Ref<Texture2D> GetNormalTexture() { return m_NormalTexture; }
		Ref<Texture2D> GetNoiseTexture() { return m_NoiseTexture; }
		float3 GetEmissiveColor() { return m_EmissiveColor; }
		float GetEmissivePower() { return m_EmissivePower; }
		float GetAlphaClip() { return m_AlphaClip; }
		bool GetAlphaBlend() { return m_AlphaBlend; }
	public:
		void SetShader(Ref<Shader> shader) { m_Shader = shader; }
		void SetColorTexture(Ref<Texture2D> texture) { m_ColorTexture = texture; }
		void SetNormalTexture(Ref<Texture2D> texture) { m_NormalTexture = texture; }
		void SetNoiseTexture(Ref<Texture2D> texture) { m_NoiseTexture = texture; }
		void SetEmissiveColor(float3 emissive) { m_EmissiveColor = emissive; }
		void SetEmissivePower(float power) { m_EmissivePower = power; }
		void SetAlphaClip(float clip) { m_AlphaClip = clip; }
		void SetAlphaBlend(bool blend) { m_AlphaBlend = blend; }

	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_ColorTexture;
		Ref<Texture2D> m_NormalTexture;
		Ref<Texture2D> m_NoiseTexture;
		float3 m_EmissiveColor = float3(0.0f);
		float m_EmissivePower = 1.0f;
		float m_AlphaClip = 0.5f;
		bool m_AlphaBlend = false;
	};
}