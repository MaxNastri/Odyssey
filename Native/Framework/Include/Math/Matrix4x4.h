#pragma once
#include "glm.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <sstream>

namespace Odyssey
{
    using namespace glm;
    class Matrix4x4 : public glm::mat4x4
    {
    public:
        Matrix4x4() {};
        Matrix4x4(const Vector4& vecX, const Vector4& vecY, const Vector4& vecZ, const Vector4& vecOffset) : glm::mat4(vecX, vecY, vecZ, vecOffset) {};
        Matrix4x4(glm::mat4 mat) : glm::mat4(mat) {};

        static Matrix4x4 Identity();
        static Matrix4x4 Translate(const Vector3& amt);
        static Matrix4x4 Scale(const Vector3& amt);
        static Matrix4x4 Scale(float amt);
        static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float n, float f);
        static Matrix4x4 Perspective(float halfFov, float width, float height, float nearZ, float farZ);
        static Matrix4x4 PerspectiveRH(float halfFov, float width, float height, float nearZ, float farZ);
        static Matrix4x4 TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
        static Matrix4x4 InitRotationFromVectors(const Vector3& u, const Vector3& v, const Vector3& n);
        static Matrix4x4 InitRotationFromDirection(const Vector3& forward, const Vector3& up);
        static Matrix4x4 InitRotation(const Quaternion& quaternion);
        static Matrix4x4 InitLookAt(const Vector3& location, const Vector3& forward, const Vector3& up);
        static Matrix4x4 InitLookAtRH(const Vector3& location, const Vector3& forward, const Vector3& up);
        float         Determinant4x4() const;
        Matrix4x4        ToNormalMatrix() const;
        Matrix4x4        Transpose() const;
        Matrix4x4        Inverse() const;
        Matrix4x4        ApplyScale(const Vector3& scale);

        Vector3 GetScale()
        {
            return Vector3((*this)[0][0], (*this)[1][1], (*this)[2][2]);
        }

        Vector3 GetTranslation()
        {
            return Vector3((*this)[3][0], (*this)[3][1], (*this)[3][2]);
        }

        void        Decompose(Vector3& position, Quaternion& rotation, Vector3& scale);
        static bool Decompose(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

        std::string ToString();

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive((*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[0][3], (*this)[1][0], (*this)[1][1], (*this)[1][2], (*this)[1][3], (*this)[2][0], (*this)[2][1], (*this)[2][2], (*this)[2][3], (*this)[3][0], (*this)[3][1], (*this)[3][2], (*this)[3][3]);
        }
    };
}