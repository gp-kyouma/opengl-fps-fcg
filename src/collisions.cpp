#include "collisions.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

glm::vec3 getAABBSize(AABB aabb)
{
    return aabb.aabb_max - aabb.aabb_min;
}

glm::vec3 getAABBCenter(AABB aabb)
{
    return (aabb.aabb_max + aabb.aabb_min) / 2.0f;
}
