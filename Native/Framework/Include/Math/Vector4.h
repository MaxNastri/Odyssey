#pragma once
#include "glm.h"
#include "Vector3.h"
#include "Vector2.h"
#include <sstream>

namespace Odyssey
{
    class Vector4 : public glm::vec4
    {
    public:
        Vector4() = default;
        Vector4(float x, float y, float z, float w)
            : glm::vec4(x, y, z, w) {};
        Vector4(const Vector4 & rhs)
            : glm::vec4(rhs) {};
        Vector4(const Vector3 & rhs)
            : glm::vec4(rhs.x, rhs.y, rhs.z, 0.0f) {};
        Vector4(const Vector2 & rhs)
            : glm::vec4(rhs.x, rhs.y, 0.0f, 0.0f) {};
        Vector4(float f)
            : glm::vec4(f, f, f, f) {};
        Vector4(const glm::vec4 & rhs)
            : glm::vec4(rhs.x, rhs.y, rhs.z, rhs.w) {};
        Vector4(const Vector3 & src, float w)
            : glm::vec4(src, w) {};

        static Vector4 Zero;
        static Vector4 One;
        Vector4 Abs() const;
        Vector4 Min(const Vector4& other) const;
        Vector4 Max(const Vector4& other) const;
        Vector4 Normalized() const;
        Vector4 Project(const Vector4& normal) const;
        Vector4 Rotate(const class Quaternion& rotation) const;
        Vector4 Rotate(const Vector3& axis, float angle) const;
        Vector4 Reflect(const Vector4& normal) const;
        Vector4 Refract(const Vector4& normal, float indexOfRefraction) const;
        Vector3 XYZ()
        {
            return Vector3(x, y, z);
        }
        Vector2 XY()
        {
            return Vector2(x, y);
        }
        float Dot(const Vector4& other) const;
        float Distance(const Vector4& other) const;
        float Magnitude() const;
        float MagnitudeSqrt() const;
        float Max() const;
        float Min() const;
        float Avg() const;
        void  Normalize();

        Vector4& operator*=(Vector4 const& v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }
        Vector4& operator/=(Vector4 const& v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }
        Vector4& operator+=(Vector4 const& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        Vector4& operator-=(Vector4 const& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        Vector4& operator*=(float f)
        {
            x *= f;
            y *= f;
            z *= f;
            w *= f;
            return *this;
        }
        Vector4& operator/=(float f)
        {
            x /= f;
            y /= f;
            z /= f;
            w /= f;
            return *this;
        }
        Vector4& operator+=(float f)
        {
            x += f;
            y += f;
            z += f;
            w += f;
            return *this;
        }
        Vector4& operator-=(float f)
        {
            x -= f;
            y -= f;
            z -= f;
            w -= f;
            return *this;
        }
        bool operator==(const Vector4& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }
        bool operator!=(const Vector4& rhs) const
        {
            return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
        }
        bool operator>(const Vector4& rhs) const
        {
            return length() > rhs.length();
        }
        bool operator<(const Vector4& rhs) const
        {
            return length() < rhs.length();
        }
        float& operator[](unsigned int i)
        {
            return (&x)[i];
        }
        Vector4 operator-() const
        {
            return Vector4(-*this);
        }
        float* Get()
        {
            return &x;
        }

        std::ostream& operator<<(std::ostream& os)
        {
            return os << "X:" << x << "   Y:" << y << "   Z:" << z << "   W:" << w;
        }

        std::string ToString() const;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(x, y, z, w);
        }
    };
}