#pragma once
#include "AnimationClip.h"
#include "AnimationProperty.h"
#include "BoneKeyframe.h"
#include "GUID.h"
#include "Ref.h"
#include "Rune.h"

namespace Odyssey
{
	class AnimationState
	{
	public:
		AnimationState() = default;
		AnimationState(std::string_view name);
		AnimationState(GUID guid, std::string_view name, GUID animationClip);

	public:
		const std::map<std::string, BlendKey>& Evaluate();
		void Reset();

	public:
		GUID GetGUID() { return m_GUID; }
		std::string_view GetName();
		Ref<AnimationClip> GetClip();

	public:
		void SetGUID(GUID guid) { m_GUID = guid; }
		void SetName(std::string_view name) { m_Name = name; }
		void SetClip(GUID guid);

	private:
		Ref<AnimationClip> m_AnimationClip;
		std::string m_Name;
		GUID m_GUID;
	};
}