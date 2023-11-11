#pragma once
#include <GUIElement.h>
#include "PropertyDrawer.h"
#include <GameObject.h>

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
		void RegisterCallbacks();
		virtual void Draw() override;

	private:
		void OnPositionModified(Vector3 pos);
		void OnRotationModified(Vector3 pos);
		void OnScaleModified(Vector3 pos);

	private:
		Entities::GameObject gameObject;
		Vector3Drawer positionDrawer;
		Vector3Drawer rotationDrawer;
		Vector3Drawer scaleDrawer;
	};
}