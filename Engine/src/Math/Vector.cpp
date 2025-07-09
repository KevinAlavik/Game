#include <Math/Vector.hpp>

namespace Math {
    Vector2f::Vector2f() : x(0.0f), y(0.0f) {}

    Vector2f::Vector2f(float x, float y) : x(x), y(y) {}

    Vector2f Vector2f::operator+(const Vector2f& rhs) const {
        return Vector2f(x + rhs.x, y + rhs.y);
    }

    Vector2f Vector2f::operator-(const Vector2f& rhs) const {
        return Vector2f(x - rhs.x, y - rhs.y);
    }

    Vector2f& Vector2f::operator+=(const Vector2f& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2f& Vector2f::operator-=(const Vector2f& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector2f Vector2f::operator*(float scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    Vector2f& Vector2f::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }


    bool Vector2f::operator==(const Vector2f& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool Vector2f::operator!=(const Vector2f& rhs) const {
        return !(*this == rhs);
    }
}

namespace Math {
    Vector3f::Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}

    Vector3f::Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3f Vector3f::operator+(const Vector3f& rhs) const {
        return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3f Vector3f::operator-(const Vector3f& rhs) const {
        return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3f& Vector3f::operator+=(const Vector3f& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vector3f& Vector3f::operator-=(const Vector3f& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vector3f Vector3f::operator*(float scalar) const {
        return Vector3f(x * scalar, y * scalar, z * scalar);
    }

    Vector3f& Vector3f::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    bool Vector3f::operator==(const Vector3f& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool Vector3f::operator!=(const Vector3f& rhs) const {
        return !(*this == rhs);
    }
}

namespace Math {
    Vector4f::Vector4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

    Vector4f::Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Vector4f Vector4f::operator+(const Vector4f& rhs) const {
        return Vector4f(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    Vector4f Vector4f::operator-(const Vector4f& rhs) const {
        return Vector4f(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Vector4f& Vector4f::operator+=(const Vector4f& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    Vector4f& Vector4f::operator-=(const Vector4f& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    Vector4f Vector4f::operator*(float scalar) const {
        return Vector4f(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vector4f& Vector4f::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    bool Vector4f::operator==(const Vector4f& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool Vector4f::operator!=(const Vector4f& rhs) const {
        return !(*this == rhs);
    }
}