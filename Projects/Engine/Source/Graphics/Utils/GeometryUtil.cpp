#include "glm.h"
#include "GeometryUtil.h"
#include <utility>
#include "mikktspace.h"

namespace Odyssey
{
	struct BasicMesh
	{
		std::vector<Vertex>* Vertices;
		std::vector<uint32_t>* Indices;
	};

	inline static int32_t GetFaceCount(const SMikkTSpaceContext* context)
	{
		BasicMesh* mesh = (BasicMesh*)context->m_pUserData;

		return (int32_t)mesh->Indices->size() / 3;
	}

	inline static int32_t GetNumberOfFaceVertices(const SMikkTSpaceContext* context, int32_t face)
	{
		return 3;
	}

	inline static int32_t GetVertexIndex(const SMikkTSpaceContext* context, int32_t face, int32_t vertexIndex)
	{
		BasicMesh* mesh = (BasicMesh*)context->m_pUserData;

		int32_t faceSize = GetNumberOfFaceVertices(context, face);
		int32_t indicesIndex = (face * faceSize) + vertexIndex;

		int32_t index = (int32_t)((*mesh->Indices)[indicesIndex]);

		return index;
	}

	static void GetPosition(const SMikkTSpaceContext* context, float outpos[], int32_t face, int32_t vertexIndex)
	{
		BasicMesh* mesh = (BasicMesh*)context->m_pUserData;

		int32_t index = GetVertexIndex(context, face, vertexIndex);
		Vertex& vertex = (*mesh->Vertices)[index];

		outpos[0] = vertex.Position.x;
		outpos[1] = vertex.Position.y;
		outpos[2] = vertex.Position.z;
	}

	static void GetNormal(const SMikkTSpaceContext* context, float outnormal[], int32_t face, int32_t vertexIndex)
	{
		BasicMesh* mesh = (BasicMesh*)context->m_pUserData;

		int32_t index = GetVertexIndex(context, face, vertexIndex);
		Vertex& vertex = (*mesh->Vertices)[index];

		outnormal[0] = vertex.Normal.x;
		outnormal[1] = vertex.Normal.y;
		outnormal[2] = vertex.Normal.z;
	}

	static void GetTexCoord0(const SMikkTSpaceContext* context, float outuv[], int32_t face, int32_t vertexIndex)
	{
		BasicMesh* mesh = (BasicMesh*)context->m_pUserData;

		int32_t index = GetVertexIndex(context, face, vertexIndex);
		Vertex& vertex = (*mesh->Vertices)[index];

		outuv[0] = vertex.TexCoord0.x;
		outuv[1] = vertex.TexCoord0.y;
	}

	static void SetTangentSpaceBasic(const SMikkTSpaceContext* context, const float tangentu[], float fSign, int32_t face, int32_t vertexIndex)
	{
		BasicMesh* mesh = (BasicMesh*)context->m_pUserData;

		int32_t index = GetVertexIndex(context, face, vertexIndex);
		Vertex& vertex = (*mesh->Vertices)[index];

		vertex.Tangent.x = tangentu[0];
		vertex.Tangent.y = tangentu[1];
		vertex.Tangent.z = tangentu[2];
		vertex.Tangent.w = fSign;
	}

	void GeometryUtil::GenerateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		BasicMesh mesh({ &vertices, &indices });

		SMikkTSpaceInterface mikkInterface{};
		SMikkTSpaceContext context{};

		mikkInterface.m_getNumFaces = GetFaceCount;
		mikkInterface.m_getNumVerticesOfFace = GetNumberOfFaceVertices;

		mikkInterface.m_getPosition = GetPosition;
		mikkInterface.m_getNormal = GetNormal;
		mikkInterface.m_getTexCoord = GetTexCoord0;
		mikkInterface.m_setTSpaceBasic = SetTangentSpaceBasic;

		context.m_pInterface = &mikkInterface;
		context.m_pUserData = &mesh;

		genTangSpaceDefault(&context);
	}

	void GeometryUtil::ComputeBox(vec3 center, vec3 scale, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
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
			vertices.push_back(Vertex(position + center, normal, uvs[0]));

			// (normal - side1 + side2) * tsize // normal // t1
			position = (normal - side1 + side2) * extents;
			vertices.push_back(Vertex(position + center, normal, uvs[1]));

			// (normal + side1 + side2) * tsize // normal // t2
			position = (normal + side1 + side2) * extents;
			vertices.push_back(Vertex(position + center, normal, uvs[2]));

			// (normal + side1 - side2) * tsize // normal // t3
			position = (normal + side1 - side2) * extents;
			vertices.push_back(Vertex(position + center, normal, uvs[3]));
		}

		ReverseWinding(vertices, indices);
	}

	void GeometryUtil::ComputeSphere(float radius, unsigned int tesselation, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
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

				vertices.push_back(Vertex(normal * radius, normal, uv));
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

	void GeometryUtil::ComputeDiamond(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
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
		Vertex verts[8] =
		{
			Vertex(vec3(0.1f, 0, -1), normal, uv),
			Vertex(vec3(1, 0, 0), normal, uv),
			Vertex(vec3(1, 0, 0.1f), normal, uv),
			Vertex(vec3(0.4f, 0, 0.4f), normal, uv),
			Vertex(vec3(0.8f, 0, 0.3f), normal, uv),
			Vertex(glm::rotate(quatHalf, vertices[4].Position), normal, uv),
			Vertex(glm::rotate(quatHalf, vertices[1].Position), normal, uv),
			Vertex(glm::rotate(quatHalf, vertices[2].Position), normal, uv),
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
				vertices[index] = Vertex(glm::rotate(quatAccumulator, verts[pt].Position), normal, uv);
				indices[index] = index;
				++index;
			}
		}

		ReverseWinding(vertices, indices);
	}

	void GeometryUtil::ReverseWinding(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
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