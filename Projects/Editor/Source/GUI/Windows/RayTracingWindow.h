#pragma once
#include "DockableWindow.h"
#include "Resource.h"
#include "GameObject.h"
#include "glm.h"

namespace Odyssey
{
	class Camera;
	class Transform;
	class VulkanRenderTexture;
	class VulkanTextureSampler;

	class RayTracingWindow : DockableWindow
	{
	public:
		RayTracingWindow();

	public:
		virtual void Update() override;
		virtual bool Draw() override;
		virtual void OnWindowResize() override;

	private:
		struct Ray
		{
			glm::vec3 Origin;
			glm::vec3 Direction;
		};

		struct HitPayload
		{
			float HitDistance;
			glm::vec3 WorldPosition;
			glm::vec3 WorldNormal;

			int ObjectIndex;
		};

		struct RayTracedMaterial
		{
			glm::vec3 Albedo{ 1.0f };
			float Roughness = 1.0f;
			float Metallic = 0.0f;
			glm::vec3 EmissionColor{ 0.0f };
			float EmissionPower = 0.0f;

			glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
		};

		struct Sphere
		{
			glm::vec3 Position{ 0.0f };
			float Radius = 0.5f;

			int MaterialIndex = 0;
		};

		glm::vec4 PerPixel(uint32_t x, uint32_t y);
		HitPayload TraceRay(const Ray& ray);
		HitPayload ClosestHit(const Ray& ray, float hitDistance, int32_t objectIndex);
		HitPayload Miss(const Ray& ray);

	private:
		void CreateObjects();
		void CreateRenderTexture();
		void DestroyRenderTexture();
		bool UpdateCameraController();
		void CalculateRayDirections();

	private: // Camera stuff
		GameObject m_GameObject;
		Transform* m_CameraTransform;
		Camera* m_Camera;
		bool m_CameraControllerInUse = false;

	private:
		uint64_t m_RenderTextureID;
		ResourceID m_RenderTexture;
		ResourceID m_RTSampler;
		std::vector<uint32_t> m_ImageData;
		std::vector<glm::vec4> m_AccumulationData;
		std::vector<glm::vec3> m_RayDirections;
		std::vector<Sphere> m_Spheres;
		std::vector<RayTracedMaterial> m_Materials;
		std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
		uint32_t m_FrameIndex = 0;
	};
}