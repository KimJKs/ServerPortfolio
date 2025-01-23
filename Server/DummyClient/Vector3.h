#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>
#include <numbers>

class Vector3
{
public:
    float x, y, z;
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    // ∫§≈Õ µ°º¿
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // ∫§≈Õ ª¨º¿
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Ω∫ƒÆ∂Û ∞ˆº¿
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // ∫§≈Õ¿« ±Ê¿Ã ∞ËªÍ
    float Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // ∫§≈Õ ¡§±‘»≠
    Vector3 Normalize() const {
        float mag = Magnitude();
        if (mag == 0.0f)
            return Vector3(0, 0, 0);
        return Vector3(x / mag, y / mag, z / mag);
    }

    // ∫§≈Õ ªÁ¿Ã¿« ∞≈∏Æ ∞ËªÍ
    static float Distance(const Vector3& a, const Vector3& b) {
        return (a - b).Magnitude();
    }

    // ∫§≈Õ ªÁ¿Ã¿« ∞¢µµ ∞ËªÍ (Degree)
    static float Angle(const Vector3& a, const Vector3& b) {
        float dotProduct = a.Dot(b);
        float magA = a.Magnitude();
        float magB = b.Magnitude();

        if (magA == 0.0f || magB == 0.0f)
            return 0.0f;

        // Cosine of the angle
        float cosTheta = dotProduct / (magA * magB);

        // Clamp to avoid numerical errors
        cosTheta = std::clamp(cosTheta, -1.0f, 1.0f);

        // Return angle in degrees
        return std::acos(cosTheta) * (180.0f / std::numbers::pi);
    }

    // Dot product
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

};
#endif