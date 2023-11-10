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
		void OnPositionModified(Vector4 pos);

	private:
		Entities::GameObject gameObject;
		Vector4Drawer positionDrawer;
	};
}