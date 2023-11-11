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
        Matrix4x4();
        Matrix4x4(const Vector4& vecX, const Vector4& vecY, const Vector4& vecZ, const Vector4& vecOffset);
        Matrix4x4(glm::mat4 mat);

    public:
        float Determinant4x4() const;
        Matrix4x4 ToNormalMatrix() const;
        Matrix4x4 Transpose() const;
        Matrix4x4 Inverse() const;
        Matrix4x4 ApplyScale(const Vector3& scale);
        Vector3 GetScale();
        Vector3 GetTranslation();
        void Decompose(Vector3& position, Quaternion& rotation, Vector3& scale);
        std::string ToString();

    public:
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
        static bool Decompose(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

    public:
        void to_json(json& j, const Matrix4x4& mat)
        {
            j = json
            {
                { "[0][0]", mat[0][0] }, { "[0][1]", mat[0][1] },{ "[0][2]", mat[0][2] },{ "[0][3]", mat[0][3] },
                { "[1][0]", mat[1][0] }, { "[1][1]", mat[1][1] },{ "[1][2]", mat[1][2] },{ "[1][3]", mat[1][3] },
                { "[2][0]", mat[2][0] }, { "[2][1]", mat[2][1] },{ "[2][2]", mat[2][2] },{ "[2][3]", mat[2][3] },
                { "[3][0]", mat[3][0] }, { "[3][1]", mat[3][1] },{ "[3][2]", mat[3][2] },{ "[3][3]", mat[3][3] }
            };
        }

        void from_json(const json& j, Matrix4x4& mat)
        {
            mat[0][0] = j.at("[0][0]");
            mat[0][1] = j.at("[0][1]");
            mat[0][2] = j.at("[0][2]");
            mat[0][3] = j.at("[0][3]");

            mat[1][0] = j.at("[1][0]");
            mat[1][1] = j.at("[1][1]");
            mat[1][2] = j.at("[1][2]");
            mat[1][3] = j.at("[1][3]");

            mat[2][0] = j.at("[2][0]");
            mat[2][1] = j.at("[2][1]");
            mat[2][2] = j.at("[2][2]");
            mat[2][3] = j.at("[2][3]");

            mat[3][0] = j.at("[3][0]");
            mat[3][1] = j.at("[3][1]");
            mat[3][2] = j.at("[3][2]");
            mat[3][3] = j.at("[3][3]");
        }
    };
}