#pragma once
#include "pch.h"

struct BoundingBox {
    Vector3 minPoint;
    Vector3 maxPoint;

    BoundingBox(Vector3 min = Vector3(), Vector3 max = Vector3())
        : minPoint(std::move(min)), maxPoint(std::move(max)) {
    }

    bool operator==(const BoundingBox& other) const {
        return minPoint.x == other.minPoint.x && minPoint.z == other.minPoint.z &&
            maxPoint.x == other.maxPoint.x && maxPoint.z == other.maxPoint.z;
    }

    bool Contains(const Vector3& point) const {
        return point.x >= minPoint.x && point.x <= maxPoint.x &&
            point.z >= minPoint.z && point.z <= maxPoint.z;
    }

    bool Intersects(const BoundingBox& other) const {
        return !(other.minPoint.x > maxPoint.x || other.maxPoint.x < minPoint.x ||
            other.minPoint.z > maxPoint.z || other.maxPoint.z < minPoint.z);
    }
};

