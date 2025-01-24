#include "Inspectors.h"
#include "AssetManager.h"
#include "Preferences.h"
#include "Project.h"
#include "FBXAssetImporter.h"
#include "Renderer.h"
#include "Cubemap.h"
#include "SceneManager.h"
#include "ScriptingManager.h"
#include "PropertiesComponent.h"
#include "Events.h"
#include "Input.h"

namespace Odyssey
{
	namespace Utils
	{
		inline bool s_ComponentsRegistered = false;
		inline std::unordered_map<std::string, std::function<void(GameObject&)>> s_AddComponentFuncs;
		inline std::map<uint32_t, std::function<Ref<Inspector>(GameObject&)>> s_CreateInspectorFuncs;

		template<typename ComponentType, typename InspectorType>
		void RegisterComponentType(uint32_t priority)
		{
			static_assert(std::is_base_of<Inspector, InspectorType>::value, "InspectorType is not a dervied class of Inspector.");

			// Register an add component function
			if (!s_AddComponentFuncs.contains(ComponentType::ClassName))
			{
				s_AddComponentFuncs[ComponentType::ClassName] = [](GameObject& gameObject)
					{
						if (!gameObject.HasComponent<ComponentType>())
							gameObject.AddComponent<ComponentType>();
					};

				// No duplicate priorities
				if (s_CreateInspectorFuncs.contains(priority))
				{
					Log::Warning("[GameObjectInspector] Duplicate priority (" + std::to_string(priority) + " detected for " + ComponentType::ClassName);
				}

				s_CreateInspectorFuncs[priority] = [](GameObject& gameObject)
					{
						if (gameObject.HasComponent<ComponentType>())
							return Ref<InspectorType>(new InspectorType(gameObject));

						return Ref<InspectorType>();
					};
			}
		}

		// Helper function for when we want to register at lowest current priority
		template<typename ComponentType, typename InspectorType>
		void RegisterComponentType()
		{
			static uint32_t lowPri = 0;

			if (!s_CreateInspectorFuncs.empty())
			{
				lowPri = std::prev(s_CreateInspectorFuncs.end())->first + 1;
			}

			RegisterComponentType<ComponentType, InspectorType>(lowPri);
		}

		template<typename T>
		void OnFieldChanged(GUID guid, uint32_t scriptID, uint32_t fieldID, T newValue)
		{
			auto& storage = ScriptingManager::GetScriptStorage(guid);

			// Validate we are working on the same script
			if (storage.ScriptID != scriptID)
				return;

			// Look through the field storage for the matching field
			for (auto& [storedFieldID, fieldStorage] : storage.Fields)
			{
				if (fieldID == storedFieldID)
				{
					fieldStorage.SetValue<T>(newValue);
					break;
				}
			}
		}
	}

