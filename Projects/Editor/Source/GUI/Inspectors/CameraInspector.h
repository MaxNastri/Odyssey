#pragma once
#include "Inspector.h"
#include "PropertyDrawers.h"
#include "PropertyDrawers.h"
#include "GameObject.h"

namespace Odyssey
{
	class GameObject;

	class CameraInspector : public Inspector
	{
	public:
		CameraInspector() = default;
		CameraInspector(GameObject& gameObject);

	public:
		virtual void Draw() override;

	private:
		void OnFieldOfViewChanged(float fov);
		void OnNearClipChanged(float nearClip);
		void OnFarClipChanged(float farClip);

	private:
		bool m_CameraEnabled;
		GameObject m_GameObject;
		FloatDrawer m_FieldOfViewDrawer;
		FloatDrawer m_NearClipDrawer;
		FloatDrawer m_FarClipDrawer;
	};
}