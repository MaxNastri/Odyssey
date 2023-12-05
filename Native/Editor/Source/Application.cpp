#include "Application.h"
#include <Input.h>
#include <Logger.h>
#include <FileManager.h>
#include <Paths.h>
#include <VulkanRenderer.h>
#include "ScriptCompiler.h"
#include <ScriptingManager.h>
#include "SceneManager.h"
#include "GUIManager.h"
#include <ComponentManager.h>
#include <Camera.h>
#include <MeshRenderer.h>
#include "Mesh.h"
#include "AssetManager.h"

namespace Odyssey
{
	Application::Application()
	{
		// Initialize our managers
		ScriptingManager::Initialize();
		FileManager::Initialize();
		FileManager::TrackFolder(Paths::Relative::ManagedProjectSource);
		AssetManager::CreateDatabase();

		// Create the renderer
		renderer = std::make_shared<VulkanRenderer>();

		// Start listening for events
		ScriptCompiler::ListenForEvents();
		GUIManager::Initialize();
		SceneManager::ListenForEvents();

		// Build the user assembly
		ScriptCompiler::BuildUserAssembly();

		// Create the scene
		SceneManager::LoadScene("Assets/Scenes/scene.yaml");

		ConstructVisuals();
		SetupEditorGUI();
		CreateRenderPasses();

		// We're off an running
		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		SceneManager::Awake();

		while (running)
		{
			float elapsed = stopwatch.Elapsed();

			if (elapsed > MaxFPS)
			{
				stopwatch.Restart();
				ScriptCompiler::Process();

				GUIManager::Update();
				SceneManager::Update();

				if (!renderer->Update())
				{
					running = false;
				}

				renderer->Render();
			}
		}
		renderer->Destroy();
	}

	void Application::Exit()
	{
		running = false;
	}

	void Application::ConstructVisuals()
	{
		Scene* scene = SceneManager::GetActiveScene();
		GameObject* go = scene->GetGameObject(0);

		ResourceHandle<Material> material;
		{
			ResourceHandle<VulkanShaderModule> vertexShader = ResourceManager::AllocateShaderModule(ShaderType::Vertex, "Assets/Shaders/vert.spv");
			ResourceHandle<VulkanShaderModule> fragmentShader = ResourceManager::AllocateShaderModule(ShaderType::Fragment, "Assets/Shaders/frag.spv");
			//material = AssetManager::LoadMaterial("");
		}
		
		AssetHandle<Mesh> mesh = AssetManager::LoadMesh("Assets/Meshes/Quad.mesh");

		if (MeshRenderer* renderer = ComponentManager::GetComponent<MeshRenderer>(go->id))
		{
			renderer->SetMaterial(material);
			renderer->SetMesh(mesh);
		}
		else
		{
			MeshRenderer* mr = ComponentManager::AddComponent<MeshRenderer>(go, mesh, material);
		}
	}

	void Application::SetupEditorGUI()
	{
		GUIManager::CreateInspectorWindow(RefHandle<GameObject>::Empty());
		GUIManager::CreateSceneHierarchyWindow();
		GUIManager::CreateSceneViewWindow();
	}

	void Application::CreateRenderPasses()
	{
		renderer->AddRenderPass(GUIManager::GetSceneViewWindow(0).GetRenderPass());
		renderer->AddRenderPass(GUIManager::GetRenderPass());
	}
}