#pragma once
#include "glm.h"
#include "Vector2.h"
#include <sstream>
#include "Serialization.h"

namespace Odyssey
{
    using namespace glm;

    class Vector3 : public glm::vec3
    {
    public:
        Vector3() = default;
        Vector3(float x, float y, float z);
        Vector3(const Vector3& rhs);
        Vector3(const Vector2& rhs);
        Vector3(float val);
        Vector3(const glm::vec3& rhs);

    public:
        Vector3& operator*=(Vector3 const& v);
        Vector3& operator/=(Vector3 const& v);
        Vector3& operator+=(Vector3 const& v);
        Vector3& operator-=(Vector3 const& v);
        Vector3& operator*=(float f);
        Vector3& operator/=(float f);
        Vector3& operator+=(float f);
        Vector3& operator-=(float f);
        bool operator==(const Vector3& rhs) const;
        bool operator!=(const Vector3& rhs) const;
        bool operator>(const Vector3& rhs) const;
        bool operator<(const Vector3& rhs) const;
        float& operator[](unsigned int i);
        Vector3 operator-() const;
        std::ostream& operator<<(std::ostream& os);

    public:
        static Vector3 Lerp(const Vector3& from, const Vector3& to, float t);

    public:
        Vector3 Cross(const Vector3& other) const;
        Vector3 Abs() const;
        Vector3 Min(const Vector3& other) const;
        Vector3 Max(const Vector3& other) const;
        Vector3 Normalized() const;
        Vector3 Project(const Vector3& normal) const;
        Vector3 Rotate(const Vector3& axis, float angle) const;
        Vector3 Rotate(const class Quaternion& rotation) const;
        Vector3 Reflect(const Vector3& normal) const;
        Vector3 Refract(const Vector3& normal, float indexOfRefraction) const;
        Vector2 XY();
        float Dot(const Vector3& other) const;
        float Distance(const Vector3& other) const;
        float Magnitude() const;
        float SqrMagnitude() const;
        float Max() const;
        float Min() const;
        float Avg() const;
        void  Normalize();
        float* Get();
        std::string ToString() const;

    public:
        static Vector3 Zero;
        static Vector3 Up;
        static Vector3 Down;
        static Vector3 Right;
        static Vector3 Left;
        static Vector3 Forward;
        static Vector3 Back;
        static Vector3 One;
        ODYSSEY_SERIALIZE(Vector3, x, y, z);
    };
}