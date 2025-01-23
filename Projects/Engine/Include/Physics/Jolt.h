#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Odyssey
{
	inline JPH::Vec3 ToJoltVec3(float3 flt)
	{
		return JPH::Vec3(flt.x, flt.y, flt.z);
	}

	inline JPH::Vec4 ToJoltVec4(float4 flt)
	{
		return JPH::Vec4(flt.x, flt.y, flt.z, flt.w);
	}

	inline JPH::Quat ToJoltQuat(quat q)
	{
		return JPH::Quat(q.x, q.y, q.z, q.w);
	}

	inline float3 ToFloat3(JPH::Vec3 vec)
	{
		return float3(vec.GetX(), vec.GetY(), vec.GetZ());
	}

	inline float4 ToFloat4(JPH::Vec4 vec)
	{
		return float4(vec.GetX(), vec.GetY(), vec.GetZ(), vec.GetW());
	}

	inline quat ToQuat(JPH::Quat q)
	{
		return quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
	}
}