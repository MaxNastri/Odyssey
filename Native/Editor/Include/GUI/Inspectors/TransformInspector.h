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
		GameObject* gameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};
}