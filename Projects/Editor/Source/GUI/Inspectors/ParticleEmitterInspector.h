#pragma once
#include "ParticleEmitter.h"
#include "Inspector.h"
#include "BoolDrawer.h"
#include "FloatDrawer.h"
#include "IntDrawer.h"
#include "RangeSlider.h"
#include "ColorDrawer.h"
#include "AssetFieldDrawer.h"

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
		void OnMaterialModified(GUID material);
		void OnLifetimeModified(float2 lifetime);
		void OnColorModified(float4 color);
		void OnSizeModified(float2 size);
		void OnSpeedModified(float2 color);

	private:
		GameObject m_GameObject;
		BoolDrawer m_LoopDrawer;
		FloatDrawer m_DurationDrawer;
		IntDrawer<uint32_t> m_EmissionRateDrawer;
		AssetFieldDrawer m_MaterialDrawer;
		RangeSlider m_LifetimeDrawer;
		RangeSlider m_SizeDrawer;
		RangeSlider m_SpeedDrawer;
		Color4Drawer m_ColorDrawer;
	};
}