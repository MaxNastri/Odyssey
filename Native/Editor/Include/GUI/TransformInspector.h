#pragma once
#include <GUIElement.h>
#include "PropertyDrawer.h"
#include <GameObject.h>
#include <Vector3Drawer.h>

namespace Odyssey
{
	namespace Entities
	{
		class Transform;
	}
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
		TransformInspector(Entities::GameObject go);

	public:
		virtual void Draw() override;

	private:
		Entities::GameObject gameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};
}