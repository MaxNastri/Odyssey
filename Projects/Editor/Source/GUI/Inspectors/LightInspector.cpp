#include "LightInspector.h"
#include "Light.h"

namespace Odyssey
{
	LightInspector::LightInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Light* light = m_GameObject.TryGetComponent<Light>())
		{
			m_LightEnabled = light->IsEnabled();
			m_LightTypeDrawer = EnumDrawer<LightType>("Light Type", light->GetType(),
				[this](LightType lightType) { OnLightTypeChanged(lightType); });
			m_ColorPicker = ColorPicker("Light Color", light->GetColor(),
				[this](glm::vec3 color) { OnColorChanged(color); });
			m_IntensityDrawer = FloatDrawer("Light Intensity", light->GetIntensity(),
				[this](float intensity) { OnIntensityChanged(intensity); });
			m_RangeDrawer = FloatDrawer("Light Range", light->GetRange(),
				[this](float range) { OnRangeChanged(range); });
		}
	}

	bool LightInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_LightEnabled))
		{
			if (Light* light = m_GameObject.TryGetComponent<Light>())
				light->SetEnabled(m_LightEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			modified |= m_LightTypeDrawer.Draw();
			modified |= m_ColorPicker.Draw();
			modified |= m_IntensityDrawer.Draw();
			modified |= m_RangeDrawer.Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void LightInspector::OnLightTypeChanged(LightType lightType)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetType(lightType);
	}

	void LightInspector::OnColorChanged(glm::vec3 color)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetColor(color);
	}

	void LightInspector::OnIntensityChanged(float intensity)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetIntensity(intensity);
	}

	void LightInspector::OnRangeChanged(float range)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetRange(range);
	}
}