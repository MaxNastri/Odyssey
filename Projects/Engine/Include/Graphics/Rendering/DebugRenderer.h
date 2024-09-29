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
			glm::vec4 DefaultColor = glm::vec4(1, 0, 0, 1);
			uint64_t MaxVertices = 25000;
		};

	public:
		static void Init(Settings settings);
		static void Shutdown();

	public:
		static void Clear();
		static void AddSphere(glm::vec3 center, float radius, glm::vec4 color);
		static void AddRing(glm::vec3 center, glm::vec4 majorAxis, glm::vec4 minorAxis, glm::vec4 color);

	public:
		static ResourceID GetVertexBuffer();
		static size_t GetVertexCount() { return m_VertexCount; }

	private:
		inline static ResourceID m_VertexBufferID;
		inline static Settings m_Settings;
		inline static std::vector<Vertex> m_Vertices;
		inline static size_t m_VertexCount = 0;
	};
}