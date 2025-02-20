#pragma once
#include "Resource.h"
#include "Vertex.h"
#include "RenderPasses.h"

namespace Odyssey
{
	class Shader;

	class DebugRenderer
	{
	public:
		struct Settings
		{
		public:
			Path DebugShaderPath = "";
			float4 DefaultColor = glm::vec4(0, 1, 0, 1);
			uint64_t MaxVertices = 25000;
		};

	public:
		static void Init(Settings settings);
		static void Shutdown();
		static uint32_t Register(std::function<void(void)> drawCallback);
		static void Deregister(uint32_t id);

	public:
		static void Update();
		static void AddSphere(float3 center, float radius, float3 color);
		static void AddRing(float3 center, float4 majorAxis, float4 minorAxis, float3 color, bool half = false);
		static void AddAABB(float3 center, float3 extents, float3 color);
		static void AddOrientedBox(float3 center, float3 extents, float3 right, float3 up, float3 forward, float3 color);
		static void AddLine(float3 startPosition, float3 startColor, float3 endPosition, float3 endColor);
		static void AddCylinder(float3 center, float radius, float halfHeight, float3 color);
		static void AddCapsule(float3 center, float radius, float halfHeight, float3 color);

	public:
		static ResourceID GetVertexBuffer();
		static size_t GetVertexCount() { return m_VertexCount; }

	private:
		inline static constexpr size_t Ring_Segments = 32;
		inline static ResourceID m_VertexBufferID;
		inline static Settings m_Settings;
		inline static std::vector<Vertex> m_Vertices;
		inline static size_t m_VertexCount = 0;

	private:
		inline static std::map<uint32_t, std::function<void(void)>> s_DrawCallbacks;
		inline static uint32_t s_RegisterID = 0;
	};
}