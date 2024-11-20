#pragma once
#include "Inspector.h"
#include "Light.h"
#include "GameObject.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class LightInspector : public Inspector
	{
	public:
		LightInspector() = default;
		LightInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnLightTypeChanged(LightType lightType);
		void OnColorChanged(glm::vec3 color);
		void OnIntensityChanged(float intensity);
		void OnRangeChanged(float range);

	private:
		bool m_LightEnabled;
		GameObject m_GameObject;
		EnumDrawer<LightType> m_LightTypeDrawer;
		ColorPicker m_ColorPicker;
		FloatDrawer m_IntensityDrawer;
		FloatDrawer m_RangeDrawer;
	};
}