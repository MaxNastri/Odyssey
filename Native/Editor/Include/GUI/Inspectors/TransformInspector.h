#pragma once
#include <GUIElement.h>
#include "Vector3Drawer.h"
#include "PropertyDrawer.h"

namespace Odyssey
{
	class GameObject;

	class TransformInspector : public GUIElement
	{
	public:
		TransformInspector() = default;
		TransformInspector(GameObject* go);

	public:
		virtual void Draw() override;

	private:
		static void OnPositionChanged(GameObject* gameObject, glm::vec3 position);
		static void OnRotationChanged(GameObject* gameObject, glm::vec3 rotation);
		static void OnScaleChanged(GameObject* gameObject, glm::vec3 scale);

	private:
		GameObject* m_GameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};
}