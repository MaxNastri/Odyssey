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
		CameraInspector(RefHandle<GameObject> go);

	public:
		virtual void Draw() override;

	private:
		RefHandle<GameObject> m_GameObject;
		FloatDrawer m_FieldOfViewDrawer;
		FloatDrawer m_NearClipDrawer;
		FloatDrawer m_FarClipDrawer;
	};
}