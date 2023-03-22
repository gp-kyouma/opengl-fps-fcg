#include "collisions.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <algorithm>

glm::vec3 AABB::getSize()
{
    return aabb_max - aabb_min;
}

glm::vec3 AABB::getCenter()
{
    return (aabb_max + aabb_min) / 2.0f;
}

// AABB x AABB
bool Collide(AABB aabb1, AABB aabb2)
{
    // FONTE:
    // https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
    return
    (aabb1.aabb_min.x <= aabb2.aabb_max.x &&
     aabb1.aabb_max.x >= aabb2.aabb_min.x &&
     aabb1.aabb_min.y <= aabb2.aabb_max.y &&
     aabb1.aabb_max.y >= aabb2.aabb_min.y &&
     aabb1.aabb_min.z <= aabb2.aabb_max.z &&
     aabb1.aabb_max.z >= aabb2.aabb_min.z);
}

// AABB x AABB com parâmetro extra de saída
// (vetor de deslocamento que deve ser aplicado a aabb1 para resolver a colisão, se houver)
bool Collide(AABB aabb1, AABB aabb2, glm::vec3 &desloc)
{
    desloc = glm::vec3(0.0f,0.0f,0.0f);

    if (!Collide(aabb1,aabb2))
        return false;

    glm::vec3 eixoX = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 eixoY = glm::vec3(0.0f,1.0f,0.0f);
    glm::vec3 eixoZ = glm::vec3(0.0f,0.0f,1.0f);

    glm::vec3 halfsize_1 = aabb1.getSize() / 2.0f;
    glm::vec3 halfsize_2 = aabb2.getSize() / 2.0f;

    // código baseado em FONTE, com algumas alterações:
    // https://stackoverflow.com/a/46307470

    // Calculate current and minimum-non-intersecting distances between centers.
    glm::vec3 distance = aabb1.getCenter() - aabb2.getCenter();
    glm::vec3 minDistance = halfsize_1 + halfsize_2;

    // Calculate and return intersection depths.
    float depthX = ((distance.x) > 0) ? (minDistance.x - distance.x) : (-minDistance.x - distance.x);
    float depthY = ((distance.y) > 0) ? (minDistance.y - distance.y) : (-minDistance.y - distance.y);
    float depthZ = ((distance.z) > 0) ? (minDistance.z - distance.z) : (-minDistance.z - distance.z);

    // /fonte

    if (fabs(depthY) <= fabs(depthX) && fabs(depthY) <= fabs(depthZ))
        desloc = eixoY * depthY;
    else if (fabs(depthZ) <= fabs(depthX) && fabs(depthZ) <= fabs(depthY))
        desloc = eixoZ * depthZ;
    else
        desloc = eixoX * depthX;

    return true;
}

// Sphere x AABB
bool Collide(Sphere sphere, AABB aabb)
{
    // FONTE:
    // https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection

    // get box closest point to sphere center by clamping
    float x = std::max(aabb.aabb_min.x, std::min(sphere.center.x, aabb.aabb_max.x));
    float y = std::max(aabb.aabb_min.y, std::min(sphere.center.y, aabb.aabb_max.y));
    float z = std::max(aabb.aabb_min.z, std::min(sphere.center.z, aabb.aabb_max.z));

    // this is the same as isPointInsideSphere
    float distance = (float) sqrt(
        (x - sphere.center.x) * (x - sphere.center.x) +
        (y - sphere.center.y) * (y - sphere.center.y) +
        (z - sphere.center.z) * (z - sphere.center.z)
    );

    return (distance < sphere.radius);
}

// Ray x AABB
// Retorna distância mínima como parâmetro extra
bool Collide(Ray ray, AABB aabb, float max_range, float &min_dist)
{
    const float epsilon = 0.00001f;

    float tmin = 0.0f;
    float tmax = max_range;

    // código baseado em FONTE:
    // https://github.com/juj/MathGeoLib/blob/2940b99b99cfe575dd45103ef20f4019dee15b54/src/Geometry/AABB.cpp#L725

    // EIXO X
    if (!(fabs(ray.direction.x) < epsilon))
	{
		float recipDir = (1/ray.direction.x);
		float t1 = (aabb.aabb_min.x - ray.origin.x) * recipDir;
		float t2 = (aabb.aabb_max.x - ray.origin.x) * recipDir;

		if (t1 < t2)
        {
            tmin = std::max(t1, tmin);
            tmax = std::min(t2, tmax);
        }
		else // Swap t1 and t2
		{
            tmin = std::max(t2, tmin);
            tmax = std::min(t1, tmax);
		}

		if (tmin > tmax)
			return false; // Box is missed since we "exit" before entering it.
	}
	else if (ray.origin.x < aabb.aabb_min.x || ray.origin.x > aabb.aabb_max.x)
		return false; // The ray can't possibly enter the box, abort.

	// EIXO Y
    if (!(fabs(ray.direction.y) < epsilon))
	{
		float recipDir = (1/ray.direction.y);
		float t1 = (aabb.aabb_min.y - ray.origin.y) * recipDir;
		float t2 = (aabb.aabb_max.y - ray.origin.y) * recipDir;

		if (t1 < t2)
        {
            tmin = std::max(t1, tmin);
            tmax = std::min(t2, tmax);
        }
		else // Swap t1 and t2
		{
            tmin = std::max(t2, tmin);
            tmax = std::min(t1, tmax);
		}

		if (tmin > tmax)
			return false; // Box is missed since we "exit" before entering it.
	}
	else if (ray.origin.y < aabb.aabb_min.y || ray.origin.y > aabb.aabb_max.y)
		return false; // The ray can't possibly enter the box, abort.

	// EIXO Z
    if (!(fabs(ray.direction.z) < epsilon))
	{
		float recipDir = (1/ray.direction.z);
		float t1 = (aabb.aabb_min.z - ray.origin.z) * recipDir;
		float t2 = (aabb.aabb_max.z - ray.origin.z) * recipDir;

		if (t1 < t2)
        {
            tmin = std::max(t1, tmin);
            tmax = std::min(t2, tmax);
        }
		else // Swap t1 and t2
		{
            tmin = std::max(t2, tmin);
            tmax = std::min(t1, tmax);
		}

		if (tmin > tmax)
			return false; // Box is missed since we "exit" before entering it.
	}
	else if (ray.origin.z < aabb.aabb_min.z || ray.origin.z > aabb.aabb_max.z)
		return false; // The ray can't possibly enter the box, abort.

    min_dist = tmin;

	return tmin <= tmax;
}
