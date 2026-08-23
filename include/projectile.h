#ifndef FCG_PROJECTILE
#define FCG_PROJECTILE

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"
#include "entity.h"

/*
STRUCTS/CLASSES
*/

enum ProjectileType
{
    PROJ_HITSCAN,
    PROJ_BULLET,
    PROJ_MELEE_INVISIBLE,
};

struct Projectile : Entity
{
    //Entity attributes omitted

    ProjectileType type;
    HitboxType hit_type;

    float lifespan;

    int damage;

    void setProjectileData(ProjectileType type);
    void doRandomSpread(int maxOffset, glm::vec4 u, glm::vec4 v);

    AABB   getHitbox();
    Sphere getHitsphere();
    Ray    getHitscan();

    bool collideAgainstAABB(AABB aabb, float& min_dist);
    bool collideAgainstEntity(Entity& entity, float& min_dist);

    void update(float deltaTime);
    bool isDead();

    void apply_kb(float power, glm::vec3 direction);//unimplemented

    void draw(); //Defined in draw.cpp
};

#endif // FCG_PROJECTILE
