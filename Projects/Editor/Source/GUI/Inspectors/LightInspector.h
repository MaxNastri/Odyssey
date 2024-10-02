#pragma once
#include "Inspector.h"
#include "Light.h"
#include "GameObject.h"
#include "IntDrawer.h"
#include "FloatDrawer.h"
#include "ColorDrawer.h"

namespace Odyssey
{
	class LightInspector : public Inspector
	{
	public:
		LightInspector() = default;
		LightInspector(GameObject& gameObject);

	public:
		virtual void Draw() override;

	private:
		void OnLightTypeChanged(uint32_t lightType);
		void OnColorChanged(glm::vec3 color);
		void OnIntensityChanged(float intensity);
		void OnRangeChanged(float range);

	private:
		GameObject m_GameObject;

		IntDrawer<uint32_t> m_LightTypeDrawer;
		Color3Drawer m_ColorDrawer;
		FloatDrawer m_IntensityDrawer;
		FloatDrawer m_RangeDrawer;
	};
}