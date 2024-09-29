#include "ParticleSystemInspector.h"

namespace Odyssey
{
	ParticleSystemInspector::ParticleSystemInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ParticleSystem* particleSystem = m_GameObject.TryGetComponent<ParticleSystem>())
		{
			m_LoopDrawer = BoolDrawer("Loop", particleSystem->Loop,
				[this](bool loop) { OnLoopModified(loop); });
			m_DurationDrawer = FloatDrawer("Duration", particleSystem->Duration,
				[this](float duration) { OnDurationModified(duration); });
			m_EmissionRateDrawer = IntDrawer<uint32_t>("Emission Rate", particleSystem->EmissionRate,
				[this](uint32_t emissionRate) { OnEmissionRateModified(emissionRate); });
		}
	}

	void ParticleSystemInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_LoopDrawer.Draw();
			m_DurationDrawer.Draw();
			m_EmissionRateDrawer.Draw();
		}
	}

	void ParticleSystemInspector::OnLoopModified(bool loop)
	{
		if (ParticleSystem* particleSystem = m_GameObject.TryGetComponent<ParticleSystem>())
			particleSystem->Loop = loop;
	}

	void ParticleSystemInspector::OnDurationModified(float duration)
	{
		if (ParticleSystem* particleSystem = m_GameObject.TryGetComponent<ParticleSystem>())
			particleSystem->Duration = duration;
	}

	void ParticleSystemInspector::OnEmissionRateModified(uint32_t emissionRate)
	{
		if (ParticleSystem* particleSystem = m_GameObject.TryGetComponent<ParticleSystem>())
			particleSystem->EmissionRate = emissionRate;
	}
}