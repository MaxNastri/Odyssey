#include "AnimationState.h"
#include "AnimationNodes.h"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "OdysseyTime.h"

namespace Odyssey
{
	AnimationLink::AnimationLink(std::shared_ptr<AnimationState> start, std::shared_ptr<AnimationState> end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer& targetValue)
		: m_GUID(GUID::New()), m_BeginState(start), m_EndState(end),
		m_Property(property), m_ComparisonOp(compareOp)

	{
		 RawBuffer::Copy(m_TargetValue, targetValue);
	}

	bool AnimationLink::Evaluate()
	{
		switch (m_ComparisonOp)
		{
			case Odyssey::ComparisonOp::Less:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() < m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() < m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() < m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::LessOrEqual:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() <= m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() <= m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() <= m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::Equal:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() == m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() == m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() == m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::Greater:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() > m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() > m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() > m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::GreaterOrEqual:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() >= m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() >= m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() >= m_TargetValue.Read<bool>();
				}
				break;
			}
		}

		return false;
	}

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