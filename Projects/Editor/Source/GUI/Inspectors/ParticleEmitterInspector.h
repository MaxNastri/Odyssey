#pragma once
#include "ParticleEmitter.h"
#include "Inspector.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class ParticleEmitterInspector : public Inspector
	{
	public:
		ParticleEmitterInspector() = default;
		ParticleEmitterInspector(GameObject& gameObject);

	public:
		virtual bool Draw() override;

	private:
		void OnLoopModified(bool loop);
		void OnDurationModified(float duration);
		void OnEmissionRateModified(uint32_t emissionRate);
		void OnMaterialModified(GUID material);
		void OnLifetimeModified(float2 lifetime);
		void OnStartColorModified(float4 color);
		void OnEndColorModified(float4 color);
		void OnSizeModified(float2 size);
		void OnSpeedModified(float2 color);

	private:
		bool m_EmitterEnabled;
		GameObject m_GameObject;
		BoolDrawer m_LoopDrawer;
		FloatDrawer m_DurationDrawer;
		IntDrawer<uint32_t> m_EmissionRateDrawer;
		FloatDrawer m_RadiusDrawer;
		AssetFieldDrawer m_MaterialDrawer;
		RangeSlider m_LifetimeDrawer;
		RangeSlider m_SizeDrawer;
		RangeSlider m_SpeedDrawer;
		ColorPicker m_StartColorDrawer;
		ColorPicker m_EndColorDrawer;
	};
}