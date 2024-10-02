#include "LightInspector.h"
#include "Light.h"

namespace Odyssey
{
	LightInspector::LightInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Light* light = m_GameObject.TryGetComponent<Light>())
		{
			m_LightTypeDrawer = IntDrawer<uint32_t>("Light Type", (uint32_t)light->GetType(),
				[this](uint32_t lightType) { OnLightTypeChanged(lightType); });
			m_ColorDrawer = Color3Drawer("Light Color", light->GetColor(),
				[this](glm::vec3 color) { OnColorChanged(color); });
			m_IntensityDrawer = FloatDrawer("Light Intensity", light->GetIntensity(),
				[this](float intensity) { OnIntensityChanged(intensity); });
			m_RangeDrawer = FloatDrawer("Light Range", light->GetRange(),
				[this](float range) { OnRangeChanged(range); });
		}
	}
	void LightInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_LightTypeDrawer.Draw();
			m_ColorDrawer.Draw();
			m_IntensityDrawer.Draw();
			m_RangeDrawer.Draw();
		}
	}
	void LightInspector::OnLightTypeChanged(uint32_t lightType)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetType((LightType)lightType);
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