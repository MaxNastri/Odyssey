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

	std::map<std::string, BlendKey>& AnimationState::Evaluate()
	{
		return m_AnimationClip->BlendKeys(Time::DeltaTime());
	}

	std::map<std::string, BlendKey>& AnimationState::Evaluate(Ref<AnimationState>& prevState, float blendFactor)
	{
		std::map<std::string, BlendKey>& currentKeys = m_AnimationClip->BlendKeys(Time::DeltaTime());
		std::map<std::string, BlendKey>& prevKeys = prevState->Evaluate();

		for (auto& [boneName, currentKey] : currentKeys)
		{
			if (prevKeys.contains(boneName))
			{
				BlendKey& prevKey = prevKeys[boneName];

				currentKey.Position = glm::mix(prevKey.Position, currentKey.Position, blendFactor);
				currentKey.Rotation = glm::slerp(prevKey.Rotation, currentKey.Rotation, blendFactor);
				currentKey.Scale = glm::mix(prevKey.Scale, currentKey.Scale, blendFactor);
			}
		}

		return currentKeys;
	}

	void AnimationState::Reset()
	{
		m_AnimationClip->Reset();
	}

	std::string_view AnimationState::GetName()
	{
		return m_Name;
	}

	Ref<AnimationClip> AnimationState::GetClip()
	{
		return m_AnimationClip;
	}

	void AnimationState::SetClip(GUID guid)
	{
		m_AnimationClip = AssetManager::LoadAsset<AnimationClip>(guid);
	}
}