#include "ParticleEmitterInspector.h"
#include "Material.h"

namespace Odyssey
{
	ParticleEmitterInspector::ParticleEmitterInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
		{
			m_EmitterEnabled = emitter->IsEnabled();
			m_LoopDrawer = BoolDrawer("Loop", emitter->IsLooping(), false,
				[this](bool loop) { OnLoopModified(loop); });
			m_DurationDrawer = FloatDrawer("Duration", emitter->GetDuration(),
				[this](float duration) { OnDurationModified(duration); });
			m_EmissionRateDrawer = IntDrawer<uint32_t>("Emission Rate", emitter->GetEmissionRate(), false,
				[this](uint32_t emissionRate) { OnEmissionRateModified(emissionRate); });
			m_MaterialDrawer = AssetFieldDrawer("Material", emitter->GetMaterial(), Material::Type,
				[this](GUID material) { OnMaterialModified(material); });
			m_LifetimeDrawer = RangeSlider("Lifetime", emitter->GetLifetime(), float2(0.1f, 10.0f), 0.1f,
				[this](float2 lifetime) { OnLifetimeModified(lifetime); });
			m_SizeDrawer = RangeSlider("Size", emitter->GetSize(), float2(0.1f, 10.0f), 0.1f,
				[this](float2 size) { OnSizeModified(size); });
			m_SpeedDrawer = RangeSlider("Speed", emitter->GetSpeed(), float2(0.1f, 10.0f), 0.1f,
				[this](float2 speed) { OnSpeedModified(speed); });
			m_StartColorDrawer = ColorPicker("Start Color", emitter->GetStartColor(),
				[this](float4 color) { OnStartColorModified(color); });
			m_EndColorDrawer = ColorPicker("End Color", emitter->GetEndColor(),
				[this](float4 color) { OnEndColorModified(color); });
		}
	}

	bool ParticleEmitterInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_EmitterEnabled))
		{
			if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
				emitter->SetEnabled(m_EmitterEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Particle Emitter", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			modified |= m_LoopDrawer.Draw();
			modified |= m_DurationDrawer.Draw();
			modified |= m_EmissionRateDrawer.Draw();
			modified |= m_StartColorDrawer.Draw();
			modified |= m_EndColorDrawer.Draw();
			modified |= m_LifetimeDrawer.Draw();
			modified |= m_SizeDrawer.Draw();
			modified |= m_SpeedDrawer.Draw();
			modified |= m_MaterialDrawer.Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void ParticleEmitterInspector::OnLoopModified(bool loop)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetLooping(loop);
	}

	void ParticleEmitterInspector::OnDurationModified(float duration)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetDuration(duration);
	}

	void ParticleEmitterInspector::OnEmissionRateModified(uint32_t emissionRate)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetEmissionRate(emissionRate);
	}

	void ParticleEmitterInspector::OnMaterialModified(GUID material)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetMaterial(material);
	}

	void ParticleEmitterInspector::OnLifetimeModified(float2 lifetime)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetLifetime(lifetime);
	}

	void ParticleEmitterInspector::OnStartColorModified(float4 color)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetStartColor(color);
	}

	void ParticleEmitterInspector::OnEndColorModified(float4 color)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetEndColor(color);
	}

	void ParticleEmitterInspector::OnSizeModified(float2 size)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetSize(size);
	}

	void ParticleEmitterInspector::OnSpeedModified(float2 speed)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetSpeed(speed);
	}
}