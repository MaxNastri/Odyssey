#pragma once
#include <GUIElement.h>
#include <GameObject.h>
#include "Vector3Drawer.h"
#include "PropertyDrawer.h"

namespace Odyssey
{
	class GUIElement;

	class TransformInspector : public GUIElement
	{
	public:
		TransformInspector() = default;
		TransformInspector(RefHandle<GameObject> go);

	public:
		virtual void Draw() override;

	private:
		RefHandle<GameObject> gameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};
}