#include "ParticleEmitterInspector.h"
#include "RangeSlider.h"
#include "Material.h"

namespace Odyssey
{
	ParticleEmitterInspector::ParticleEmitterInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
		{
			m_LoopDrawer = BoolDrawer("Loop", emitter->Loop,
				[this](bool loop) { OnLoopModified(loop); });
			m_DurationDrawer = FloatDrawer("Duration", emitter->GetDuration(),
				[this](float duration) { OnDurationModified(duration); });
			m_EmissionRateDrawer = IntDrawer<uint32_t>("Emission Rate", emitter->EmissionRate,
				[this](uint32_t emissionRate) { OnEmissionRateModified(emissionRate); });
			m_MaterialDrawer = AssetFieldDrawer("Material", emitter->GetMaterial(), Material::Type,
				[this](GUID material) { OnMaterialModified(material); });
			m_LifetimeDrawer = RangeSlider("Lifetime", emitter->GetLifetime(), float2(0.1f, 10.0f), 0.1f,
				[this](float2 lifetime) { OnLifetimeModified(lifetime); });
			m_SizeDrawer = RangeSlider("Size", emitter->GetSize(), float2(0.1f, 10.0f), 0.1f,
				[this](float2 size) { OnSizeModified(size); });
			m_SpeedDrawer = RangeSlider("Speed", emitter->GetSpeed(), float2(0.1f, 10.0f), 0.1f,
				[this](float2 speed) { OnSpeedModified(speed); });
			m_ColorDrawer = Color4Drawer("Particle Color", emitter->GetColor(),
				[this](float4 color) { OnColorModified(color); });
		}
	}

	void ParticleEmitterInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Particle Emitter", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_LoopDrawer.Draw();
			m_DurationDrawer.Draw();
			m_EmissionRateDrawer.Draw();
			m_ColorDrawer.Draw();
			m_LifetimeDrawer.Draw();
			m_SizeDrawer.Draw();
			m_SpeedDrawer.Draw();
			m_MaterialDrawer.Draw();
		}
	}

	void ParticleEmitterInspector::OnLoopModified(bool loop)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->Loop = loop;
	}

	void ParticleEmitterInspector::OnDurationModified(float duration)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetDuration(duration);
	}

	void ParticleEmitterInspector::OnEmissionRateModified(uint32_t emissionRate)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->EmissionRate = emissionRate;
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

	void ParticleEmitterInspector::OnColorModified(float4 color)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetColor(color);
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