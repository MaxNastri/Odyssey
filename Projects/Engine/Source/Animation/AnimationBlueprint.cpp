#include "AnimationBlueprint.h"
#include "AnimationState.h"
#include "AnimationNodes.h"
#include "Enum.hpp"
#include "AnimationClip.h"

namespace Odyssey
{
	AnimationBlueprint::AnimationBlueprint()
		: Blueprint(), Asset()
	{

	}

	AnimationBlueprint::AnimationBlueprint(const Path& assetPath)
		: Blueprint(), Asset(assetPath)
	{
		Load();
	}

	void AnimationBlueprint::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void AnimationBlueprint::Load()
	{
		LoadFromDisk();
	}

	void AnimationBlueprint::LoadFromDisk()
	{
		AssetDeserializer deserializer(m_AssetPath);

		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			SerializationNode propertiesNode = root.GetNode("Properties");
			assert(propertiesNode.IsSequence());

			for (size_t i = 0; i < propertiesNode.ChildCount(); i++)
			{
				SerializationNode propertyNode = propertiesNode.GetChild(i);
				assert(propertyNode.IsMap());

				auto& animProperty = m_Properties.emplace_back(std::make_shared<AnimationProperty>());

				std::string enumStr;
				propertyNode.ReadData("Name", animProperty->Name);
				propertyNode.ReadData("Type", enumStr);
				animProperty->Type = Enum::ToEnum<AnimationPropertyType>(enumStr);

				m_PropertyMap[animProperty->Name] = animProperty;

				switch (animProperty->Type)
				{
					case AnimationPropertyType::Trigger:
					case AnimationPropertyType::Bool:
					{
						bool value = false;
						propertyNode.ReadData("Value", value);

						animProperty->ValueBuffer.Allocate(sizeof(bool));
						animProperty->ValueBuffer.Write(&value);
						break;
					}
					case AnimationPropertyType::Float:
					{
						float value = 0.0f;
						propertyNode.ReadData("Value", value);

						animProperty->ValueBuffer.Allocate(sizeof(float));
						animProperty->ValueBuffer.Write(&value);
						break;
					}
					case AnimationPropertyType::Int:
					{
						int32_t value = 0;
						propertyNode.ReadData("Value", value);

						animProperty->ValueBuffer.Allocate(sizeof(int32_t));
						animProperty->ValueBuffer.Write(&value);
						break;
					}
					default:
						break;
				}
			}

			SerializationNode statesNode = root.GetNode("Animation States");
			assert(statesNode.IsSequence());

			for (size_t i = 0; i < statesNode.ChildCount(); i++)
			{
				SerializationNode stateNode = statesNode.GetChild(i);
				assert(stateNode.IsMap());

				float2 nodePos = float2(0.0f);
				GUID nodeGUID;
				GUID clipGUID;
				std::string stateName;

				stateNode.ReadData("GUID", nodeGUID.Ref());
				stateNode.ReadData("Name", stateName);
				stateNode.ReadData("Clip", clipGUID.Ref());
				stateNode.ReadData("Position", nodePos);

				auto state = std::make_shared<AnimationState>(stateName, clipGUID);
				auto node = AddNode<AnimationStateNode>(nodeGUID, stateName, state);

				if (!m_CurrentState)
					m_CurrentState = state;

				m_States[node->Guid] = state;
			}
		}
	}

	void AnimationBlueprint::SaveToDisk(const Path& assetPath)
	{
		if (assetPath.empty())
			return;

		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		SerializationNode propertiesNode = root.CreateSequenceNode("Properties");

		for (auto& property : m_Properties)
		{
			SerializationNode propertyNode = propertiesNode.AppendChild();
			propertyNode.SetMap();

			std::string_view typeName = Enum::ToString<AnimationPropertyType>(property->Type);
			propertyNode.WriteData("Name", property->Name);
			propertyNode.WriteData("Type", typeName);

			switch (property->Type)
			{
				case AnimationPropertyType::Trigger:
				case AnimationPropertyType::Bool:
				{
					bool value = property->ValueBuffer.Read<bool>();
					propertyNode.WriteData("Value", value);
					break;
				}
				case AnimationPropertyType::Float:
				{
					float value = property->ValueBuffer.Read<float>();
					propertyNode.WriteData("Value", value);
					break;
				}
				case AnimationPropertyType::Int:
				{
					int32_t value = property->ValueBuffer.Read<int32_t>();
					propertyNode.WriteData("Value", value);
					break;
				}
				default:
					break;
			}
		}

		SerializationNode statesNode = root.CreateSequenceNode("Animation States");
		for (auto& [nodeGUID, state] : m_States)
		{
			SerializationNode stateNode = statesNode.AppendChild();
			stateNode.SetMap();

			GUID clipGUID = state->GetClip()->GetGUID();
			float2 nodePos = ImguiExt::GetNodePosition((uint64_t)nodeGUID);

			stateNode.WriteData("GUID", (uint64_t)nodeGUID);
			stateNode.WriteData("Name", state->GetName());
			stateNode.WriteData("Clip", clipGUID);
			stateNode.WriteData("Position", nodePos);
		}

		serializer.WriteToDisk(assetPath);
	}

	void AnimationBlueprint::Update()
	{
		ClearTriggers();
	}

	const std::map<std::string, BlendKey>& AnimationBlueprint::GetKeyframe()
	{
		if (m_StateToLinks.contains(m_CurrentState))
		{
			auto& links = m_StateToLinks[m_CurrentState];

			for (auto& animationLink : links)
			{
				if (animationLink->Evaluate())
				{
					m_CurrentState = animationLink->GetEndState();
					break;
				}
			}
		}

		return m_CurrentState->Evaluate();
	}

	std::shared_ptr<AnimationStateNode> AnimationBlueprint::AddAnimationState(std::string name)
	{
		auto state = std::make_shared<AnimationState>(name);
		auto node = AddNode<AnimationStateNode>(name, state);

		m_States[node->Guid] = state;

		if (!m_CurrentState)
			m_CurrentState = state;

		return std::static_pointer_cast<AnimationStateNode>(node);
	}

	std::shared_ptr<AnimationState> AnimationBlueprint::GetAnimationState(GUID nodeGUID)
	{
		if (m_States.contains(nodeGUID))
			return m_States[nodeGUID];

		return nullptr;
	}

	void AnimationBlueprint::AddProperty(std::string_view name, AnimationPropertyType type)
	{
		m_Properties.push_back(std::make_shared<AnimationProperty>(name, type));
	}

	bool AnimationBlueprint::SetBool(const std::string& name, bool value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetFloat(const std::string& name, float value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetInt(const std::string& name, int32_t value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetTrigger(const std::string& name)
	{
		if (m_PropertyMap.contains(name))
		{
			bool trigger = true;
			m_PropertyMap[name]->ValueBuffer.Write(&trigger);
			return true;
		}
		return false;
	}

	void AnimationBlueprint::AddAnimationLink(GUID startNode, GUID endNode, int32_t propertyIndex, ComparisonOp comparisonOp, RawBuffer& propertyValue)
	{
		if (m_States.contains(startNode) && m_States.contains(endNode))
		{
			auto startState = m_States[startNode];
			auto endState = m_States[endNode];
			auto animProperty = m_Properties[propertyIndex];

			m_StateToLinks[startState].push_back(std::make_shared<AnimationLink>(startState, endState, animProperty, comparisonOp, propertyValue));
		}
	}

	void AnimationBlueprint::ClearTriggers()
	{
		const bool clearTrigger = false;

		for (auto& property : m_Properties)
		{
			if (property->Type == AnimationPropertyType::Trigger)
			{
				property->ValueBuffer.Write(&clearTrigger);
			}
		}
	}
}