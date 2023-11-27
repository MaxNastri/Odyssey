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

namespace Odyssey
{
	Application::Application()
	{
		ScriptingManager::Initialize();
		FileManager::Initialize();
		FileManager::TrackFolder(Paths::Relative::ManagedProjectSource);
		ScriptCompiler::ListenForEvents();
		GUIManager::ListenForEvents();
		SceneManager::ListenForEvents();

		ScriptCompiler::BuildUserAssembly();
		r = new VulkanRenderer();

		running = true;
	}

	void Application::Run()
	{
		running = true;
		stopwatch.Start();

		// Create the scene
		SceneManager::LoadScene("scene.yaml");

		Scene* scene = SceneManager::GetActiveScene();
		RefHandle<GameObject> go = scene->GetGameObject(0);

		ConstructVisuals();

		GUIManager::CreateInspectorWindow(go);
		GUIManager::CreateSceneHierarchyWindow(SceneManager::GetActiveSceneRef());

		scene->Awake();
		while (running)
		{
			float elapsed = stopwatch.Elapsed();

			if (elapsed > MaxFPS)
			{
				stopwatch.Restart();
				ScriptCompiler::Process();

				SceneManager::Update();

				if (!r->Update())
				{
					running = false;
				}

				r->Render();
			}
		}
		r->Destroy();
		delete r;
		r = nullptr;
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
			ResourceHandle<VulkanShader> vertexShader = ResourceManager::AllocateShader(ShaderType::Vertex, "vert.spv");
			ResourceHandle<VulkanShader> fragmentShader = ResourceManager::AllocateShader(ShaderType::Fragment, "frag.spv");
			material = ResourceManager::AllocateMaterial(vertexShader, fragmentShader);
		}
		ResourceHandle<Mesh> mesh;
		{
			std::vector<VulkanVertex> vertices;
			vertices.resize(4);
			vertices[0] = VulkanVertex(glm::vec3(-0.5f, -0.5f, 0), glm::vec3(1, 0, 0));
			vertices[1] = VulkanVertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0, 1, 0));
			vertices[2] = VulkanVertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0, 0, 1));
			vertices[3] = VulkanVertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1));

			std::vector<uint32_t> indices{ 0, 3,2,2,1,0 };

			mesh = ResourceManager::AllocateMesh(vertices, indices);
		}

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
}