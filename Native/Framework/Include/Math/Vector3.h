#pragma once
#include "glm.h"
#include "Vector2.h"
#include <sstream>

namespace Odyssey
{
    using namespace glm;

    class Vector3 : public glm::vec3
    {
    public:
        Vector3() = default;
        Vector3(float x, float y, float z)
            : glm::vec3(x, y, z) {};
        Vector3(const Vector3 & rhs)
            : glm::vec3(rhs) {};
        Vector3(const Vector2 & rhs)
            : glm::vec3(rhs.x, rhs.y, 0.0f) {};
        Vector3(float val)
            : glm::vec3(val, val, val) {};
        Vector3(const glm::vec3 & rhs)
            : glm::vec3(rhs.x, rhs.y, rhs.z) {};

        static Vector3 Zero;
        static Vector3 Up;
        static Vector3 Down;
        static Vector3 Right;
        static Vector3 Left;
        static Vector3 Forward;
        static Vector3 Back;
        static Vector3 One;

        static Vector3 Lerp(const Vector3& from, const Vector3& to, float t);

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
        Vector2 XY()
        {
            return Vector2(x, y);
        }
        float Dot(const Vector3& other) const;
        float Distance(const Vector3& other) const;
        float Magnitude() const;
        float SqrMagnitude() const;
        float Max() const;
        float Min() const;
        float Avg() const;
        void  Normalize();

        Vector3& operator*=(Vector3 const& v)
        {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            return *this;
        }
        Vector3& operator/=(Vector3 const& v)
        {
            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            return *this;
        }
        Vector3& operator+=(Vector3 const& v)
        {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }
        Vector3& operator-=(Vector3 const& v)
        {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            return *this;
        }
        Vector3& operator*=(float f)
        {
            this->x *= f;
            this->y *= f;
            this->z *= f;
            return *this;
        }
        Vector3& operator/=(float f)
        {
            this->x /= f;
            this->y /= f;
            this->z /= f;
            return *this;
        }
        Vector3& operator+=(float f)
        {
            this->x += f;
            this->y += f;
            this->z += f;
            return *this;
        }
        Vector3& operator-=(float f)
        {
            this->x -= f;
            this->y -= f;
            this->z -= f;
            return *this;
        }
        bool operator==(const Vector3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
        bool operator!=(const Vector3& rhs) const
        {
            return !(x == rhs.x && y == rhs.y && z == rhs.z);
        }
        bool operator>(const Vector3& rhs) const
        {
            return length() > rhs.length();
        }
        bool operator<(const Vector3& rhs) const
        {
            return length() < rhs.length();
        }
        float& operator[](unsigned int i)
        {
            return (&x)[i];
        }
        Vector3 operator-() const
        {
            return Vector3(-x, -y, -z);
        }
        float* Get()
        {
            return &x;
        }

        std::ostream& operator<<(std::ostream& os)
        {
            return os << "X:" << x << "   Y:" << y << "   Z:" << z;
        }

        std::string ToString() const
        {
            std::stringstream ss;
            ss << "X:" << x << "   Y:" << y << "   Z:" << z;
            return ss.str();
        }

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(x, y, z);
        }
    };
}