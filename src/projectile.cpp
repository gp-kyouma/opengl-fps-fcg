#include "projectile.h"

#include <algorithm>

#include "matrices.h"
#include "vec_aux.h"
#include "timer_aux.h"

void Projectile::setProjectileData(ProjectileType type)
{
    this->type = type;

    const float max_range = 50.0f;

    switch (type)
    {
        case PROJ_HITSCAN:
            hit_type = RAY;
            e_size = glm::vec3(1.0f,1.0f,max_range);
            speed = 0;
            lifespan = 0.15f;
            break;
        case PROJ_BULLET:
            hit_type = SPHERE;
            e_size = glm::vec3(0.1f,0.1f,0.1f);
            speed = 12;
            lifespan = 4.0f;
            break;
        case PROJ_MELEE_INVISIBLE:
            hit_type = BOX;
            e_size = glm::vec3(1.5f,1.5f,1.5f);
            speed = 0.5f;
            lifespan = 0.15f;
            break;
    }
    //TODO tinker with kb values
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

    view = toVec3(Matrix_Rotate(offset_random_v*pi24, v) * Matrix_Rotate(offset_random_u*pi24, u) * Vetor(view));
}

AABB Projectile::getHitbox()
{
    AABB result;
    glm::vec3 half = e_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

Sphere Projectile::getHitsphere()
{
    Sphere result;
    glm::vec3 half = e_size / 2.0f;
    float largest = std::max(half.x, std::max(half.y, half.z));

    result.center = pos;
    result.radius = largest;

    return result;
}

Ray Projectile::getHitscan()
{
    Ray result;

    result.origin    = pos;
    result.direction = view;

    return result;
}

bool Projectile::collideAgainstEntity(Entity& entity, float& min_dist)
{
    AABB hitbox = entity.getHitbox();// currently assumes that Entity always uses AABB

    return collideAgainstAABB(hitbox,min_dist);
}

bool Projectile::collideAgainstAABB(AABB aabb, float& min_dist)
{
    bool result = false;

    if (lifespan > 0.0f)
        switch (hit_type)
        {
            case BOX:
                result = Collide(getHitbox(),aabb);
                break;
            case SPHERE:
                result = Collide(getHitsphere(),aabb);
                break;
            case RAY:
                result = Collide(getHitscan(),aabb,e_size.z,min_dist);
                break;
            case POINT_3D:
                result = Collide(pos,aabb);
                break;
            default:
                break;
        }
    return result;
}

void Projectile::update(float deltaTime)
{
    pos += view * speed * deltaTime;
    decrementTimer(lifespan, deltaTime, 0.0f);
}

bool Projectile::isDead()
{
    return (lifespan == 0.0f);
}

void Projectile::apply_kb(float power, glm::vec3 direction){}//unimplemented
