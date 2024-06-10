#include "projectile.h"

#include <algorithm>

#include "matrices.h"
#include "vec_aux.h"
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
            speed = 12;
            lifespan = 4.0f;
            break;
        case PROJ_MELEE_INVISIBLE:
            hit_type = BOX;
            p_size = glm::vec3(1.5f,1.5f,1.5f);
            speed = 0.5f;
            lifespan = 0.15f;
            break;
    }
}

void Projectile::doRandomSpread(int maxOffset, glm::vec4 u, glm::vec4 v)
{
    if (maxOffset < 1) return;

    int offset = maxOffset;
    if (offset > 100) offset = 100;

    const float pi24 = 3.141592f / 24.0f;

    // v1 (should converge to center more)
    float offset_random_u = ((float)(rand() % (offset+1)) - (float)(rand() % (offset+1))) / 100.0f;
    float offset_random_v = ((float)(rand() % (offset+1)) - (float)(rand() % (offset+1))) / 100.0f;

    // v2 (should be more random)
    //float offset_random_u = (((float)(rand() % (offset+1))) / 100.0f) * (((int)rand() % 2 == 0) ? 1 : -1);
    //float offset_random_v = (((float)(rand() % (offset+1))) / 100.0f) * (((int)rand() % 2 == 0) ? 1 : -1);

    dir = toVec3(Matrix_Rotate(offset_random_v*pi24, v) * Matrix_Rotate(offset_random_u*pi24, u) * Vetor(dir));
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
