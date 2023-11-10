#include "Matrix4x4.h"
#include <gtx/matrix_decompose.hpp>

namespace Odyssey
{
    Matrix4x4 Matrix4x4::Identity()
    {
        return glm::identity<glm::mat4>();
    }

    Matrix4x4 Matrix4x4::Translate(const Vector3& amt)
    {
        return glm::translate(amt);
    }

    Matrix4x4 Matrix4x4::Scale(const Vector3& amt)
    {
        return glm::scale(amt);
    }

    Matrix4x4 Matrix4x4::Scale(float amt)
    {
        return glm::scale(glm::vec3(amt));
    }

    Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float n, float f)
    {
        return glm::orthoLH(left, right, bottom, top, n, f);
    }

    Matrix4x4 Matrix4x4::Perspective(float halfFov, float width, float height, float nearZ, float farZ)
    {
        return glm::perspectiveFovLH_ZO(glm::radians(halfFov), 1920.0f, 1080.0f, nearZ, farZ);
    }

    Matrix4x4 Matrix4x4::PerspectiveRH(float halfFov, float width, float height, float nearZ, float farZ)
    {
        return glm::perspectiveFovRH_ZO(glm::radians(halfFov), width, height, nearZ, farZ);
    }

    Matrix4x4 Matrix4x4::TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
    {
        Matrix4x4 trans = glm::translate(translation);
        Matrix4x4 rot = InitRotation(rotation);
        Matrix4x4 sc = glm::scale(scale);
        return trans * rot * sc;
    }

    Matrix4x4 Matrix4x4::InitRotationFromVectors(const Vector3& u, const Vector3& v, const Vector3& n)
    {
        return glm::mat4(glm::vec4(u.x, u.y, u.z, 0.0f), glm::vec4(v.x, v.y, v.z, 0.0f), glm::vec4(n.x, n.y, n.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    Matrix4x4 Matrix4x4::InitRotationFromDirection(const Vector3& forward, const Vector3& up)
    {
        Vector3 n = forward.Normalized();
        Vector3 u = up.Cross(n).Normalized();
        Vector3 v = n.Cross(u);
        return InitRotationFromVectors(u, v, n);
    }

    Matrix4x4 Matrix4x4::InitRotation(const Quaternion& quaternion)
    {
        float yy2 = 2.0f * quaternion.y * quaternion.y;
        float xy2 = 2.0f * quaternion.x * quaternion.y;
        float xz2 = 2.0f * quaternion.x * quaternion.z;
        float yz2 = 2.0f * quaternion.y * quaternion.z;
        float zz2 = 2.0f * quaternion.z * quaternion.z;
        float wz2 = 2.0f * quaternion.w * quaternion.z;
        float wy2 = 2.0f * quaternion.w * quaternion.y;
        float wx2 = 2.0f * quaternion.w * quaternion.x;
        float xx2 = 2.0f * quaternion.x * quaternion.x;

        glm::vec4 a = glm::vec4(-yy2 - zz2 + 1.0f, xy2 + wz2, xz2 - wy2, 0.0f);
        glm::vec4 b = glm::vec4(xy2 - wz2, -xx2 - zz2 + 1.0f, yz2 + wx2, 0.0f);
        glm::vec4 c = glm::vec4(xz2 + wy2, yz2 - wx2, -xx2 - yy2 + 1.0f, 0.0f);
        glm::vec4 d = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return Matrix4x4(a, b, c, d);
    }

    Matrix4x4 Matrix4x4::InitLookAt(const Vector3& location, const Vector3& forward, const Vector3& up)
    {
        return glm::lookAtLH(location, forward, up);
    }

    Matrix4x4 Matrix4x4::InitLookAtRH(const Vector3& location, const Vector3& forward, const Vector3& up)
    {
        return glm::lookAt(location, forward, up);
    }

    float Matrix4x4::Determinant4x4() const
    {
        return glm::determinant(*this);
    }

    Matrix4x4 Matrix4x4::ToNormalMatrix() const
    {
        // TODO: There *should* be a faster and easier way to do this!
        return Inverse().Transpose();
    }

    Matrix4x4 Matrix4x4::Transpose() const
    {
        return glm::transpose(*this);
    }

    Matrix4x4 Matrix4x4::Inverse() const
    {
        return glm::inverse(*this);
    }

    Matrix4x4 Matrix4x4::ApplyScale(const Vector3& scale)
    {
        *this = glm::scale(*this, scale);
        return *this;
    }

    void Matrix4x4::Decompose(Vector3& position, Quaternion& rotation, Vector3& scale)
    {
        glm::vec4 perspective;
        glm::vec3 skew;
        glm::decompose(*this, scale, rotation, position, skew, perspective);
    }

    bool Matrix4x4::Decompose(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale)
    {
        using T = float;

        mat4 LocalMatrix(transform);

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        outTranslation = vec3(LocalMatrix[3]);
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3], Pdum3;

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        outScale.x = glm::length(Row[0]);
        Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
        outScale.y = glm::length(Row[1]);
        Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
        outScale.z = glm::length(Row[2]);
        Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
#if 1
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                outScale[i] *= static_cast<T>(-1);
                Row[i] *= static_cast<T>(-1);
            }
        }
#endif

        outRotation.y = asin(-Row[0][2]);
        if (cos(outRotation.y) != 0)
        {
            outRotation.x = atan2(Row[1][2], Row[2][2]);
            outRotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else
        {
            outRotation.x = atan2(-Row[2][0], Row[1][1]);
            outRotation.z = 0;
        }

        return true;
    }

    std::string Matrix4x4::ToString()
    {
        std::string str = "0: " + std::to_string((*this)[0][0]) + " | " + std::to_string((*this)[0][1]) + " | " + std::to_string((*this)[0][2]) + " | " + std::to_string((*this)[0][3]) + "\n" + std::to_string((*this)[1][0]) + " | " + std::to_string((*this)[1][1]) + " | " +
            std::to_string((*this)[1][2]) + " | " + std::to_string((*this)[1][3]) + "\n" + std::to_string((*this)[2][0]) + " | " + std::to_string((*this)[2][1]) + " | " + std::to_string((*this)[2][2]) + " | " + std::to_string((*this)[2][3]) + "\n" + std::to_string((*this)[3][0]) +
            " | " + std::to_string((*this)[3][1]) + " | " + std::to_string((*this)[3][2]) + " | " + std::to_string((*this)[3][3]) + "\n";

        return str;
    }

}