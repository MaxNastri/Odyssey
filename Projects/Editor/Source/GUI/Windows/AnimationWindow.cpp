#include "AnimationWindow.h"
#include "AnimationBlueprint.h"
#include "AssetManager.h"
#include "GUIManager.h"
#include "Input.h"
#include "Enum.h"
#include "FileDialogs.h"
#include "AnimationClip.h"
#include "AnimationState.h"
#include "AnimationNodes.h"

namespace Odyssey
{
	AnimationWindow::AnimationWindow(size_t windowID)
		: DockableWindow("Animation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_Blueprint = AssetManager::CreateAsset<AnimationBlueprint>("");
		m_WindowFlags = ImGuiWindowFlags_MenuBar;
		CreateBuilder();
	}

	void AnimationWindow::Destroy()
	{

	}

	void AnimationWindow::Update()
	{
		m_Blueprint->Update();
	}

	void AnimationWindow::Draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, float2(0.0f, 0.0f));

		if (!Begin())
		{
			ImGui::PopStyleVar();
			return;
		}

		// Draw the menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Blueprint"))
				{
					m_Blueprint = AssetManager::CreateAsset<AnimationBlueprint>("");
					CreateBuilder();
				}
				else if (ImGui::MenuItem("Open Blueprint"))
				{
					const Path& path = FileDialogs::OpenFile("Animation Blueprint", ".rune");
					if (!path.empty())
					{
						m_Blueprint = AssetManager::LoadAsset<AnimationBlueprint>(path);
						CreateBuilder();
					}
				}
				else if (ImGui::MenuItem("Save Blueprint"))
				{
					if (!m_Blueprint->GetAssetPath().empty())
					{
						m_Blueprint->Save();
					}
					else
					{
						const Path& path = FileDialogs::SaveFile("Animation Blueprint", ".rune");
						if (!path.empty())
						{
							m_Blueprint->SetAssetPath(path);
							m_Blueprint->Save();
						}
					}
				}
				else if (ImGui::MenuItem("Save Blueprint To..."))
				{
					Path path = FileDialogs::SaveFile("Animation Blueprint", ".rune");
					if (!path.empty())
					{
						if (!path.has_extension())
							path = path / Path(".rune");

						m_Blueprint->SetAssetPath(path);
						m_Blueprint->Save();
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Host a dockspace for the node editor and it's panels
		m_DockspaceID = ImGui::GetID("AnimationWindowDS");
		if (!ImGui::DockBuilderGetNode(m_DockspaceID))
		{
			ImGui::DockBuilderRemoveNode(m_DockspaceID);
			ImGui::DockBuilderAddNode(m_DockspaceID);
			ImGui::DockBuilderSetNodeSize(m_DockspaceID, m_WindowSize);
			ImGui::DockBuilderFinish(m_DockspaceID);
		}
		ImGui::DockSpace(m_DockspaceID);
		End();

		// Pop the window padding
		ImGui::PopStyleVar();

		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		// Draw the node editor
		if (ImGui::Begin("Node Editor"))
		{
			m_Builder->SetEditor();

			DrawPropertiesPanel();
			DrawSelectPropertyMenu();
			DrawAddPropertyMenu();
			DrawCreateNodeMenu();
			DrawAddLinkMenu();

			// Begin building the UI
			m_Builder->Begin();

			// Draw the blueprint
			m_Builder->DrawBlueprint();

			// End building the UI
			m_Builder->End();
			ImGui::End();
		}
	}

	void AnimationWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void AnimationWindow::CreateBuilder()
	{
		m_Builder = std::make_shared<BlueprintBuilder>(m_Blueprint.get());
		m_Builder->OverrideCreateNodeMenu(Create_Node_Menu_Name, Create_Node_Menu_ID);
		m_Builder->OverrideCreateLinkMenu(Add_Link_Menu_Name, Add_Link_Menu_ID);
	}

	void AnimationWindow::DrawPropertiesPanel()
	{
		constexpr float2 buttonSize = float2(25.0f, 25.0f);

		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		if (ImGui::Begin("Properties Panel"))
		{
			float2 textSize = ImGui::CalcTextSize("Property Editor") * 1.1f;
			ImGui::FilledRectSpanText("Property Editor", float4(1.0f), float4(0.15f, 0.15f, 0.15f, 1.0f), textSize.y, float2(1.0f, 0.0f));

			ImGui::BeginHorizontal("Property Editor");
			float panelWidth = ImGui::GetContentRegionMax().x;
			float panelHeight = ImGui::GetContentRegionMax().y;
			float panelLeft = ImGui::GetCursorPosX();
			auto style = ImGui::GetStyle();

			// + button
			float2 addButtonSize = ImGui::CalcTextSize("+") + style.FramePadding + style.ItemSpacing;
			float addButtonPosition = panelWidth - addButtonSize.x - style.FramePadding.x;

			ImGui::SetCursorPosX(addButtonPosition);
			if (ImGui::Button("+", addButtonSize))
				OpenSelectPropertyMenu();

			// Navigate button
			float2 navButtonSize = ImGui::CalcTextSize("Navigate") + style.FramePadding + style.ItemSpacing;
			float navButtonPos = addButtonPosition - navButtonSize.x - style.ItemSpacing.x;

			ImGui::SetCursorPosX(navButtonPos);
			if (ImGui::Button("Navigate", navButtonSize))
				ImguiExt::NavigateToContent(false);

			// Reset the cursor to the panel's left and draw the search label
			ImGui::SetCursorPosX(panelLeft);
			ImGui::TextUnformatted("Search");

			// Calculate the width of the search text box
			float currentPos = ImGui::GetCursorPosX();
			float searchWidth = navButtonPos - currentPos - style.ItemSpacing.x;

			// Draw the search text box
			std::string data;
			ImGui::PushItemWidth(searchWidth);
			ImGui::InputText("##SearchLabel", data.data(), data.size());
			ImGui::EndHorizontal();

			// Start drawing properties
			float inputWidth = 100.0f;
			float inputPos = panelWidth - inputWidth - style.FramePadding.x;
			float labelWidth = inputPos - style.ItemSpacing.x;

			labelWidth = (labelWidth + style.FramePadding.x + style.ItemSpacing.x) / panelWidth;

			auto& properties = m_Blueprint->GetProperties();

			if (ImGui::BeginTable("##PropertyTable", 2, ImGuiTableFlags_SizingMask_))
			{
				for (size_t i = 0; i < properties.size(); i++)
				{
					auto& animProperty = properties[i];

					ImGui::PushID((int32_t)i);

					// Name column
					ImGui::TableNextColumn();

					char buffer[128] = "";
					animProperty->Name.copy(buffer, 128);

					if (ImGui::SelectableInput("##PropertyLabel", false, ImGuiSelectableFlags_SpanAllColumns, buffer, ARRAYSIZE(buffer)))
						animProperty->Name = buffer;

					if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
					{
						int32_t direction = Input::GetMouseAxisVertical() > 0.0f ? 1 : -1;
						size_t next = i + direction;

						if (next >= 0 && next < properties.size())
						{
							std::swap(properties[i], properties[next]);
						}
					}

					// Widget column
					ImGui::TableNextColumn();

					ImGui::SetCursorPosX(inputPos);
					ImGui::SetNextItemWidth(inputWidth);

					switch (animProperty->Type)
					{
						case AnimationPropertyType::Float:
						{
							float data = animProperty->ValueBuffer.Read<float>();

							if (ImGui::InputFloat("##InputLabel", &data))
								animProperty->ValueBuffer.Write(&data, sizeof(float));

							break;
						}
						case AnimationPropertyType::Int:
						{
							int32_t data = animProperty->ValueBuffer.Read<int32_t>();

							if (ImGui::InputScalar("##InputLabel", ImGuiDataType_S32, &data))
								animProperty->ValueBuffer.Write(&data, sizeof(int32_t));

							break;
						}
						case AnimationPropertyType::Bool:
						{
							bool data = animProperty->ValueBuffer.Read<bool>();

							if (ImGui::Checkbox("##InputLabel", &data))
								animProperty->ValueBuffer.Write(&data, sizeof(bool));

							break;
						}
						case AnimationPropertyType::Trigger:
						{
							bool data = animProperty->ValueBuffer.Read<bool>();
							int radio = data;
							if (data = ImGui::RadioButton("##InputLabel", &radio, 1))
								animProperty->ValueBuffer.Write(&data, sizeof(bool));

							break;
						}
					}
					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			// Draw the node inspector panel within the properties panel
			DrawNodeInspectorPanel();

			ImGui::End();
		}
	}

	void AnimationWindow::DrawNodeInspectorPanel()
	{
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		static bool menu = true;

		if (ImGui::Begin("Node Inspector Panel"))
		{
			float2 textSize = ImGui::CalcTextSize("Hello world") * 1.1f;
			ImGui::FilledRectSpanText("Node Inspector", float4(1.0f), float4(0.15f, 0.15f, 0.15f, 1.0f), textSize.y, float2(1.0f, 0.0f));

			if (ImguiExt::HasSelectionChanged())
			{
				ImguiExt::NodeId node;
				ImguiExt::GetSelectedNodes(&node, 1);
				m_AnimationState = m_Blueprint->GetAnimationState((GUID)node.Get());

				if (m_AnimationState)
				{
					auto onAnimationClipChanged = [this](GUID guid)
						{
							if (m_AnimationState)
								m_AnimationState->SetClip(guid);
						};

					auto onStateNameChanged = [this](std::string_view name)
						{
							if (m_AnimationState)
								m_AnimationState->SetName(name);
						};

					GUID clipGUID;

					if (auto clip = m_AnimationState->GetClip())
						clipGUID = clip->GetGUID();

					m_AnimationClipDrawer = AssetFieldDrawer("Animation Clip", clipGUID, AnimationClip::Type, onAnimationClipChanged);
					m_StateNameDrawer = StringDrawer("State", m_AnimationState->GetName(), onStateNameChanged);
				}
				else
				{
					// No valid nodes selected, lets look for a selected link
					ImguiExt::LinkId link;
					ImguiExt::GetSelectedLinks(&link, 1);
					m_AnimationLink = m_Blueprint->GetAnimationLink((GUID)link.Get());

					if (m_AnimationLink)
					{
						auto onPropertyNameChanged = [this](std::string_view property, uint64_t index)
							{
								if (m_AnimationLink)
									m_AnimationLink->SetProperty(m_Blueprint->GetProperty(property.data()));
							};

						auto onComparisonOpChanged = [this](ComparisonOp op)
							{
								if (m_AnimationLink)
									m_AnimationLink->SetComparisonOp(op);
							};

						m_PropertyNameDrawer = DropdownDrawer("Property", m_Blueprint->GetAllPropertyNames(), m_AnimationLink->GetProperty()->Name, onPropertyNameChanged);
						m_ComparisonDrawer = EnumDrawer<ComparisonOp>("Comparison", m_AnimationLink->GetComparisonOp(), onComparisonOpChanged);

						auto animProperty = m_AnimationLink->GetProperty();
						switch (animProperty->Type)
						{
							case AnimationPropertyType::Float:
							{
								auto onLinkValueChanged = [this](float value)
									{
										if (m_AnimationLink && m_AnimationLink->GetProperty())
										{
											m_AnimationLink->SetFloat(value);
										}
									};
								m_LinkValueDrawer = std::make_unique<FloatDrawer>("Value", animProperty->ValueBuffer.Read<float>(), onLinkValueChanged);
								break;
							}
							case AnimationPropertyType::Int:
							{
								auto onLinkValueChanged = [this](int32_t value)
									{
										if (m_AnimationLink && m_AnimationLink->GetProperty())
										{
											m_AnimationLink->SetInt(value);
										}
									};
								m_LinkValueDrawer = std::make_unique<IntDrawer<int32_t>>("Value", animProperty->ValueBuffer.Read<int32_t>(), onLinkValueChanged);
								break;
							}
							case AnimationPropertyType::Bool:
							case AnimationPropertyType::Trigger:
							{
								auto onLinkValueChanged = [this](bool value)
									{
										if (m_AnimationLink && m_AnimationLink->GetProperty())
										{
											m_AnimationLink->SetBool(value);
										}
									};
								m_LinkValueDrawer = std::make_unique<BoolDrawer>("Value", animProperty->ValueBuffer.Read<bool>(), onLinkValueChanged);
								break;
							}
							default:
								break;
						}
					}
				}
			}

			if (m_AnimationState)
			{
				m_StateNameDrawer.Draw();
				m_AnimationClipDrawer.Draw();
			}
			else if (m_AnimationLink)
			{
				m_PropertyNameDrawer.Draw();
				m_ComparisonDrawer.Draw();
				m_LinkValueDrawer->Draw();
			}

			ImGui::End();
		}
	}

	void AnimationWindow::DrawSelectPropertyMenu()
	{
		ImGui::PushOverrideID(Select_Property_Menu_ID);

		if (ImGui::BeginPopup(Select_Property_Menu_Name))
		{
			if (ImGui::MenuItem("Float"))
				OpenAddPropertyMenu(AnimationPropertyType::Float);
			if (ImGui::MenuItem("Int"))
				OpenAddPropertyMenu(AnimationPropertyType::Int);
			if (ImGui::MenuItem("Bool"))
				OpenAddPropertyMenu(AnimationPropertyType::Bool);
			if (ImGui::MenuItem("Trigger"))
				OpenAddPropertyMenu(AnimationPropertyType::Trigger);

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AnimationWindow::DrawAddPropertyMenu()
	{
		ImGui::PushOverrideID(Add_Property_Menu_ID);

		if (ImGui::BeginPopup(Add_Property_Menu_Name))
		{
			ImGui::Text("Property Name:");

			// Send focus to the input text by default
			if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !Input::GetMouseButtonDown(MouseButton::Left)))
				ImGui::SetKeyboardFocusHere(0);

			ImGui::InputText("##PropName", m_AddPropertyBuffer, ARRAYSIZE(m_AddPropertyBuffer));

			ImGui::BeginHorizontal(Add_Property_Menu_ID);

			if (ImGui::Button("Add") || Input::GetKeyDown(KeyCode::Enter) || Input::GetKeyDown(KeyCode::KeypadEnter))
			{
				m_Blueprint->AddProperty(m_AddPropertyBuffer, m_AddPropertyType);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndHorizontal();

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AnimationWindow::DrawCreateNodeMenu()
	{
		ImGui::PushOverrideID(Create_Node_Menu_ID);

		if (ImGui::BeginPopup(Create_Node_Menu_Name))
		{
			ImGui::TextUnformatted("Create New Node");
			ImGui::Separator();

			if (ImGui::MenuItem("Animation State"))
			{
				std::shared_ptr<AnimationStateNode> node = m_Blueprint->AddAnimationState("State");
				m_Builder->ConnectNewNode(node.get());
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AnimationWindow::DrawAddLinkMenu()
	{
		ImGui::PushOverrideID(Add_Link_Menu_ID);

		if (ImGui::BeginPopup(Add_Link_Menu_Name))
		{
			ImGui::Text("Select Property:");

			auto& properties = m_Blueprint->GetProperties();
			const std::string selectedProperty = m_SelectedProperty >= 0 ? properties[m_SelectedProperty]->Name : "";

			if (ImGui::BeginCombo("##PropertyCombo", selectedProperty.c_str()))
			{
				for (size_t i = 0; i < properties.size(); i++)
				{
					auto& animProperty = properties[i];

					const bool selected = m_SelectedProperty == i;
					const std::string& name = animProperty->Name;

					if (ImGui::Selectable(name.c_str(), selected))
					{
						m_SelectedProperty = (int32_t)i;
						m_AddLinkPropertyValue.Allocate(animProperty->ValueBuffer.GetSize());

						// Triggers will always compare against true, so write it by default
						if (animProperty->Type == AnimationPropertyType::Trigger)
						{
							const bool trigger = true;
							m_AddLinkPropertyValue.Write(&trigger);
							m_SelectedComparisonOp = Enum::ToInt(ComparisonOp::Equal);
						}
						else if (animProperty->Type == AnimationPropertyType::Bool)
						{
							// Assign a default comparison op of Equal for bools
							m_SelectedComparisonOp = Enum::ToInt(ComparisonOp::Equal);
						}
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Only show the comparison op dropdown when a valid property is selected
			// Skip the comparison op UI for triggers
			if (m_SelectedProperty >= 0 && properties[m_SelectedProperty]->Type != AnimationPropertyType::Trigger)
			{
				// For bools always force the comparison op to Equals
				if (properties[m_SelectedProperty]->Type == AnimationPropertyType::Bool)
					m_SelectedComparisonOp = Enum::ToInt(ComparisonOp::Equal);

				// Get the display string
				const std::string comparisonOp = m_SelectedComparisonOp >= 0 ?
					Comparison_Op_Display[m_SelectedComparisonOp] : "";

				if (ImGui::BeginCombo("##CompareOpCombo", comparisonOp.c_str()))
				{
					// Skip the dropdown selection for bools
					if (properties[m_SelectedProperty]->Type != AnimationPropertyType::Bool)
					{
						for (size_t i = 0; i < Comparison_Op_Display.size(); i++)
						{
							const bool selected = m_SelectedComparisonOp == i;
							std::string_view display = Comparison_Op_Display[i];

							if (ImGui::Selectable(display.data(), selected))
								m_SelectedComparisonOp = (int32_t)i;
						}
					}

					ImGui::EndCombo();
				}
			}

			if (m_SelectedProperty >= 0 && m_SelectedComparisonOp >= 0)
			{
				auto& animProperty = properties[m_SelectedProperty];

				switch (animProperty->Type)
				{
					case AnimationPropertyType::Float:
					{
						float data = m_AddLinkPropertyValue.Read<float>();

						if (ImGui::InputFloat("##InputLabel", &data))
							m_AddLinkPropertyValue.Write(&data);

						break;
					}
					case AnimationPropertyType::Int:
					{
						int32_t data = m_AddLinkPropertyValue.Read<int32_t>();

						if (ImGui::InputScalar("##InputLabel", ImGuiDataType_S32, &data))
							m_AddLinkPropertyValue.Write(&data);

						break;
					}
					case AnimationPropertyType::Bool:
					{
						bool data = m_AddLinkPropertyValue.Read<bool>();
						const std::string boolValue = data ? "True" : "False";

						if (ImGui::BeginCombo("##BoolCombo", boolValue.c_str()))
						{
							if (ImGui::Selectable("True", data))
							{
								const bool trueValue = true;
								m_AddLinkPropertyValue.Write(&trueValue);
							}
							if (ImGui::Selectable("False", !data))
							{
								const bool falseValue = false;
								m_AddLinkPropertyValue.Write(&falseValue);
							}

							ImGui::EndCombo();
						}

						break;
					}
				}

				// Only allow the add button when the other 2 fields are filled
				if (ImGui::Button("Add") || Input::GetKeyDown(KeyCode::Enter) || Input::GetKeyDown(KeyCode::KeypadEnter))
				{
					GUID beginNode, endNode;
					if (m_Builder->GetPendingLinkNodes(beginNode, endNode))
					{
						m_Blueprint->AddAnimationLink(beginNode, endNode, m_SelectedProperty, (ComparisonOp)m_SelectedComparisonOp, m_AddLinkPropertyValue);
						m_Builder->ClearPendingLink();
					}
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
			}

			if (ImGui::Button("Cancel") || Input::GetKeyDown(KeyCode::Escape))
			{
				m_Builder->ClearPendingLink();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AnimationWindow::OpenSelectPropertyMenu()
	{
		ImGui::PushOverrideID(Select_Property_Menu_ID);
		ImGui::OpenPopup(Select_Property_Menu_Name);
		ImGui::PopID();
	}

	void AnimationWindow::OpenAddPropertyMenu(AnimationPropertyType propertyType)
	{
		// Clear the buffer of any previous data
		ZeroMemory(m_AddPropertyBuffer, ARRAYSIZE(m_AddPropertyBuffer));

		// Cache the property type
		m_AddPropertyType = propertyType;

		// Open the popup menu
		ImGui::PushOverrideID(Add_Property_Menu_ID);
		ImGui::OpenPopup(Add_Property_Menu_Name);
		ImGui::PopID();
	}

	void AnimationWindow::OpenAddLinkMenu()
	{
		ZeroMemory(m_AddLinkBuffer, ARRAYSIZE(m_AddLinkBuffer));
		m_SelectedProperty = -1;
		m_SelectedComparisonOp = -1;
		m_AddLinkPropertyValue.Free();
	}
}