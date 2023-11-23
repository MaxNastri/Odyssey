#include "RenderScene.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentManager.h"

namespace Odyssey
{
	RenderScene::RenderScene(Scene* scene)
	{
		if (Camera* mainCamera = scene->GetMainCamera())
		{
			m_CameraInverseView = mainCamera->GetInverseView();
			m_CameraProjection = mainCamera->GetProjection();
		}

		for (auto& gameObject : scene->gameObjects)
		{
			RenderObject renderObject;

			if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject))
			{
				renderObject.WorldMatrix = transform->GetWorldMatrix();
				renderObject.IsValid = true;
			}

			if (MeshRenderer* renderer = ComponentManager::GetComponent<MeshRenderer>(gameObject))
			{
				renderObject.Mesh = renderer->GetMesh();
				renderObject.Material = renderer->GetMaterial();
				renderObject.IsValid = true;
			}

			if (renderObject.IsValid)
			{
				m_RenderObjects.push_back(renderObject);
			}
		}
	}
}