#include "TransformInspector.h"
#include <Transform.h>
#include <ComponentManager.h>

namespace Odyssey::Editor
{
	TransformInspector::TransformInspector(Entities::GameObject go)
	{
		using namespace Entities;

		gameObject = go;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(go))
		{
			positionDrawer = Vector4Drawer("Position", transform->position);
		}
	}

	void TransformInspector::Draw()
	{
		positionDrawer.Draw();
	}

	void TransformInspector::RegisterCallbacks()
	{
		std::function<void(Vector4)> positionModified = [this](Vector4 position)
			{
				OnPositionModified(position);
			};
		positionDrawer.SetCallback(positionModified);
	}

	void TransformInspector::OnPositionModified(Vector4 position)
	{
		using namespace Entities;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject))
		{
			transform->position = position;
		}
	}
}