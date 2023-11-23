#pragma once
#include "glm.h"
#include "ResourceHandle.h"

namespace Odyssey
{
	class Material;
	class Mesh;
	class Scene;

	struct RenderObject
	{
	public:
		glm::mat4 WorldMatrix;
		ResourceHandle<Mesh> Mesh;
		ResourceHandle<Material> Material;
		bool IsValid = false;
	};

	class RenderScene
	{
	public:
		RenderScene() = default;
		RenderScene(Scene* scene);

	public:
		std::vector<RenderObject> m_RenderObjects;
		glm::mat4 m_CameraInverseView;
		glm::mat4 m_CameraProjection;
	};
}