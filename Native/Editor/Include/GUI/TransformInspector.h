#pragma once
#include <GUIElement.h>
#include "PropertyDrawer.h"
#include <GameObject.h>
#include <Vector3Drawer.h>

namespace Odyssey
{
	namespace Graphics
	{
		class GUIElement;
	}
}

namespace Odyssey::Editor
{
	class TransformInspector : public Graphics::GUIElement
	{
	public:
		TransformInspector() = default;
		TransformInspector(GameObject go);

	public:
		virtual void Draw() override;

	private:
		GameObject gameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};
}