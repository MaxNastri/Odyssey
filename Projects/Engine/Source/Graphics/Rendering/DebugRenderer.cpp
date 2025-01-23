#include "DebugRenderer.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "VulkanBuffer.h"

namespace Odyssey
{
	void DebugRenderer::Init(Settings settings)
	{
		m_Settings = settings;

		m_Vertices.resize(m_Settings.MaxVertices);

		// Allocate a vertex buffer
		size_t dataSize = m_Vertices.size() * sizeof(m_Vertices[0]);
		m_VertexBufferID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Vertex, dataSize);

		// Upload the vertex data
		Ref<VulkanBuffer> vertexBuffer = ResourceManager::GetResource<VulkanBuffer>(m_VertexBufferID);
		vertexBuffer->UploadData(m_Vertices.data(), dataSize);
	}

	void DebugRenderer::Shutdown()
	{
		ResourceManager::Destroy(m_VertexBufferID);
		m_VertexBufferID = ResourceID::Invalid();
		m_VertexCount = 0;
	}

	uint32_t DebugRenderer::Register(std::function<void(void)> drawCallback)
	{
		uint32_t id = s_RegisterID++;
		s_DrawCallbacks[id] = drawCallback;
		return id;
	}

	void DebugRenderer::Deregister(uint32_t id)
	{
		if (s_DrawCallbacks.contains(id))
			s_DrawCallbacks.erase(id);
	}

	void DebugRenderer::Update()
	{
		// Reset the vertex count
		m_VertexCount = 0;

		// Invoke all of the draw callbacks
		for (auto& [id, drawCallback] : s_DrawCallbacks)
			drawCallback();
	}

	void DebugRenderer::AddSphere(float3 center, float radius, float3 color)
	{
		AddRing(center, { radius, 0, 0, 0 }, { 0, 0, radius, 0 }, color);
		AddRing(center, { radius, 0, 0, 0 }, { 0, radius, 0, 0 }, color);
		AddRing(center, { 0, radius, 0, 0 }, { 0, 0, radius, 0 }, color);
	}

	void DebugRenderer::AddRing(float3 center, float4 majorAxis, float4 minorAxis, float3 color, bool half)
	{
		// Determine how many ring segments to add
		size_t ringSegments = half ? (Ring_Segments / 2) + 1 : Ring_Segments;
		std::vector<Vertex> vertices;
		vertices.resize(ringSegments + 1);

		// The delta angle will always be measured using the full ring segments
		constexpr float angleDelta = glm::two_pi<float>() / Ring_Segments;
		float cosDelta = cosf(angleDelta);
		float sinDelta = sinf(angleDelta);

		float4 incrementalSin = float4(0, 0, 0, 0);
		const float4 initialCos = float4(1, 1, 1, 1);
		float4 incrementalCos = initialCos;

		for (size_t i = 0; i < ringSegments; i++)
		{
			vertices[i].Position = (majorAxis * incrementalCos) + (minorAxis * incrementalSin) + float4(center, 0);
			vertices[i].Color = float4(color, 1.0f);

			// Advance the angles
			float4 newCos = (incrementalCos * cosDelta) - (incrementalSin * sinDelta);
			float4 newSin = (incrementalCos * sinDelta) + (incrementalSin * cosDelta);
			incrementalCos = newCos;
			incrementalSin = newSin;
		}

		// Connect the ring back to the first vertex
		vertices[ringSegments] = vertices[0];

		for (size_t i = 0; i < ringSegments; i++)
		{
			m_Vertices[m_VertexCount] = vertices[i];
			++m_VertexCount;
			m_Vertices[m_VertexCount] = vertices[i + 1];
			++m_VertexCount;
		}
	}

	void DebugRenderer::AddAABB(float3 center, float3 extents, float3 color)
	{
		// Add a box oriented to normalized world space
		AddOrientedBox(center, extents, float3(1.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), color);
	}

	void DebugRenderer::AddOrientedBox(float3 center, float3 extents, float3 right, float3 up, float3 forward, float3 color)
	{
		// Generate the 8 corners of the box
		std::vector<float3> corners;
		corners.resize(8);

		right = glm::normalize(right) * extents.x;
		up = glm::normalize(up) * extents.y;
		forward = glm::normalize(forward) * extents.z;

		// Near upper left
		corners[0] = center - right + up - forward;
		// Near upper right
		corners[1] = center + right + up - forward;
		// Near lower right
		corners[2] = center + right - up - forward;
		// Near lower left
		corners[3] = center - right - up - forward;
		// Far upper left
		corners[4] = center - right + up + forward;
		// Far upper right
		corners[5] = center + right + up + forward;
		// Far lower right
		corners[6] = center + right - up + forward;
		// Far lower left
		corners[7] = center - right - up + forward;

		// Now draw lines between the corners
		for (size_t i = 0; i < 4; i++)
		{
			if (i < 3)
			{
				// Draw the near face line
				AddLine(corners[i], color, corners[i + 1], color);
				// Draw the corresponding far face line
				AddLine(corners[i + 4], color, corners[i + 5], color);
			}
			else
			{
				AddLine(corners[i], color, corners[0], color);
				AddLine(corners[i + 4], color, corners[4], color);
			}
			AddLine(corners[i], color, corners[i + 4], color);
		}
	}

	void DebugRenderer::AddLine(float3 startPosition, float3 startColor, float3 endPosition, float3 endColor)
	{
		m_Vertices[m_VertexCount].Position = startPosition;
		m_Vertices[m_VertexCount].Color = float4(startColor, 1.0f);
		++m_VertexCount;

		m_Vertices[m_VertexCount].Position = endPosition;
		m_Vertices[m_VertexCount].Color = float4(endColor, 1.0f);
		++m_VertexCount;
	}

	void DebugRenderer::AddCylinder(float3 center, float radius, float halfHeight, float3 color)
	{
		float3 topCenter = center + float3(0.0f, halfHeight - (radius * 0.5f), 0.0f);
		float3 bottomCenter = center - float3(0.0f, halfHeight + (radius * 0.5f), 0.0f);

		// Top ring around the center
		size_t topRingStart = m_VertexCount;
		AddRing(topCenter, { radius, 0, 0, 0 }, { 0, 0, radius, 0 }, color);

		// Bottom ring around the center
		size_t bottomRingStart = m_VertexCount;
		AddRing(bottomCenter, { radius, 0, 0, 0 }, { 0, 0, radius, 0 }, color);

		// 4 lines connecting the two rings together
		// Note: Each line moves along a quarter of the ring segment per iteration
		size_t offset = Ring_Segments / 2;
		for (size_t i = 0; i < 4; i++)
		{
			AddLine(m_Vertices[topRingStart + (offset * i)].Position, color, m_Vertices[bottomRingStart + (offset * i)].Position, color);
		}
	}

	void DebugRenderer::AddCapsule(float3 center, float radius, float halfHeight, float3 color)
	{
		float3 topPos = center + float3(0, halfHeight - (radius / 2.0f), 0.0f);
		float3 botPos = center - float3(0, halfHeight + (radius / 2.0f), 0.0f);

		// Add a cylinder
		AddCylinder(center, radius, halfHeight, color);

		// Top half-ring cap
		AddRing(topPos, { radius, 0, 0, 0 }, { 0, radius, 0, 0 }, color, true);
		AddRing(topPos, { 0, 0, radius, 0 }, { 0, radius, 0, 0 }, color, true);

		// Bottom half-ring cap
		AddRing(botPos, { radius, 0, 0, 0 }, { 0, -radius, 0, 0 }, color, true);
		AddRing(botPos, { 0, 0, radius, 0 }, { 0, -radius, 0, 0 }, color, true);
	}

	ResourceID DebugRenderer::GetVertexBuffer()
	{
		// Upload the vertex data before returning the buffer ID
		size_t dataSize = m_Vertices.size() * sizeof(m_Vertices[0]);
		Ref<VulkanBuffer> vertexBuffer = ResourceManager::GetResource<VulkanBuffer>(m_VertexBufferID);
		vertexBuffer->UploadData(m_Vertices.data(), dataSize);

		return m_VertexBufferID;
	}
}