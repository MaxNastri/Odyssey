#include "AnimationBlueprint.h"
#include "AnimationClip.h"
#include "AnimationNodes.h"
#include "Enum.h"

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

				auto& animProperty = m_Properties.emplace_back(new AnimationProperty());

				std::string enumStr;
				propertyNode.ReadData("Name", animProperty->Name);
				propertyNode.ReadData("Type", enumStr);
				animProperty->Type = Enum::ToEnum<AnimationPropertyType>(enumStr);

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

				Ref<AnimationState> state = new AnimationState(nodeGUID, stateName, clipGUID);
				Ref<AnimationStateNode> node = AddNode<AnimationStateNode>(nodeGUID, stateName, state);
				node->SetInitialPosition(nodePos);

				if (!m_CurrentState)
					m_CurrentState = state;

				m_States[node->Guid] = state;
			}

			SerializationNode linksNode = root.GetNode("Animation Links");
			assert(linksNode.IsSequence());

			for (size_t i = 0; i < linksNode.ChildCount(); i++)
			{
				SerializationNode linkNode = linksNode.GetChild(i);
				assert(linkNode.IsMap());

				GUID linkGUID;
				GUID beginGUID;
				GUID endGUID;

				linkNode.ReadData("GUID", linkGUID.Ref());
				linkNode.ReadData("Begin State", beginGUID.Ref());
				linkNode.ReadData("End State", endGUID.Ref());

				// Create the link
				AddLink(linkGUID, beginGUID, endGUID);

				auto& beginState = m_States[beginGUID];
				auto& endState = m_States[endGUID];

				// Create the animtation link
				Ref<AnimationLink> animationLink = new AnimationLink(linkGUID, beginState, endState);
				m_StateToLinks[beginState].emplace_back(animationLink);
				m_StateToLinks[endState].emplace_back(animationLink);

				// Deserialize forward transitions
				{
					SerializationNode transitionsNode = linkNode.GetNode("Forward Transitions");
					assert(transitionsNode.IsSequence());

					for (size_t i = 0; i < transitionsNode.ChildCount(); i++)
					{
						SerializationNode conditionNode = transitionsNode.GetChild(i);
						assert(conditionNode.IsMap());

						std::string propertyName;
						std::string comparisonName;
						conditionNode.ReadData("Property", propertyName);
						conditionNode.ReadData("Comparison", comparisonName);

						ComparisonOp comparison = Enum::ToEnum<ComparisonOp>(comparisonName);
						RawBuffer valueBuffer;

						if (Ref<AnimationProperty> animationProperty = GetProperty(propertyName))
						{
							switch (animationProperty->Type)
							{
								case AnimationPropertyType::Trigger:
								case AnimationPropertyType::Bool:
								{
									bool value;
									conditionNode.ReadData("Value", value);
									RawBuffer::Copy(valueBuffer, &value, sizeof(value));

									break;
								}
								case AnimationPropertyType::Float:
								{
									float value;
									conditionNode.ReadData("Value", value);
									RawBuffer::Copy(valueBuffer, &value, sizeof(value));
									break;
								}
								case AnimationPropertyType::Int:
								{
									int32_t value;
									conditionNode.ReadData("Value", value);
									RawBuffer::Copy(valueBuffer, &value, sizeof(value));
									break;
								}
								default:
									break;
							}

							Ref<AnimationCondition> condition = new AnimationCondition(animationProperty, comparison, valueBuffer);
							animationLink->AddTransition(beginState, endState, condition);
						}
					}
				}


				// Deserialize forward transitions
				{
					SerializationNode transitionsNode = linkNode.GetNode("Return Transitions");
					assert(transitionsNode.IsSequence());

					for (size_t i = 0; i < transitionsNode.ChildCount(); i++)
					{
						SerializationNode conditionNode = transitionsNode.GetChild(i);
						assert(conditionNode.IsMap());

						std::string propertyName;
						std::string comparisonName;
						conditionNode.ReadData("Property", propertyName);
						conditionNode.ReadData("Comparison", comparisonName);

						ComparisonOp comparison = Enum::ToEnum<ComparisonOp>(comparisonName);
						RawBuffer valueBuffer;

						if (Ref<AnimationProperty> animationProperty = GetProperty(propertyName))
						{
							switch (animationProperty->Type)
							{
								case AnimationPropertyType::Trigger:
								case AnimationPropertyType::Bool:
								{
									bool value;
									conditionNode.ReadData("Value", value);
									RawBuffer::Copy(valueBuffer, &value, sizeof(value));

									break;
								}
								case AnimationPropertyType::Float:
								{
									float value;
									conditionNode.ReadData("Value", value);
									RawBuffer::Copy(valueBuffer, &value, sizeof(value));
									break;
								}
								case AnimationPropertyType::Int:
								{
									int32_t value;
									conditionNode.ReadData("Value", value);
									RawBuffer::Copy(valueBuffer, &value, sizeof(value));
									break;
								}
								default:
									break;
							}

							Ref<AnimationCondition> condition = new AnimationCondition(animationProperty, comparison, valueBuffer);
							animationLink->AddTransition(endState, beginState, condition);
						}
					}
				}
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

		SerializationNode linksNode = root.CreateSequenceNode("Animation Links");
		for (auto& [animState, animLinks] : m_StateToLinks)
		{
			for (Ref<AnimationLink>& animLink : animLinks)
			{
				SerializationNode linkNode = linksNode.AppendChild();
				linkNode.SetMap();

				GUID linkGUID = animLink->GetGUID();
				GUID beginState = animLink->GetBeginState()->GetGUID();
				GUID endState = animLink->GetEndState()->GetGUID();

				linkNode.WriteData("GUID", linkGUID.CRef());
				linkNode.WriteData("Begin State", beginState.CRef());
				linkNode.WriteData("End State", endState.CRef());

				// Serialize forward transitions
				{
					SerializationNode transitionsNode = linkNode.CreateSequenceNode("Forward Transitions");
					for (Ref<AnimationCondition> condition : animLink->GetForwardTransitions())
					{
						SerializationNode conditionNode = transitionsNode.AppendChild();
						conditionNode.SetMap();

						std::string propertyName = condition->GetPropertyName();
						ComparisonOp comparison = condition->GetComparison();
						conditionNode.WriteData("Property", propertyName);
						conditionNode.WriteData("Comparison", Enum::ToString(comparison));

						switch (condition->GetPropertyType())
						{
							case AnimationPropertyType::Trigger:
							case AnimationPropertyType::Bool:
							{
								bool value = condition->GetTargetValue<bool>();
								conditionNode.WriteData("Value", value);
								break;
							}
							case AnimationPropertyType::Float:
							{
								float value = condition->GetTargetValue<float>();
								conditionNode.WriteData("Value", value);
								break;
							}
							case AnimationPropertyType::Int:
							{
								int32_t value = condition->GetTargetValue<int32_t>();
								conditionNode.WriteData("Value", value);
								break;
							}
							default:
								break;
						}
					}
				}

				// Serialize Return transitions
				{
					SerializationNode transitionsNode = linkNode.CreateSequenceNode("Return Transitions");
					for (Ref<AnimationCondition> condition : animLink->GetReturnTransitions())
					{
						SerializationNode conditionNode = transitionsNode.AppendChild();
						conditionNode.SetMap();

						std::string propertyName = condition->GetPropertyName();
						ComparisonOp comparison = condition->GetComparison();
						conditionNode.WriteData("Property", propertyName);
						conditionNode.WriteData("Comparison", Enum::ToString(comparison));

						switch (condition->GetPropertyType())
						{
							case AnimationPropertyType::Trigger:
							case AnimationPropertyType::Bool:
							{
								bool value = condition->GetTargetValue<bool>();
								conditionNode.WriteData("Value", value);
								break;
							}
							case AnimationPropertyType::Float:
							{
								float value = condition->GetTargetValue<float>();
								conditionNode.WriteData("Value", value);
								break;
							}
							case AnimationPropertyType::Int:
							{
								int32_t value = condition->GetTargetValue<int32_t>();
								conditionNode.WriteData("Value", value);
								break;
							}
							default:
								break;
						}
					}
				}
			}
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
				if (animationLink->Evaluate(m_CurrentState))
					break;
			}
		}

		return m_CurrentState->Evaluate();
	}

	Ref<AnimationStateNode> AnimationBlueprint::AddAnimationState(std::string name)
	{
		// Create the animation state and node
		Ref<AnimationState> state = new AnimationState(name);
		auto node = AddNode<AnimationStateNode>(name, state);

		// Set the state's guid to match the node
		state->SetGUID(node->Guid);

		// Track the state by guid for later
		m_States[node->Guid] = state;

		// TEMP
		if (!m_CurrentState)
			m_CurrentState = state;

		return node.As<AnimationStateNode>();
	}

	Ref<AnimationProperty> AnimationBlueprint::GetProperty(const std::string& propertyName)
	{
		for (Ref<AnimationProperty>& animationProperty : m_Properties)
			if (animationProperty->Name == propertyName)
				return animationProperty;

		return Ref<AnimationProperty>();
	}

	Ref<AnimationState> AnimationBlueprint::GetAnimationState(GUID nodeGUID)
	{
		if (m_States.contains(nodeGUID))
			return m_States[nodeGUID];

		return nullptr;
	}

	Ref<AnimationLink> AnimationBlueprint::GetAnimationLink(GUID linkGUID)
	{
		for (auto& [animationState, animationLinks] : m_StateToLinks)
		{
			for (auto& animationLink : animationLinks)
			{
				if (animationLink->GetGUID() == linkGUID)
					return animationLink;
			}
		}

		return nullptr;
	}

	std::vector<std::string> AnimationBlueprint::GetAllPropertyNames()
	{
		std::vector<std::string> names;

		for (Ref<AnimationProperty> animationProperty : m_Properties)
			names.emplace_back(animationProperty->Name);

		return names;
	}

	void AnimationBlueprint::AddProperty(std::string_view name, AnimationPropertyType type)
	{
		m_Properties.emplace_back(new AnimationProperty(name, type));
	}

	bool AnimationBlueprint::SetBool(const std::string& name, bool value)
	{
		if (Ref<AnimationProperty> animationProperty = GetProperty(name))
		{
			animationProperty->ValueBuffer.Write(&value);
			return true;
		}

		return false;
	}

	bool AnimationBlueprint::SetFloat(const std::string& name, float value)
	{
		if (Ref<AnimationProperty> animationProperty = GetProperty(name))
		{
			animationProperty->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetInt(const std::string& name, int32_t value)
	{
		if (Ref<AnimationProperty> animationProperty = GetProperty(name))
		{
			animationProperty->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetTrigger(const std::string& name)
	{
		if (Ref<AnimationProperty> animationProperty = GetProperty(name))
		{
			bool trigger = true;
			animationProperty->ValueBuffer.Write(&trigger);
			return true;
		}
		return false;
	}

	void AnimationBlueprint::AddAnimationLink(GUID beginNode, GUID endNode, int32_t propertyIndex, ComparisonOp comparisonOp, RawBuffer& propertyValue)
	{
		if (m_States.contains(beginNode) && m_States.contains(endNode))
		{
			auto beginState = m_States[beginNode];
			auto endState = m_States[endNode];
			auto animProperty = m_Properties[propertyIndex];

			Ref<AnimationLink> animationLink = new AnimationLink(beginState, endState);
			Ref<AnimationCondition> condition = new AnimationCondition(animProperty, comparisonOp, propertyValue);
			animationLink->AddTransition(beginState, endState, condition);

			AddLink(animationLink->GetGUID(), beginNode, endNode);

			m_StateToLinks[beginState].push_back(animationLink);
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