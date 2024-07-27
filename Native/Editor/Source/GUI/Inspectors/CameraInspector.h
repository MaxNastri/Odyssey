#pragma once
#include "Inspector.h"
#include "PropertyDrawer.h"
#include "FloatDrawer.h"

namespace Odyssey
{
	class GameObject;

	class CameraInspector : public Inspector
	{
	public:
		CameraInspector() = default;
		CameraInspector(GameObject* gameObject);

	public:
		virtual void Draw() override;

	private:
		static void OnFieldOfViewChanged(GameObject* gameObject, float fov);
		static void OnNearClipChanged(GameObject* gameObject, float nearClip);
		static void OnFarClipChanged(GameObject* gameObject, float farClip);

	private:
		GameObject* m_GameObject;
		FloatDrawer m_FieldOfViewDrawer;
		FloatDrawer m_NearClipDrawer;
		FloatDrawer m_FarClipDrawer;
	};
}