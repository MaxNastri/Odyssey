#pragma once
#include "ParticleEmitter.h"
#include "Inspector.h"
#include "BoolDrawer.h"
#include "FloatDrawer.h"
#include "IntDrawer.h"
#include "RangeSlider.h"
#include "ColorDrawer.h"

namespace Odyssey
{
	class ParticleEmitterInspector : public Inspector
	{
	public:
		ParticleEmitterInspector() = default;
		ParticleEmitterInspector(GameObject& gameObject);

	public:
		virtual void Draw() override;

	private:
		void OnLoopModified(bool loop);
		void OnDurationModified(float duration);
		void OnEmissionRateModified(uint32_t emissionRate);
		void OnLifetimeModified(float2 lifetime);
		void OnColorModified(float3 color);
		void OnSizeModified(float2 size);
		void OnSpeedModified(float2 color);

	private:
		GameObject m_GameObject;
		BoolDrawer m_LoopDrawer;
		FloatDrawer m_DurationDrawer;
		IntDrawer<uint32_t> m_EmissionRateDrawer;
		RangeSlider m_LifetimeDrawer;
		RangeSlider m_SizeDrawer;
		RangeSlider m_SpeedDrawer;
		ColorDrawer m_ColorDrawer;
	};
}