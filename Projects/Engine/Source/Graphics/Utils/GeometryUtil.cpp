#include "glm.h"
#include "GeometryUtil.h"
#include <utility>

namespace Odyssey
{
	void GeometryUtil::ComputeBox(vec3 center, vec3 scale, std::vector<VulkanVertex>& vertices, std::vector<unsigned int>& indices)
	{
		// A box has six faces, each one pointing in a different direction.
		constexpr int FaceCount = 6;

		static const vec3 faceNormals[FaceCount] =
		{
			{  0.0f,  0.0f,  1.0f },
			{  0.0f,  0.0f, -1.0f },
			{  1.0f,  0.0f,  0.0f },
			{ -1.0f,  0.0f,  0.0f },
			{  0.0f,  1.0f,  0.0f },
			{  0.0f, -1.0f,  0.0f },
		};

		static const vec2 uvs[4] =
		{
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },
		};

		// Convert the scale to extents
		vec3 extents = scale / 2.0f;

		// Create each face in turn.
		for (int i = 0; i < FaceCount; i++)
		{
			vec3 normal = faceNormals[i];

			// Get two vectors perpendicular both to the face normal and to each other.
			vec3 basis = (i >= 4) ? vec3(0.0f, 0.0f, 1.0f) : vec3(0.0f, 1.0f, 0.0f);

			vec3 side1 = glm::cross(normal, basis);
			vec3 side2 = glm::cross(normal, side1);

			// Six indices (two triangles) per face.
			unsigned int vbase = static_cast<unsigned int>(vertices.size());

			// Push back the indices
			indices.push_back(vbase + 0);
			indices.push_back(vbase + 1);
			indices.push_back(vbase + 2);

			indices.push_back(vbase + 0);
			indices.push_back(vbase + 2);
			indices.push_back(vbase + 3);

			// Four vertices per face.
			// (normal - side1 - side2) * tsize // normal // t0
			vec3 position = (normal - side1 - side2) * extents;
			vertices.push_back(VulkanVertex(position + center, normal, uvs[0]));

			// (normal - side1 + side2) * tsize // normal // t1
			position = (normal - side1 + side2) * extents;
			vertices.push_back(VulkanVertex(position + center, normal, uvs[1]));

			// (normal + side1 + side2) * tsize // normal // t2
			position = (normal + side1 + side2) * extents;
			vertices.push_back(VulkanVertex(position + center, normal, uvs[2]));

			// (normal + side1 - side2) * tsize // normal // t3
			position = (normal + side1 - side2) * extents;
			vertices.push_back(VulkanVertex(position + center, normal, uvs[3]));
		}

		ReverseWinding(vertices, indices);
	}

	void GeometryUtil::ComputeSphere(float radius, unsigned int tesselation, std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices)
	{
		unsigned int verticalSegments = tesselation;
		unsigned int horizontalSegment = tesselation * 2;

		for (unsigned int i = 0; i <= verticalSegments; ++i)
		{
			float v = 1.0f - float(i) / float(verticalSegments);
			float latitude = (float(i) * glm::pi<float>() / float(verticalSegments)) - glm::half_pi<float>();

			float dy = glm::sin(latitude);
			float dxz = glm::cos(latitude);

			for (unsigned int j = 0; j <= horizontalSegment; ++j)
			{
				float u = float(j) / float(horizontalSegment);
				float longitude = float(j) * glm::two_pi<float>() / float(horizontalSegment);

				float dx = glm::sin(longitude);
				float dz = glm::cos(longitude);
				dx *= dxz;
				dz *= dxz;

				vec3 normal = vec3(dx, dy, dz);
				vec2 uv = vec2(u, v);

				vertices.push_back(VulkanVertex(normal * radius, normal, uv));
			}
		}

		unsigned int stride = horizontalSegment + 1;

		for (unsigned int i = 0; i < verticalSegments; ++i)
		{
			for (unsigned int j = 0; j < horizontalSegment; ++j)
			{
				unsigned int nextI = i + 1;
				unsigned int nextJ = (j + 1) % stride;

				indices.push_back(i * stride + j);
				indices.push_back(nextI * stride + j);
				indices.push_back(i * stride + nextJ);

				indices.push_back(i * stride + nextJ);
				indices.push_back(nextI * stride + j);
				indices.push_back(nextI * stride + nextJ);
			}
		}

		ReverseWinding(vertices, indices);
	}

	void GeometryUtil::ComputeDiamond(std::vector<VulkanVertex>& vertices, std::vector<unsigned int>& indices)
	{
		// Math constants
		const float pi = acosf(-1.0f);
		const quat quatFull = glm::angleAxis(2.0f * pi * 0.5f, vec3(0, 0, 1));
		const quat quatHalf = glm::angleAxis(2.0f * pi * 0.125f * 0.5f, vec3(0, 0, 1));

		// Constants for normal + uv
		vec3 normal = vec3(1, 1, 1);
		vec2 uv = vec2(0, 0);

		// Reserve the necessary slots in the out vertices
		vertices.resize(7 * 8);
		indices.resize(7 * 8);

		// Make the verts we need
		VulkanVertex verts[8] =
		{
			VulkanVertex(vec3(0.1f, 0, -1), normal, uv),
			VulkanVertex(vec3(1, 0, 0), normal, uv),
			VulkanVertex(vec3(1, 0, 0.1f), normal, uv),
			VulkanVertex(vec3(0.4f, 0, 0.4f), normal, uv),
			VulkanVertex(vec3(0.8f, 0, 0.3f), normal, uv),
			VulkanVertex(glm::rotate(quatHalf, vertices[4].Position), normal, uv),
			VulkanVertex(glm::rotate(quatHalf, vertices[1].Position), normal, uv),
			VulkanVertex(glm::rotate(quatHalf, vertices[2].Position), normal, uv),
		};

		int index = 0;
		for (int i = 0; i < 7; ++i)
		{
			vertices[index] = verts[i];
			indices[index] = index;
			++index;
		}

		quat quatAccumulator;

		for (int i = 1; i < 8; ++i)
		{
			quatAccumulator = quatAccumulator * quatFull;

			for (int pt = 0; pt < 7; ++pt)
			{
				vertices[index] = VulkanVertex(glm::rotate(quatAccumulator, verts[pt].Position), normal, uv);
				indices[index] = index;
				++index;
			}
		}

		ReverseWinding(vertices, indices);
	}

	void GeometryUtil::ReverseWinding(std::vector<VulkanVertex>& vertices, std::vector<unsigned int>& indices)
	{
		for (auto iter = indices.begin(); iter != indices.end(); iter += 3)
		{
			std::swap(*(iter), *(iter + 2));
		}

		for (auto iter = vertices.begin(); iter != vertices.end(); ++iter)
		{
			iter->TexCoord0.x = 1.0f - iter->TexCoord0.x;
		}
	}
}