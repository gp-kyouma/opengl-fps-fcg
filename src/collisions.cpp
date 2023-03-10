#include "collisions.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

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
    //
}
