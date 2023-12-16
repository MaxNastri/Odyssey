#include "RayTracingWindow.h"
#include "Application.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"
#include "imgui.h"
#include "Camera.h"
#include "Transform.h"
#include "Input.h"
#include "Random.h"
#include "OdysseyTime.h"
#include <execution>

namespace Odyssey
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	RayTracingWindow::RayTracingWindow()
		: DockableWindow("Ray Tracing", glm::vec2(0,0), glm::vec2(500,500), glm::vec2(2,2))
	{
		m_GameObject.id = -2;
		m_CameraTransform = m_GameObject.AddComponent<Transform>();
		m_CameraTransform->SetGameObject(&m_GameObject);
		m_CameraTransform->Awake();
		m_Camera = m_GameObject.AddComponent<Camera>();
		m_Camera->SetGameObject(&m_GameObject);
		m_Camera->Awake();

		CreateObjects();
	}

	void RayTracingWindow::Update()
	{
		m_CameraControllerInUse = false;
		if (UpdateCameraController())
		{
			CalculateRayDirections();
			m_FrameIndex = 1;
		}

		// Set the current RT as the scene view pass target
		for (int i = 0; i < m_ImageData.size(); i++)
		{
			m_ImageData[i] = ConvertToRGBA(glm::vec4(1, 0, 0, 1));
		}
	}

	void RayTracingWindow::Draw()
	{
		if (!Begin())
			return;

		uint32_t width = (uint32_t)m_WindowSize.x;
		uint32_t height = (uint32_t)m_WindowSize.y;

		if (m_FrameIndex == 1)
			memset(m_AccumulationData.data(), 0, width * height * sizeof(glm::vec4));

		std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
			[this, width](uint32_t y)
			{
				std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y, width](uint32_t x)
					{
						glm::vec4 color = PerPixel(x, y);
						m_AccumulationData[x + y * width] += color;

						glm::vec4 accumulatedColor = m_AccumulationData[x + y * width];
						accumulatedColor /= (float)m_FrameIndex;

						accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
						m_ImageData[x + y * width] = ConvertToRGBA(accumulatedColor);
					});
			});

		m_RenderTexture.Get()->SetData(m_ImageData.data());
		std::string dt = std::to_string(Time::DeltaTime());
		ImGui::Text(dt.c_str());
		ImGui::Image(reinterpret_cast<void*>(m_RenderTextureID), ImVec2(m_WindowSize.x, m_WindowSize.y));
		m_FrameIndex++;
		End();
	}

	void RayTracingWindow::OnWindowResize()
	{
		m_FrameIndex = 1;

		uint32_t width = (uint32_t)m_WindowSize.x;
		uint32_t height = (uint32_t)m_WindowSize.y;
		size_t pixels = (size_t)(m_WindowSize.x * m_WindowSize.y);

		// Resize the data
		m_ImageData.clear();
		m_ImageData.resize(pixels);

		m_AccumulationData.clear();
		m_AccumulationData.resize(pixels);

		m_ImageHorizontalIter.resize(width);
		for (uint32_t i = 0; i < width; i++)
			m_ImageHorizontalIter[i] = i;

		m_ImageVerticalIter.resize(height);
		for (uint32_t i = 0; i < height; i++)
			m_ImageVerticalIter[i] = i;

		// Remake the render texture
		DestroyRenderTexture();
		CreateRenderTexture();

		// Resize the camer viewport
		m_Camera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);

		// Recalculate ray directions for the camera
		m_RayDirections.resize((size_t)(m_WindowSize.x * m_WindowSize.y));
		CalculateRayDirections();
	}

	glm::vec4 RayTracingWindow::PerPixel(uint32_t x, uint32_t y)
	{
		Ray ray;
		ray.Origin = m_CameraTransform->GetPosition();
		ray.Direction = m_RayDirections[x + y * (uint32_t)m_WindowSize.x];

		glm::vec3 light(0.0f);
		glm::vec3 contribution(1.0f);

		int bounces = 8;

		for (int32_t i = 0; i < bounces; i++)
		{
			HitPayload payload = TraceRay(ray);

			if (payload.HitDistance < 0.0f)
			{
				glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
				light += skyColor * contribution;
				break;
			}

			const Sphere& sphere = m_Spheres[payload.ObjectIndex];
			const RayTracedMaterial& material = m_Materials[sphere.MaterialIndex];

			contribution *= material.Albedo * (1.4f - material.Roughness);
			light += material.GetEmission() * material.Albedo;

			ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.000001f;
			ray.Direction = glm::normalize(payload.WorldNormal + (material.Roughness * Random::InUnitSphere()));
		}

		return glm::vec4(light, 1.0f);
	}

	RayTracingWindow::HitPayload RayTracingWindow::TraceRay(const Ray& ray)
	{
		// Quadratic formula for calculating collisions
		// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
		// where
		// a = ray origin
		// b = ray direction
		// r = radius
		// t = hit distance

		int closestSphere = -1;
		float hitDistance = std::numeric_limits<float>::max();

		for (size_t i = 0; i < m_Spheres.size(); i++)
		{
			const Sphere& sphere = m_Spheres[i];

			glm::vec3 origin = ray.Origin - sphere.Position;

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			// Quadratic forumula discriminant:
			// b^2 - 4ac

			float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f)
				continue;

			// Quadratic formula:
			// (-b +- sqrt(discriminant)) / 2a

			// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
			float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (closestT > 0.0f && closestT < hitDistance)
			{
				hitDistance = closestT;
				closestSphere = (int)i;
			}
		}

		if (closestSphere < 0)
			return Miss(ray);

		return ClosestHit(ray, hitDistance, closestSphere);
	}

	RayTracingWindow::HitPayload RayTracingWindow::ClosestHit(const Ray& ray, float hitDistance, int32_t objectIndex)
	{
		HitPayload payload;
		payload.HitDistance = hitDistance;
		payload.ObjectIndex = objectIndex;

		const Sphere& closestSphere = m_Spheres[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphere.Position;
		payload.WorldPosition = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(payload.WorldPosition);
		payload.WorldPosition += closestSphere.Position;

		return payload;
	}

	RayTracingWindow::HitPayload RayTracingWindow::Miss(const Ray& ray)
	{
		HitPayload payload;
		payload.HitDistance = -1.0f;
		return payload;
	}

	void RayTracingWindow::CreateObjects()
	{
		RayTracedMaterial& pinkSphere = m_Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 1.0f, 1.0f };
		pinkSphere.Roughness = 0.75f;

		RayTracedMaterial& blueSphere = m_Materials.emplace_back();
		blueSphere.Albedo = { 0.3f, 0.3f, 0.7f };
		blueSphere.Roughness = 0.75f;

		RayTracedMaterial& orangeSphere = m_Materials.emplace_back();
		orangeSphere.Albedo = { 0.8f, 0.5f, 0.2f };
		orangeSphere.Roughness = 1.0f;
		orangeSphere.EmissionColor = orangeSphere.Albedo;
		orangeSphere.EmissionPower = 2.0f;

		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 2.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 5.0f, 15.0f, 25.0f };
			sphere.Radius = 20.0f;
			sphere.MaterialIndex = 2;
			m_Spheres.push_back(sphere);
		}
	}

	void RayTracingWindow::CreateRenderTexture()
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass
		m_RenderTexture = ResourceManager::AllocateTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);

		// Create an IMGui texture handle
		if (auto renderer = Application::GetRenderer())
			if (auto imgui = renderer->GetImGui())
				m_RenderTextureID = imgui->AddTexture(m_RenderTexture);
	}

	void RayTracingWindow::DestroyRenderTexture()
	{
		// Destroy the existing render texture
		if (m_RenderTexture.IsValid())
		{
			// Remove the imgui texture
			if (auto renderer = Application::GetRenderer())
				if (auto imgui = renderer->GetImGui())
					//imgui->RemoveTexture(m_RenderTextureID[index]);

			// Destroy the render texture
					ResourceManager::DestroyTexture(m_RenderTexture);
		}
	}

	bool RayTracingWindow::UpdateCameraController()
	{
		const float speed = 2.0f;
		bool moved = false;

		glm::vec3 inputVel = glm::zero<vec3>();

		if (m_CursorInContentRegion && Input::GetMouseButtonDown(MouseButton::Right))
		{
			m_CameraControllerInUse = true;

			if (Input::GetKeyDown(KeyCode::W))
			{
				inputVel += glm::vec3(0, 0, 1);
			}
			if (Input::GetKeyDown(KeyCode::S))
			{
				inputVel += glm::vec3(0, 0, -1);
			}
			if (Input::GetKeyDown(KeyCode::D))
			{
				inputVel += glm::vec3(1, 0, 0);
			}
			if (Input::GetKeyDown(KeyCode::A))
			{
				inputVel += glm::vec3(-1, 0, 0);
			}
			if (Input::GetKeyDown(KeyCode::E))
			{
				inputVel += glm::vec3(0, 1, 0);
			}
			if (Input::GetKeyDown(KeyCode::Q))
			{
				inputVel += glm::vec3(0, -1, 0);
			}

			if (inputVel != glm::zero<vec3>())
			{
				inputVel = glm::normalize(inputVel);
				glm::vec3 right = m_CameraTransform->Right() * inputVel.x;
				glm::vec3 up = m_CameraTransform->Up() * inputVel.y;
				glm::vec3 fwd = m_CameraTransform->Forward() * inputVel.z;
				glm::vec3 velocity = (right + up + fwd) * speed * Time::DeltaTime();
				m_CameraTransform->AddPosition(velocity);
				moved = true;
			}

			float mouseH = (float)Input::GetMouseAxisHorizontal();
			float mouseV = (float)Input::GetMouseAxisVerticle();

			if (mouseH != 0.0f || mouseV != 0.0f)
			{
				glm::vec3 yaw = vec3(0, 1, 0) * mouseH * Time::DeltaTime() * 2.0f;
				glm::vec3 pitch = vec3(1, 0, 0) * mouseV * Time::DeltaTime() * 2.0f;

				m_CameraTransform->AddRotation(yaw);
				m_CameraTransform->AddRotation(pitch);
				moved = true;
			}
		}

		return moved;
	}

	void RayTracingWindow::CalculateRayDirections()
	{
		glm::mat4 inverseProjection = m_Camera->GetInverseProjection();
		glm::mat4 inverseView = m_Camera->GetView();

		uint32_t width = (uint32_t)m_WindowSize.x;

		std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
			[this, width, inverseProjection, inverseView](uint32_t y)
			{
				std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y, width, inverseProjection, inverseView](uint32_t x)
					{
						glm::vec2 coord = { (float)x / (float)m_WindowSize.x, (float)y / (float)m_WindowSize.y };
						coord = coord * 2.0f - 1.0f; // -1 -> 1

						glm::vec4 target = inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
						glm::vec3 rayDirection = glm::vec3(inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
						m_RayDirections[x + y * (uint32_t)m_WindowSize.x] = rayDirection;
					});
			});
	}
}