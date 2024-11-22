#include "AnimationState.h"
#include "AnimationNodes.h"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "OdysseyTime.h"

namespace Odyssey
{
	AnimationState::AnimationState(std::string_view name)
		: m_Name(name), m_GUID(GUID::New())
	{

	}

	AnimationState::AnimationState(GUID guid, std::string_view name, GUID animationClip)
		: m_Name(name), m_GUID(guid)
	{
		SetClip(animationClip);
	}

	const std::map<std::string, BlendKey>& AnimationState::Evaluate()
	{
		return m_AnimationClip->BlendKeys(Time::DeltaTime());
	}

	void AnimationState::Reset()
	{
		m_AnimationClip->Reset();
	}

	std::string_view AnimationState::GetName()
	{
		return m_Name;
	}

	std::shared_ptr<AnimationClip> AnimationState::GetClip()
	{
		return m_AnimationClip;
	}

	void AnimationState::SetClip(GUID guid)
	{
		m_AnimationClip = AssetManager::LoadAsset<AnimationClip>(guid);
	}
}