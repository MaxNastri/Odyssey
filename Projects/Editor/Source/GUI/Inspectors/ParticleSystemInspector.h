#pragma once
#include "ParticleSystem.h"
#include "Inspector.h"
#include "BoolDrawer.h"
#include "FloatDrawer.h"
#include "IntDrawer.h"

namespace Odyssey
{
	class ParticleSystemInspector : public Inspector
	{
	public:
		ParticleSystemInspector() = default;
		ParticleSystemInspector(GameObject& gameObject);

	public:
		virtual void Draw() override;

	private:
		void OnLoopModified(bool loop);
		void OnDurationModified(float duration);
		void OnEmissionRateModified(uint32_t emissionRate);

	private:
		GameObject m_GameObject;
		BoolDrawer m_LoopDrawer;
		FloatDrawer m_DurationDrawer;
		IntDrawer<uint32_t> m_EmissionRateDrawer;
	};
}