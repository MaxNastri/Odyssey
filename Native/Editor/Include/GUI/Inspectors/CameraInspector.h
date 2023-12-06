#pragma once
#include <GUIElement.h>
#include <GameObject.h>
#include "PropertyDrawer.h"
#include "FloatDrawer.h"

namespace Odyssey
{
	class CameraInspector : public GUIElement
	{
	public:
		CameraInspector() = default;
		CameraInspector(GameObject* go);

	public:
		virtual void Draw() override;

	private:
		GameObject* m_GameObject;
		FloatDrawer m_FieldOfViewDrawer;
		FloatDrawer m_NearClipDrawer;
		FloatDrawer m_FarClipDrawer;
	};
}