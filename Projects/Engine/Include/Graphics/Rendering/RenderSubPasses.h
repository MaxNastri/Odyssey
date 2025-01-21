#pragma once
#include "Resource.h"
#include "RenderPasses.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "Shader.h"
#include "VulkanPushDescriptors.h"
#include "BinaryBuffer.h"
#include "Material.h"

namespace Odyssey
{
	struct RenderSubPassData
	{
		uint8_t CameraTag;
	};

	class RenderSubPass
	{
	public:
		virtual void Setup() { }
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) { }
	};

	class DepthSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private: // Non-skinned
		Ref<Shader> m_Shader;
		ResourceID m_Pipeline;

	private: //Skinned
		Ref<Shader> m_SkinnedShader;
		ResourceID m_SkinnedPipeline;

	private: // Shared
		Ref<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID m_DepthUBO;

	private:
		inline static const GUID& Shader_GUID = 879318792137863213;
		inline static const GUID& Skinned_Shader_GUID = 218097783217681239;
	};

	class RenderObjectSubPass : public RenderSubPass
	{
	public:
		RenderObjectSubPass(RenderQueue renderQueue);
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		Ref<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID m_GlobalDataUBO;
		Ref<Texture2D> m_BlackTexture;
		ResourceID m_BlackTextureID;
		Ref<Texture2D> m_WhiteTexture;
		ResourceID m_WhiteTextureID;
		RenderQueue m_RenderQueue;

		struct GlobalData
		{
			float4 ZBufferParams;
			float4 ProjectionParams;
			float4 ScreenParams;
			float4 Time;
		};
	private:
		inline static const GUID& s_BlackTextureGUID = 783478723187327676;
		inline static const GUID& s_WhiteTextureGUID = 879823474869737113;
	};

	class DebugSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		Ref<Shader> m_Shader;
		ResourceID m_GraphicsPipeline;
		Ref<VulkanPushDescriptors> m_PushDescriptors;

	private:
		inline static const GUID& s_DebugShaderGUID = 3857300505190494605;
	};

	class SkyboxSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		ResourceID m_GraphicsPipeline;
		Ref<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID uboID;
		Ref<Shader> m_Shader;
		Ref<Mesh> m_CubeMesh;
		inline static const GUID& s_SkyboxShaderGUID = 12373133592092994291;
		inline static const GUID& s_CubeMeshGUID = 4325336624522892849;
	};

	class ParticleSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		ResourceID m_GraphicsPipeline;
		Ref<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID m_ModelUBO;
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_ParticleTexture;
		inline static const GUID& s_ParticleShaderGUID = 16032593712191697003;
		inline static const GUID& s_ParticleTextureGUID = 488672041793267412;
	};

	class Opaque2DSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		void OnSpriteShaderModified();

	private:
		inline static constexpr size_t Max_Supported_Sprites = 1024;

	private:
		Ref<Shader> m_Shader;
		Ref<Mesh> m_QuadMesh;
		ResourceID m_GraphicsPipeline;
		Ref<VulkanPushDescriptors> m_PushDescriptors;
		std::array<ResourceID, Max_Supported_Sprites> m_SpriteDataUBO;

	private:
		struct alignas(16) SpriteData
		{
			float4 PositionScale = float4(0.0f);
			float4 BaseColor = float4(1.0f);
			float4 Fill = float4(1.0f);
			mat4 Projection = mat4(1.0f);
		};

	private:
		inline static const GUID Shader_GUID = 712863487126392356;
		inline static const GUID Quad_Mesh_GUID = 4325336624522892848;
	};

	class Transparent2DSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		ResourceID m_GraphicsPipeline;

	private:
		inline static const GUID Shader_GUID = 16032593712191697003;
	};
}