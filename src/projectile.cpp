#include "projectile.h"

#include <algorithm>

#include "timer_aux.h"

void Projectile::setProjectileData(ProjectileType type)
{
    this->type = type;

    const float max_range = 100.0f;

    switch (type)
    {
        case PROJ_HITSCAN:
            hit_type = RAY;
            p_size = glm::vec3(1.0f,1.0f,max_range);
            speed = 0;
            lifespan = 0.15f;
            break;
        case PROJ_BULLET:
            hit_type = SPHERE;
            p_size = glm::vec3(0.1f,0.1f,0.1f);
            speed = 2;//10;
            lifespan = 4.0f;
            break;
        case PROJ_MELEE_INVISIBLE:
            hit_type = BOX;
            p_size = glm::vec3(1.0f,1.0f,1.0f);
            speed = 0.5f;
            lifespan = 0.15f;
            break;
    }
}

AABB Projectile::getHitbox()
{
    AABB result;
    glm::vec3 half = p_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

Sphere Projectile::getHitsphere()
{
    Sphere result;
    glm::vec3 half = p_size / 2.0f;
    float largest = std::max(half.x, std::max(half.y, half.z));

    result.center = pos;
    result.radius = largest;

    return result;
}

Ray Projectile::getHitscan()
{
    Ray result;

    result.origin    = pos;
    result.direction = dir;

    return result;
}

void Projectile::update(float deltaTime)
{
    pos += dir * speed * deltaTime;
    decrementTimer(lifespan, deltaTime, 0.0f);
}

bool Projectile::isDead()
{
    return (lifespan == 0.0f);
}