	AnimatorInspector::AnimatorInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
		{
			m_AnimatorEnabled = animator->IsEnabled();

			m_RigDrawer = AssetFieldDrawer("Rig", animator->GetRigAsset(), AnimationRig::Type,
				[this](GUID guid) { OnRigModified(guid); });

			m_BlueprintDrawer = AssetFieldDrawer("Blueprint", animator->GetBlueprintAsset(), AnimationBlueprint::Type,
				[this](GUID guid) { OnBlueprintModified(guid); });

			m_DebugEnabledDrawer = BoolDrawer("Debug", false, false,
				[this](bool enabled) { OnDebugEnabledModified(enabled); });
		}
	}

	bool AnimatorInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_AnimatorEnabled))
		{
			if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
				animator->SetEnabled(m_AnimatorEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_RigDrawer.Draw();
			m_BlueprintDrawer.Draw();
			m_DebugEnabledDrawer.Draw();

			if (ImGui::Button("Play"))
			{
				if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
					animator->Play();
				modified = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Pause"))
			{
				if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
					animator->Pause();
				modified = true;
			}
		}

		ImGui::PopID();

		return modified;
	}

	void AnimatorInspector::OnRigModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetRig(guid);
	}

	void AnimatorInspector::OnBlueprintModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetBlueprint(guid);
	}

	void AnimatorInspector::OnDebugEnabledModified(bool enabled)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetDebugEnabled(enabled);
	}

	CameraInspector::CameraInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			m_CameraEnabled = camera->IsEnabled();
			m_FieldOfViewDrawer = FloatDrawer("Field of View", camera->GetFieldOfView(),
				[this](float fov) { OnFieldOfViewChanged(fov); });

			m_NearClipDrawer = FloatDrawer("Near Clip", camera->GetNearClip(),
				[this](float nearClip) { OnNearClipChanged(nearClip); });

			m_FarClipDrawer = FloatDrawer("Far Clip", camera->GetFarClip(),
				[this](float farClip) { OnFarClipChanged(farClip); });
		}
	}

	bool CameraInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_CameraEnabled))
		{
			if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
				camera->SetEnabled(m_CameraEnabled);
			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			modified |= m_FieldOfViewDrawer.Draw();
			modified |= m_NearClipDrawer.Draw();
			modified |= m_FarClipDrawer.Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void CameraInspector::OnFieldOfViewChanged(float fov)
	{
		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			camera->SetFieldOfView(fov);
		}
	}

	void CameraInspector::OnNearClipChanged(float nearClip)
	{
		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			camera->SetNearClip(nearClip);
		}
	}

	void CameraInspector::OnFarClipChanged(float farClip)
	{
		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			camera->SetFarClip(farClip);
		}
	}

	LightInspector::LightInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Light* light = m_GameObject.TryGetComponent<Light>())
		{
			m_LightEnabled = light->IsEnabled();
			m_LightTypeDrawer = EnumDrawer<LightType>("Light Type", light->GetType(),
				[this](LightType lightType) { OnLightTypeChanged(lightType); });
			m_ColorPicker = ColorPicker("Light Color", light->GetColor(),
				[this](glm::vec3 color) { OnColorChanged(color); });
			m_IntensityDrawer = FloatDrawer("Light Intensity", light->GetIntensity(),
				[this](float intensity) { OnIntensityChanged(intensity); });
			m_RangeDrawer = FloatDrawer("Light Range", light->GetRange(),
				[this](float range) { OnRangeChanged(range); });
		}
	}

	bool LightInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_LightEnabled))
		{
			if (Light* light = m_GameObject.TryGetComponent<Light>())
				light->SetEnabled(m_LightEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::IsItemHovered() && Input::GetMouseButtonDown(MouseButton::Right))
				ImGui::OpenPopup("Component Context Menu");

			modified |= m_LightTypeDrawer.Draw();
			modified |= m_ColorPicker.Draw();
			modified |= m_IntensityDrawer.Draw();
			modified |= m_RangeDrawer.Draw();
		}
		else
		{
			if (ImGui::IsItemHovered() && Input::GetMouseButtonDown(MouseButton::Right))
				ImGui::OpenPopup("Component Context Menu");
		}

		if (ImGui::BeginPopup("Component Context Menu"))
		{
			if (ImGui::MenuItem("Remove Component"))
				m_GameObject.RemoveComponent<Light>();

			ImGui::EndPopup();
		}
		ImGui::PopID();

		return modified;
	}

	void LightInspector::OnLightTypeChanged(LightType lightType)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetType(lightType);
	}

	void LightInspector::OnColorChanged(glm::vec3 color)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetColor(color);
	}

	void LightInspector::OnIntensityChanged(float intensity)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetIntensity(intensity);
	}

	void LightInspector::OnRangeChanged(float range)
	{
		if (Light* light = m_GameObject.TryGetComponent<Light>())
			light->SetRange(range);
	}

	GameObjectInspector::GameObjectInspector(GUID guid)
	{
		m_TargetGUID = guid;
		m_Target = SceneManager::GetActiveScene()->GetGameObject(guid);

		// Note: Priority parameter determines the display order
		if (!Utils::s_ComponentsRegistered)
		{
			Utils::RegisterComponentType<Transform, TransformInspector>();
			Utils::RegisterComponentType<Camera, CameraInspector>();
			Utils::RegisterComponentType<Light, LightInspector>();
			Utils::RegisterComponentType<MeshRenderer, MeshRendererInspector>();
			Utils::RegisterComponentType<SpriteRenderer, SpriteRendererInspector>();
			Utils::RegisterComponentType<Animator, AnimatorInspector>();
			Utils::RegisterComponentType<ParticleEmitter, ParticleEmitterInspector>();
			Utils::RegisterComponentType<ScriptComponent, ScriptInspector>();
			Utils::RegisterComponentType<BoxCollider, BoxColliderInspector>();
			Utils::RegisterComponentType<CapsuleCollider, CapsuleColliderInspector>();
			Utils::RegisterComponentType<SphereCollider, SphereColliderInspector>();
			Utils::RegisterComponentType<RigidBody, RigidBodyInspector>();
			Utils::RegisterComponentType<CharacterController, CharacterControllerInspector>();
			Utils::s_ComponentsRegistered = true;
		}

		auto onSceneModified = [this](SceneModifiedEvent* eventData) { OnSceneModified(eventData); };
		m_OnSceneModifiedListener = EventSystem::Listen<SceneModifiedEvent>(onSceneModified);

		CreateInspectors();
	}

	GameObjectInspector::~GameObjectInspector()
	{
		EventSystem::RemoveListener<SceneModifiedEvent>(m_OnSceneModifiedListener);
	}

	bool GameObjectInspector::Draw()
	{
		bool modified = false;

		// Don't draw unless we have a target
		if (!m_Target.IsValid())
			return modified;

		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
			m_NameDrawer.Draw();

		ImGui::Spacing();

		for (auto& inspector : m_Inspectors)
		{
			modified |= inspector->Draw();
		}

		for (auto& userScriptInspector : userScriptInspectors)
		{
			modified |= userScriptInspector.Draw();
		}

		ImGui::Spacing();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component Popup");

		if (ImGui::BeginPopup("Add Component Popup"))
		{
			auto kv = std::views::keys(Utils::s_AddComponentFuncs);
			std::vector<std::string> possibleComponents{ kv.begin(), kv.end() };

			uint64_t selected = 0;

			for (size_t i = 0; i < possibleComponents.size(); i++)
			{
				const std::string& componentName = possibleComponents[i];
				if (ImGui::Selectable(componentName.c_str()))
				{
					selected = i;
					Utils::s_AddComponentFuncs[componentName](m_Target);
					CreateInspectors();
					modified = true;
				}
			}

			// No component was selected
			if (selected == 0)
			{
				auto& scriptMetadatas = ScriptingManager::GetAllScriptMetadatas();

				for (auto& [scriptID, metadata] : scriptMetadatas)
				{
					if (ImGui::Selectable(metadata.Name.c_str()))
					{
						if (!m_Target.HasComponent<ScriptComponent>())
							m_Target.AddComponent<ScriptComponent>(metadata.ScriptID);

						CreateInspectors();
						modified = true;
					}
				}
			}

			ImGui::EndPopup();
		}

		return modified;
	}

	void GameObjectInspector::CreateInspectors()
	{
		m_Inspectors.clear();
		userScriptInspectors.clear();

		m_NameDrawer = StringDrawer("Name", m_Target.GetName(), false,
			[this](std::string_view name) { OnNameChanged(name); });

		for (auto& [className, createInspectorFunc] : Utils::s_CreateInspectorFuncs)
		{
			auto inspector = createInspectorFunc(m_Target);
			if (inspector)
				m_Inspectors.push_back(std::move(inspector));
		}
	}

	void GameObjectInspector::OnNameChanged(std::string_view name)
	{
		if (m_Target.HasComponent<PropertiesComponent>())
			m_Target.SetName(name);
	}

	void GameObjectInspector::OnSceneModified(SceneModifiedEvent* eventData)
	{
		m_Target = eventData->Scene->GetGameObject(m_TargetGUID);

		if (m_Target.IsValid())
		{
			CreateInspectors();
		}
		else
		{
			m_Inspectors.clear();
			userScriptInspectors.clear();
		}
	}

	MaterialInspector::MaterialInspector(GUID guid)
	{
		if (m_Material = AssetManager::LoadAsset<Material>(guid))
		{
			GUID shaderGUID;
			GUID colorTextureGUID;
			GUID normalTextureGUID;
			GUID noiseTextureGUID;

			if (Ref<Shader> shader = m_Material->GetShader())
				shaderGUID = shader->GetGUID();

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Material->GetName(), false);
			m_ShaderDrawer = AssetFieldDrawer("Shader", shaderGUID, Shader::Type);
			m_ColorTextureDrawer = AssetFieldDrawer("Color Texture", colorTextureGUID, Texture2D::Type);
			m_NormalTextureDrawer = AssetFieldDrawer("Normal Texture", normalTextureGUID, Texture2D::Type);
			m_NoiseTextureDrawer = AssetFieldDrawer("Noise Texture", noiseTextureGUID, Texture2D::Type);
			m_EmissiveColorDrawer = ColorPicker("Emissive Color", m_Material->GetEmissiveColor());
			m_EmissivePowerDrawer = FloatDrawer("Emissive Power", m_Material->GetEmissivePower());
			m_AlphaClipDrawer = FloatDrawer("Alpha Clip", m_Material->GetAlphaClip());
			m_RenderQueueDrawer = EnumDrawer<RenderQueue>("Render Queue", m_Material->GetRenderQueue());
			m_BlendModeDrawer = EnumDrawer<BlendMode>("Blend Mode", m_Material->GetBlendMode());
			m_DepthWriteDrawer = BoolDrawer("Depth Write", m_Material->GetDepthWrite());
		}
	}

	bool MaterialInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();

		if (m_NameDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
		}

		if (m_ShaderDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;

			if (auto shader = AssetManager::LoadAsset<Shader>(m_ShaderDrawer.GetGUID()))
				m_Material->SetShader(shader);
		}

		if (m_EmissiveColorDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
			m_Material->SetEmissiveColor(m_EmissiveColorDrawer.GetColor3());
		}

		if (m_EmissivePowerDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
			m_Material->SetEmissivePower(m_EmissivePowerDrawer.GetValue());
		}

		if (m_RenderQueueDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
			m_Material->SetRenderQueue(m_RenderQueueDrawer.GetValue());
		}

		if (m_BlendModeDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
			m_Material->SetBlendMode(m_BlendModeDrawer.GetValue());
		}

		if (m_AlphaClipDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
			m_Material->SetAlphaClip(m_AlphaClipDrawer.GetValue());
		}

		if (m_DepthWriteDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
			m_Material->SetDepthWrite(m_DepthWriteDrawer.GetValue());
		}

		Ref<Shader> shader = m_Material->GetShader();
		auto& shaderBindings = shader->GetBindings();
		auto textures = m_Material->GetTextures();

		for (auto& [propertyName, shaderBinding] : shaderBindings)
		{
			if (shaderBinding.DescriptorType == DescriptorType::Sampler)
			{
				std::string displayName = propertyName;
				displayName[0] = std::toupper(displayName[0]);

				// Remove any reference to sampler for the display name
				size_t pos = displayName.find("Sampler");
				if (pos != std::string::npos)
					displayName = displayName.substr(0, pos);

				pos = displayName.find("Texture");
				if (pos != std::string::npos)
				{
					// Split the texture into its own word with a captial T
					std::string texture = displayName.substr(pos, displayName.length());
					texture[0] = std::toupper(texture[0]);
					displayName = displayName.substr(0, pos) + " " + texture;
				}
				else
				{
					displayName = displayName + " Texture";
				}


				GUID textureGUID = textures.contains(propertyName) ? textures[propertyName]->GetGUID() : GUID::Empty();
				AssetFieldDrawer textureDrawer = AssetFieldDrawer(displayName, textureGUID, Texture2D::Type);

				if (textureDrawer.Draw())
				{
					m_Dirty = true;
					modified = true;
					m_Material->SetTexture(propertyName, textureDrawer.GetGUID());
				}
			}
		}

		if (m_Dirty && ImGui::Button("Save"))
		{
			if (m_Material->GetName() != m_NameDrawer.GetValue())
				m_Material->SetName(m_NameDrawer.GetValue());

			m_Material->Save();
			m_Dirty = false;
		}

		return modified;
	}

	MeshInspector::MeshInspector(GUID guid)
	{
		if (m_Mesh = AssetManager::LoadAsset<Mesh>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Mesh->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Mesh->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Mesh->GetType(), true);
			m_VertexCountDrawer = StringDrawer("Vertex Count", std::to_string(m_Mesh->GetVertexCount()), true);
			m_IndexCountDrawer = StringDrawer("Index Count", std::to_string(m_Mesh->GetIndexCount()), true);
			m_SourceMeshDrawer = AssetFieldDrawer("Source Asset", m_Mesh->GetSourceAsset(), SourceModel::Type,
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
		}
	}

	bool MeshInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_TypeDrawer.Draw();
		modified |= m_VertexCountDrawer.Draw();
		modified |= m_IndexCountDrawer.Draw();
		modified |= m_SourceMeshDrawer.Draw();

		return modified;
	}

	void MeshInspector::OnNameChanged(std::string_view name)
	{
		if (m_Mesh)
		{
			m_Mesh->SetName(name);
			m_Mesh->Save();
		}
	}

	void MeshInspector::OnSourceAssetChanged(GUID sourceGUID)
	{
		if (m_Mesh)
		{
			m_Mesh->SetSourceAsset(sourceGUID);
			m_Mesh->Save();
		}
	}

	MeshRendererInspector::MeshRendererInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}

	bool MeshRendererInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_MeshRendererEnabled))
		{
			if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
				meshRenderer->SetEnabled(m_MeshRendererEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_MeshDrawer.Draw())
			{
				if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					meshRenderer->SetMesh(m_MeshDrawer.GetGUID());

				modified = true;
			}

			for (size_t i = 0; i < m_MaterialDrawers.size(); i++)
			{
				if (m_MaterialDrawers[i].Draw())
				{
					if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
						meshRenderer->SetMaterial(m_MaterialDrawers[i].GetGUID(), i);

					modified = true;
				}
			}

			if (ImGui::Button("Add Material"))
			{
				size_t materialIndex = m_MaterialDrawers.size();
				m_MaterialDrawers.emplace_back(AssetFieldDrawer(std::format("Material {}", materialIndex), 0, Material::Type));
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove Last Material"))
			{
				if (m_MaterialDrawers.size() > 0)
					m_MaterialDrawers.erase(m_MaterialDrawers.end() - 1);

				if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					meshRenderer->RemoveMaterial();
			}

			struct Data
			{
				GUID Mesh;
				GUID* Materials = nullptr;
				size_t MaterialCount = 0;
			};

			if (ImGui::Button("Copy"))
			{
				Data data;
				data.Mesh = m_MeshDrawer.GetGUID();
				data.MaterialCount = m_MaterialDrawers.size();
				data.Materials = new GUID[data.MaterialCount];

				for (size_t i = 0; i < m_MaterialDrawers.size(); i++)
				{
					data.Materials[i] = m_MaterialDrawers[i].GetGUID();
				}

				ClipBoard::Copy("Mesh Renderer", &data, sizeof(data));
			}

			ImGui::SameLine();

			if (ImGui::Button("Paste"))
			{
				if (ClipBoard::GetContext() == "Mesh Renderer")
				{
					if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					{
						Data data = ClipBoard::Paste().Read<Data>();

						meshRenderer->SetMesh(data.Mesh);

						for (size_t i = 0; i < data.MaterialCount; i++)
						{
							meshRenderer->SetMaterial(data.Materials[i], i);
						}

						InitDrawers();
					}
				}
			}
		}

		ImGui::PopID();

		return modified;
	}

	void MeshRendererInspector::InitDrawers()
	{
		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
		{
			GUID meshGUID = meshRenderer->GetMesh() ? meshRenderer->GetMesh()->GetGUID() : GUID(0);

			m_MeshRendererEnabled = meshRenderer->IsEnabled();

			m_MeshDrawer = AssetFieldDrawer("Mesh", meshGUID, Mesh::Type);
			m_MaterialDrawers.clear();

			auto& materials = meshRenderer->GetMaterials();
			for (size_t i = 0; i < materials.size(); i++)
			{
				m_MaterialDrawers.emplace_back(AssetFieldDrawer(std::format("Material {}", i), materials[i]->GetGUID(), Material::Type));
			}
		}
	}

	ParticleEmitterInspector::ParticleEmitterInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
		{
			m_EmitterEnabled = emitter->IsEnabled();
			m_LoopDrawer = BoolDrawer("Loop", emitter->IsLooping(), false,
				[this](bool loop) { OnLoopModified(loop); });
			m_DurationDrawer = FloatDrawer("Duration", emitter->GetDuration(),
				[this](float duration) { OnDurationModified(duration); });
			m_EmissionRateDrawer = IntDrawer<uint32_t>("Emission Rate", emitter->GetEmissionRate(), false,
				[this](uint32_t emissionRate) { OnEmissionRateModified(emissionRate); });
			m_RadiusDrawer = FloatDrawer("Radius", emitter->GetRadius());
			m_AngleDrawer = FloatDrawer("Angle", emitter->GetAngle());

			m_MaterialDrawer = AssetFieldDrawer("Material", emitter->GetMaterial(), Material::Type,
				[this](GUID material) { OnMaterialModified(material); });
			m_LifetimeDrawer = RangeSlider("Lifetime", emitter->GetLifetime(), float2(0.1f, 10.0f), 0.1f, true,
				[this](float2 lifetime) { OnLifetimeModified(lifetime); });
			m_SizeDrawer = RangeSlider("Size", emitter->GetSize(), float2(0.1f, 10.0f), 0.1f, true,
				[this](float2 size) { OnSizeModified(size); });
			m_SpeedDrawer = RangeSlider("Speed", emitter->GetSpeed(), float2(0.1f, 10.0f), 0.1f, true,
				[this](float2 speed) { OnSpeedModified(speed); });
			m_StartColorDrawer = ColorPicker("Start Color", emitter->GetStartColor(),
				[this](float4 color) { OnStartColorModified(color); });
			m_EndColorDrawer = ColorPicker("End Color", emitter->GetEndColor(),
				[this](float4 color) { OnEndColorModified(color); });
			m_ShapeDrawer = EnumDrawer<EmitterShape>("Shape", emitter->GetShape());
		}
	}

	bool ParticleEmitterInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_EmitterEnabled))
		{
			if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
				emitter->SetEnabled(m_EmitterEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Particle Emitter", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			modified |= m_LoopDrawer.Draw();
			modified |= m_DurationDrawer.Draw();
			modified |= m_EmissionRateDrawer.Draw();

			if (m_ShapeDrawer.Draw())
			{
				if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
					emitter->SetShape(m_ShapeDrawer.GetValue());
			}
			if (m_RadiusDrawer.Draw())
			{
				if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
					emitter->SetRadius(m_RadiusDrawer.GetValue());
			}
			if (m_AngleDrawer.Draw())
			{
				if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
					emitter->SetAngle(m_AngleDrawer.GetValue());
			}
			modified |= m_StartColorDrawer.Draw();
			modified |= m_EndColorDrawer.Draw();
			modified |= m_LifetimeDrawer.Draw();
			modified |= m_SizeDrawer.Draw();
			modified |= m_SpeedDrawer.Draw();
			modified |= m_MaterialDrawer.Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void ParticleEmitterInspector::OnLoopModified(bool loop)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetLooping(loop);
	}

	void ParticleEmitterInspector::OnDurationModified(float duration)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetDuration(duration);
	}

	void ParticleEmitterInspector::OnEmissionRateModified(uint32_t emissionRate)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetEmissionRate(emissionRate);
	}

	void ParticleEmitterInspector::OnMaterialModified(GUID material)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetMaterial(material);
	}

	void ParticleEmitterInspector::OnLifetimeModified(float2 lifetime)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetLifetime(lifetime);
	}

	void ParticleEmitterInspector::OnStartColorModified(float4 color)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetStartColor(color);
	}

	void ParticleEmitterInspector::OnEndColorModified(float4 color)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetEndColor(color);
	}

	void ParticleEmitterInspector::OnSizeModified(float2 size)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetSize(size);
	}

	void ParticleEmitterInspector::OnSpeedModified(float2 speed)
	{
		if (ParticleEmitter* emitter = m_GameObject.TryGetComponent<ParticleEmitter>())
			emitter->SetSpeed(speed);
	}

	ShaderInspector::ShaderInspector(GUID guid)
	{
		if (m_Shader = AssetManager::LoadAsset<Shader>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Shader->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Shader->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_SourceShaderDrawer = AssetFieldDrawer("Source Asset", m_Shader->GetSourceAsset(), SourceShader::Type,
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
		}
	}

	bool ShaderInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_SourceShaderDrawer.Draw();

		ImGui::SameLine();

		if (ImGui::Button("Compile"))
			m_Shader->Recompile();

		return modified;
	}

	void ShaderInspector::OnNameChanged(std::string_view name)
	{
		if (m_Shader)
		{
			m_Shader->SetName(name);
			m_Shader->Save();
		}
	}

	void ShaderInspector::OnSourceAssetChanged(GUID sourceGUID)
	{
		if (m_Shader)
		{
			m_Shader->SetSourceAsset(sourceGUID);
			m_Shader->Save();
		}
	}

	SourceModelInspector::SourceModelInspector(GUID guid)
	{
		if (m_Model = AssetManager::LoadSourceAsset<SourceModel>(guid))
		{
			m_Drawers.push_back(StringDrawer("Dst Asset Path", "", false,
				[this](std::string_view path) { OnDstPathChanged(path); }));
		}
	}

	bool SourceModelInspector::Draw()
	{
		bool modified = false;

		for (auto& drawer : m_Drawers)
			modified |= drawer.Draw();

		if (ImGui::Button("Create Mesh Asset"))
		{
			AssetManager::CreateAsset<Mesh>(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}
		else if (ImGui::Button("Create Rig Asset"))
		{
			AssetManager::CreateAsset<AnimationRig>(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}
		else if (ImGui::Button("Create Animation Clip"))
		{
			AssetManager::CreateAsset<AnimationClip>(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}
		else if (ImGui::Button("Test FBX"))
		{
			FBXAssetImporter importer;
			bool res = importer.Import(m_Model->GetPath());
			int debug = 0;
		}

		return modified;
	}

	SourceShaderInspector::SourceShaderInspector(GUID guid)
	{
		if (m_Shader = AssetManager::LoadSourceAsset<SourceShader>(guid))
		{
			m_ShaderNameDrawer = StringDrawer("Shader Name", m_Shader->GetName(), true);
			m_ShaderLanguageDrawer = StringDrawer("Shader Language", m_Shader->GetShaderLanguage(), true);
			m_CompiledDrawer = BoolDrawer("Compiled", m_Shader->IsCompiled(), true);
			m_DstAssetPathDrawer = StringDrawer("Destination Asset Path", m_DstAssetPath, false,
				[this](std::string_view assetPath) { OnDstAssetPathChanged(assetPath); });

			m_ShaderNameDrawer.SetLabelWidth(0.5f);
			m_ShaderLanguageDrawer.SetLabelWidth(0.5f);
			m_CompiledDrawer.SetLabelWidth(0.5f);
			m_DstAssetPathDrawer.SetLabelWidth(0.7f);
		}
	}

	bool SourceShaderInspector::Draw()
	{
		bool modified = false;
		modified |= m_ShaderNameDrawer.Draw();
		modified |= m_ShaderLanguageDrawer.Draw();
		modified |= m_CompiledDrawer.Draw();
		modified |= m_DstAssetPathDrawer.Draw();

		if (ImGui::Button("Compile"))
		{
			m_CompiledDrawer.SetValue(m_Shader->Compile());
		}
		if (ImGui::Button("Create Shader"))
		{
			if (!m_DstAssetPath.empty())
				AssetManager::CreateAsset<Shader>(Project::GetActiveAssetsDirectory() / m_DstAssetPath, m_Shader);
		}

		return modified;
	}

	SourceTextureInspector::SourceTextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadSourceAsset<SourceTexture>(guid))
		{
			m_TextureNameDrawer = StringDrawer("Texture Name", m_Texture->GetName(), true);

			m_AssetPathDrawer = StringDrawer("Asset Path", m_AssetPath, false,
				[this](std::string_view assetPath) { OnAssetPathChanged(assetPath); });
		}
	}

	bool SourceTextureInspector::Draw()
	{
		bool modified = false;

		modified |= m_TextureNameDrawer.Draw();
		modified |= m_AssetPathDrawer.Draw();

		if (ImGui::Button("Create Texture2D"))
		{
			AssetManager::CreateAsset<Texture2D>(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}
		if (ImGui::Button("Create Cubemap"))
		{
			AssetManager::CreateAsset<Cubemap>(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}

		return modified;
	}

	TextureInspector::TextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadAsset<Texture2D>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Texture->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Texture->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Texture->GetType(), true);
			m_SourceAssetDrawer = AssetFieldDrawer("Source Asset", m_Texture->GetSourceAsset(), SourceTexture::Type,
				[this](GUID sourceGUID) { OnSourceAssetchanged(sourceGUID); });
			m_MipMapDrawer = BoolDrawer("Mip Maps Enabled", m_Texture->GetMipMapsEnabled());
			m_MipBiasDrawer = FloatDrawer("Mip Bias", m_Texture->GetMipBias());
			m_MaxMipCountDrawer = IntDrawer<uint32_t>("Max Mip Count", m_Texture->GetMaxMipCount());
			m_PreviewTexture = Renderer::AddImguiTexture(m_Texture);
		}
	}

	static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");
		ImRect bb;
		bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
		bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);

		ImGui::SetCursorScreenPos(float2(bb.Min.x, bb.Max.y) + g.Style.WindowPadding);
		return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}

	bool TextureInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_TypeDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_SourceAssetDrawer.Draw();

		if (m_MipMapDrawer.Draw())
		{
			m_Texture->SetMipMapsEnabled(m_MipMapDrawer.GetValue());
			m_Texture->Save();
		}

		if (m_MipBiasDrawer.Draw())
		{
			m_Texture->SetMipBias(m_MipBiasDrawer.GetValue());
			m_Texture->Save();
		}

		if (m_MaxMipCountDrawer.Draw())
		{
			m_Texture->SetMaxMipCount(m_MaxMipCountDrawer.GetValue());
			m_Texture->Save();
		}

		static float bottomPaneHeight = 400.0f;
		static float topPaneHeight = 800.0f;
		Splitter(false, 4.0f, &bottomPaneHeight, &topPaneHeight, 50.0f, 50.0f);

		ImGui::BeginChild("TexturePreview");
		float2 windowSize = ImGui::GetContentRegionAvail();
		float2 textureSize = float2(m_Texture->GetWidth(), m_Texture->GetHeight());
		float xScale = windowSize.x / textureSize.x;
		float yScale = windowSize.y / textureSize.y;
		float scale = std::min(xScale, yScale);

		textureSize *= scale;
		float2 upperLeft = (windowSize / 2.0f) - (textureSize / 2.0f);
		ImGui::SetCursorPos(upperLeft);
		ImGui::Image((void*)m_PreviewTexture, textureSize);
		ImGui::EndChild();

		return modified;
	}

	void TextureInspector::OnNameChanged(std::string_view name)
	{
		if (m_Texture)
		{
			m_Texture->SetName(name);
			m_Texture->Save();
		}
	}

	void TextureInspector::OnSourceAssetchanged(GUID sourceGUID)
	{
		if (m_Texture)
		{
			m_Texture->SetSourceAsset(sourceGUID);
			m_Texture->Save();
		}
	}

	TransformInspector::TransformInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			positionDrawer = Vector3Drawer("Position", transform->m_Position, glm::vec3(0, 0, 0), true,
				[this](glm::vec3 position) { OnPositionChanged(position); });

			rotationDrawer = Vector3Drawer("Rotation", transform->m_EulerRotation, glm::vec3(0, 0, 0), true,
				[this](glm::vec3 rotation) { OnRotationChanged(rotation); });

			scaleDrawer = Vector3Drawer("Scale", transform->m_Scale, glm::vec3(1, 1, 1), true,
				[this](glm::vec3 scale) { OnScaleChanged(scale); });
		}
	}

	bool TransformInspector::Draw()
	{
		bool modified = false;

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			Transform& transform = m_GameObject.GetComponent<Transform>();
			positionDrawer.SetValue(transform.GetPosition());
			rotationDrawer.SetValue(transform.GetEulerRotation(false));
			scaleDrawer.SetValue(transform.GetScale());

			modified |= positionDrawer.Draw();
			modified |= rotationDrawer.Draw();
			modified |= scaleDrawer.Draw();

			struct Data
			{
				float3 pos, rot, scale;
			};

			if (ImGui::Button("Copy"))
			{
				Data data{ positionDrawer.GetValue(), rotationDrawer.GetValue(), scaleDrawer.GetValue() };
				ClipBoard::Copy("Transform", &data, sizeof(data));
			}

			ImGui::SameLine();

			if (ImGui::Button("Paste"))
			{
				if (ClipBoard::GetContext() == "Transform")
				{
					Data data = ClipBoard::Paste().Read<Data>();

					OnPositionChanged(data.pos);
					OnRotationChanged(data.rot);
					OnScaleChanged(data.scale);
				}
			}
		}

		ImGui::Spacing();

		return modified;
	}

	void TransformInspector::OnPositionChanged(glm::vec3 position)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			transform->SetPosition(position);
	}

	void TransformInspector::OnRotationChanged(glm::vec3 rotation)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			transform->SetRotation(rotation, false);
		}
	}

	void TransformInspector::OnScaleChanged(glm::vec3 scale)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			transform->SetScale(scale);
	}

	ScriptInspector::ScriptInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ScriptComponent* scriptComponent = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			m_ScriptEnabled = scriptComponent->IsEnabled();

			ScriptMetadata& metadata = ScriptingManager::GetScriptMetadata(scriptComponent->GetScriptID());
			displayName = metadata.Name;

			// Remove any namespaces
			size_t found = displayName.find_last_of('.');
			if (found != std::string::npos)
			{
				displayName = displayName.substr(found + 1);
			}

			InitializeDrawers(scriptComponent);
		}
	}

	bool ScriptInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_ScriptEnabled))
		{
			if (ScriptComponent* script = m_GameObject.TryGetComponent<ScriptComponent>())
				script->SetEnabled(m_ScriptEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader(("Script - " + displayName).c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (Ref<PropertyDrawer>& drawer : drawers)
				modified |= drawer->Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void ScriptInspector::UpdateFields()
	{
		drawers.clear();
		if (ScriptComponent* script = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			InitializeDrawers(script);
		}
	}

	void ScriptInspector::InitializeDrawers(ScriptComponent* userScript)
	{
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		for (auto& [fieldID, fieldStorage] : storage.Fields)
		{
			if (fieldStorage.DataType == DataType::Entity || fieldStorage.DataType == DataType::Component)
			{
				Coral::ScopedString typeName = fieldStorage.Type->GetFullName();
				GUID initialValue;
				fieldStorage.TryGetValue(initialValue);

				CreateEntityDrawer(fieldStorage.Name, storage.ScriptID, fieldID, typeName, initialValue);
			}
			// TODO: Convert into IsAssetType check
			else if (fieldStorage.DataType == DataType::Mesh || fieldStorage.DataType == DataType::Material || fieldStorage.DataType == DataType::Texture2D || fieldStorage.DataType == DataType::Prefab)
			{
				GUID initialValue;
				fieldStorage.TryGetValue(initialValue);
				CreateAssetDrawer(fieldStorage.Name, fieldStorage.Type->GetFullName(), storage.ScriptID, fieldID, initialValue);
			}
			else if (fieldStorage.DataType == DataType::String)
				CreateStringDrawer(storage.ScriptID, fieldID, fieldStorage);
			else
				CreateDrawerFromProperty(storage.ScriptID, fieldID, fieldStorage);
		}
	}

	void ScriptInspector::CreateEntityDrawer(std::string_view fieldName, uint32_t scriptID, uint32_t fieldID, const std::string& typeName, GUID initialValue)
	{
		auto callback = [this, scriptID, fieldID](GUID guid)
			{
				Utils::OnFieldChanged<GUID>(m_GameObject.GetGUID(), scriptID, fieldID, guid);
			};

		drawers.emplace_back(new EntityFieldDrawer(fieldName, initialValue, typeName, callback));
	}

	void ScriptInspector::CreateAssetDrawer(const std::string& fieldName, const std::string& assetType, uint32_t scriptID, uint32_t fieldID, GUID initialValue)
	{
		auto callback = [this, scriptID, fieldID](GUID guid)
			{
				Utils::OnFieldChanged<GUID>(m_GameObject.GetGUID(), scriptID, fieldID, guid);
			};

		drawers.emplace_back(new AssetFieldDrawer(fieldName, initialValue, assetType, callback));
	}

	void ScriptInspector::CreateDrawerFromProperty(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		switch (fieldStorage.DataType)
		{
			case DataType::Byte:
			{
				uint8_t initialValue = fieldStorage.GetValue<uint8_t>();
				auto callback = [this, scriptID, fieldID](uint8_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<uint8_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::UShort:
			{
				uint16_t initialValue = fieldStorage.GetValue<uint16_t>();
				auto callback = [this, scriptID, fieldID](uint16_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<uint16_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::UInt:
			{
				uint32_t initialValue = fieldStorage.GetValue<uint32_t>();
				auto callback = [this, scriptID, fieldID](uint32_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<uint32_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::ULong:
			{
				uint64_t initialValue = fieldStorage.GetValue<uint64_t>();
				auto callback = [this, scriptID, fieldID](uint64_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<uint64_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::SByte:
			{
				char8_t initialValue = fieldStorage.GetValue<char8_t>();
				auto callback = [this, scriptID, fieldID](char8_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<char8_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::Short:
			{
				int16_t initialValue = fieldStorage.GetValue<int16_t>();
				auto callback = [this, scriptID, fieldID](int16_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<int16_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::Int:
			{
				int32_t initialValue = fieldStorage.GetValue<int32_t>();
				auto callback = [this, scriptID, fieldID](int32_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<int32_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::Long:
			{
				int64_t initialValue = fieldStorage.GetValue<int64_t>();
				auto callback = [this, scriptID, fieldID](int64_t newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new IntDrawer<int64_t>(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::Float:
			{
				float initialValue = fieldStorage.GetValue<float>();
				auto callback = [this, scriptID, fieldID](float newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new FloatDrawer(fieldStorage.Name, initialValue, callback));
				break;
			}
			case DataType::Double:
			{
				double initialValue = fieldStorage.GetValue<double>();
				auto callback = [this, scriptID, fieldID](double newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new DoubleDrawer(fieldStorage.Name, initialValue, callback));
				break;
			}
			case DataType::Bool:
			{
				Coral::Bool32 initialValue = fieldStorage.GetValue<Coral::Bool32>();
				auto callback = [this, scriptID, fieldID](bool newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new BoolDrawer(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::Vector3:
			{
				glm::vec3 initialValue = fieldStorage.GetValue<glm::vec3>();
				auto callback = [this, scriptID, fieldID](glm::vec3 newValue) { Utils::OnFieldChanged(m_GameObject.GetGUID(), scriptID, fieldID, newValue); };
				drawers.emplace_back(new Vector3Drawer(fieldStorage.Name, initialValue, glm::zero<glm::vec3>(), false, callback));
				break;
			}
		}
	}

	void ScriptInspector::CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		auto callback = [this, scriptID, fieldID](std::string_view newValue)
			{
				OnStringFieldChanged(scriptID, fieldID, newValue);
			};

		drawers.emplace_back(new StringDrawer(fieldStorage.Name, "", false, callback));
	}

	void ScriptInspector::OnStringFieldChanged(uint32_t scriptID, uint32_t fieldID, std::string_view newValue)
	{
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		// Validate we are working on the same script
		if (storage.ScriptID != scriptID)
			return;

		// Look through the field storage for the matching field
		for (auto& [storedFieldID, fieldStorage] : storage.Fields)
		{
			if (fieldID == storedFieldID)
			{
				fieldStorage.ValueBuffer.Allocate(newValue.size());
				fieldStorage.SetValue(newValue);
				break;
			}
		}
	}

	SpriteRendererInspector::SpriteRendererInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}

	bool SpriteRendererInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_Enabled))
		{
			if (SpriteRenderer* spriteRenderer = m_GameObject.TryGetComponent<SpriteRenderer>())
				spriteRenderer->SetEnabled(m_Enabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_SpriteDrawer.Draw())
			{
				if (SpriteRenderer* spriteRenderer = m_GameObject.TryGetComponent<SpriteRenderer>())
					spriteRenderer->SetSprite(m_SpriteDrawer.GetGUID());
			}

			if (m_FillDrawer.Draw())
			{
				if (SpriteRenderer* spriteRenderer = m_GameObject.TryGetComponent<SpriteRenderer>())
					spriteRenderer->SetFill(m_FillDrawer.GetValue());
			}

			if (m_BaseColorPicker.Draw())
			{
				if (SpriteRenderer* spriteRenderer = m_GameObject.TryGetComponent<SpriteRenderer>())
					spriteRenderer->SetBaseColor(m_BaseColorPicker.GetColor4());
			}

			if (m_AnchorDrawer.Draw())
			{
				if (SpriteRenderer* spriteRenderer = m_GameObject.TryGetComponent<SpriteRenderer>())
					spriteRenderer->SetAnchor(m_AnchorDrawer.GetValue());
			}
		}

		ImGui::PopID();

		return modified;
	}

	void SpriteRendererInspector::InitDrawers()
	{
		if (SpriteRenderer* spriteRenderer = m_GameObject.TryGetComponent<SpriteRenderer>())
		{
			m_Enabled = spriteRenderer->IsEnabled();

			GUID sprite = spriteRenderer->GetSprite() ? spriteRenderer->GetSprite()->GetGUID() : GUID::Empty();
			m_SpriteDrawer = AssetFieldDrawer("Sprite", sprite, Texture2D::Type);
			m_FillDrawer = RangeSlider("Fill", spriteRenderer->GetFill(), float2(0.0f, 1.0f), 0.1f, false);
			m_BaseColorPicker = ColorPicker("Base Color", spriteRenderer->GetBaseColor());
			m_AnchorDrawer = EnumDrawer<SpriteRenderer::AnchorPosition>("Anchor Position", spriteRenderer->GetAnchor());
		}
	}

	CharacterControllerInspector::CharacterControllerInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}

	bool CharacterControllerInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_Enabled))
		{
			if (CharacterController* controller = m_GameObject.TryGetComponent<CharacterController>())
				controller->SetEnabled(m_Enabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Character Controller", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_CenterDrawer.Draw())
			{
				if (CharacterController* controller = m_GameObject.TryGetComponent<CharacterController>())
					controller->SetCenter(m_CenterDrawer.GetValue());
			}
			if (m_RadiusDrawer.Draw())
			{
				if (CharacterController* controller = m_GameObject.TryGetComponent<CharacterController>())
					controller->SetRadius(m_RadiusDrawer.GetValue());
			}
			if (m_HeightDrawer.Draw())
			{
				if (CharacterController* controller = m_GameObject.TryGetComponent<CharacterController>())
					controller->SetHeight(m_HeightDrawer.GetValue());
			}
			if (m_DebugDrawer.Draw())
			{
				if (CharacterController* controller = m_GameObject.TryGetComponent<CharacterController>())
					controller->SetDebugEnabled(m_DebugDrawer.GetValue());
			}
		}

		ImGui::PopID();

		return modified;
	}

	void CharacterControllerInspector::InitDrawers()
	{
		if (CharacterController* controller = m_GameObject.TryGetComponent<CharacterController>())
		{
			m_Enabled = controller->IsEnabled();
			m_CenterDrawer = Vector3Drawer("Center", controller->GetCenter(), float3(0.0f), false);
			m_RadiusDrawer = FloatDrawer("Radius", controller->GetRadius());
			m_HeightDrawer = FloatDrawer("Height", controller->GetHeight());
			m_DebugDrawer = BoolDrawer("Enable Debug", controller->IsDebugEnabled(), false);
		}
	}

	BoxColliderInspector::BoxColliderInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}

	bool BoxColliderInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_Enabled))
		{
			if (BoxCollider* boxCollider = m_GameObject.TryGetComponent<BoxCollider>())
				boxCollider->SetEnabled(m_Enabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_CenterDrawer.Draw())
			{
				if (BoxCollider* boxCollider = m_GameObject.TryGetComponent<BoxCollider>())
					boxCollider->SetCenter(m_CenterDrawer.GetValue());
			}
			if (m_ExtentsDrawer.Draw())
			{
				if (BoxCollider* boxCollider = m_GameObject.TryGetComponent<BoxCollider>())
					boxCollider->SetExtents(m_ExtentsDrawer.GetValue());
			}
			if (m_DebugDrawer.Draw())
			{
				if (BoxCollider* boxCollider = m_GameObject.TryGetComponent<BoxCollider>())
					boxCollider->SetDebugEnabled(m_DebugDrawer.GetValue());
			}
		}

		ImGui::PopID();

		return modified;
	}

	void BoxColliderInspector::InitDrawers()
	{
		if (BoxCollider* boxCollider = m_GameObject.TryGetComponent<BoxCollider>())
		{
			m_Enabled = boxCollider->IsEnabled();
			m_CenterDrawer = Vector3Drawer("Center", boxCollider->GetCenter(), float3(0.0f), false);
			m_ExtentsDrawer = Vector3Drawer("Extents", boxCollider->GetExtents(), float3(1.0f), false);
			m_DebugDrawer = BoolDrawer("Enable Debug", boxCollider->IsDebugEnabled(), false);
		}
	}

	CapsuleColliderInspector::CapsuleColliderInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}
	bool CapsuleColliderInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_Enabled))
		{
			if (CapsuleCollider* collider = m_GameObject.TryGetComponent<CapsuleCollider>())
				collider->SetEnabled(m_Enabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Capsule Collider", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_CenterDrawer.Draw())
			{
				if (CapsuleCollider* collider = m_GameObject.TryGetComponent<CapsuleCollider>())
					collider->SetCenter(m_CenterDrawer.GetValue());
			}
			if (m_RadiusDrawer.Draw())
			{
				if (CapsuleCollider* collider = m_GameObject.TryGetComponent<CapsuleCollider>())
					collider->SetRadius(m_RadiusDrawer.GetValue());
			}
			if (m_HeightDrawer.Draw())
			{
				if (CapsuleCollider* collider = m_GameObject.TryGetComponent<CapsuleCollider>())
					collider->SetHeight(m_HeightDrawer.GetValue());
			}
			if (m_DebugDrawer.Draw())
			{
				if (CapsuleCollider* collider = m_GameObject.TryGetComponent<CapsuleCollider>())
					collider->SetDebugEnabled(m_DebugDrawer.GetValue());
			}
		}

		ImGui::PopID();

		return modified;
	}
	void CapsuleColliderInspector::InitDrawers()
	{
		if (CapsuleCollider* collider = m_GameObject.TryGetComponent<CapsuleCollider>())
		{
			m_Enabled = collider->IsEnabled();
			m_CenterDrawer = Vector3Drawer("Center", collider->GetCenter(), float3(0.0f), false);
			m_RadiusDrawer = FloatDrawer("Radius", collider->GetRadius());
			m_HeightDrawer = FloatDrawer("Height", collider->GetHeight());
			m_DebugDrawer = BoolDrawer("Enable Debug", collider->IsDebugEnabled(), false);
		}
	}

	SphereColliderInspector::SphereColliderInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}

	bool SphereColliderInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_Enabled))
		{
			if (SphereCollider* collider = m_GameObject.TryGetComponent<SphereCollider>())
				collider->SetEnabled(m_Enabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_CenterDrawer.Draw())
			{
				if (SphereCollider* sphereCollider = m_GameObject.TryGetComponent<SphereCollider>())
					sphereCollider->SetCenter(m_CenterDrawer.GetValue());
			}
			if (m_RadiusDrawer.Draw())
			{
				if (SphereCollider* sphereCollider = m_GameObject.TryGetComponent<SphereCollider>())
					sphereCollider->SetRadius(m_RadiusDrawer.GetValue());
			}
			if (m_DebugDrawer.Draw())
			{
				if (SphereCollider* sphereCollider = m_GameObject.TryGetComponent<SphereCollider>())
					sphereCollider->SetDebugEnabled(m_DebugDrawer.GetValue());
			}
		}

		ImGui::PopID();

		return modified;
	}

	void SphereColliderInspector::InitDrawers()
	{
		if (SphereCollider* sphereCollider = m_GameObject.TryGetComponent<SphereCollider>())
		{
			m_Enabled = sphereCollider->IsEnabled();
			m_CenterDrawer = Vector3Drawer("Center", sphereCollider->GetCenter(), float3(0.0f), false);
			m_RadiusDrawer = FloatDrawer("Radius", sphereCollider->GetRadius());
			m_DebugDrawer = BoolDrawer("Enable Debug", sphereCollider->IsDebugEnabled(), false);
		}
	}

	RigidBodyInspector::RigidBodyInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
	}

	bool RigidBodyInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_Enabled))
		{
			if (RigidBody* rigidBody = m_GameObject.TryGetComponent<RigidBody>())
				rigidBody->SetEnabled(m_Enabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_LayerDrawer.Draw())
			{
				if (RigidBody* rigidBody = m_GameObject.TryGetComponent<RigidBody>())
					rigidBody->SetLayer(m_LayerDrawer.GetValue());
			}
			if (m_FrictionDrawer.Draw())
			{
				if (RigidBody* rigidBody = m_GameObject.TryGetComponent<RigidBody>())
					rigidBody->SetFriction(m_FrictionDrawer.GetValue());
			}
			if (m_MaxLinearVelocityDrawer.Draw())
			{
				if (RigidBody* rigidBody = m_GameObject.TryGetComponent<RigidBody>())
					rigidBody->SetMaxLinearVelocity(m_MaxLinearVelocityDrawer.GetValue());
			}
		}

		ImGui::PopID();

		return modified;
	}

	void RigidBodyInspector::InitDrawers()
	{
		if (RigidBody* rigidBody = m_GameObject.TryGetComponent<RigidBody>())
		{
			m_Enabled = rigidBody->IsEnabled();
			m_LayerDrawer = EnumDrawer<PhysicsLayer>("Physics Layer", rigidBody->GetLayer());
			m_FrictionDrawer = FloatDrawer("Friction", rigidBody->GetFriction());
			m_MaxLinearVelocityDrawer = FloatDrawer("Max Linear Velocity", rigidBody->GetMaxLinearVelocity());
		}
	}
}