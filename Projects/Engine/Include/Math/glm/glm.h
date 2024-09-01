#pragma once
#ifndef GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_LEFT_HANDED
#endif

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif // !GLM_ENABLE_EXPERIMENTAL
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#include "glm.hpp"
#include "ext.hpp"
#include <glm/gtx/matrix_decompose.hpp>

using namespace glm;

namespace Odyssey
{
    inline vec3 SafeNormalize(const vec3& in)
    {
        float invMag = 1.0f / glm::length(in);

        if (invMag * 0.0f == invMag * 0.0f)
        {
            return vec3(in.x * invMag, in.y * invMag, in.z * invMag);
        }

        return in;
    }
}