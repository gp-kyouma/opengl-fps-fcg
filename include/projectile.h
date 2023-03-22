#ifndef FCG_PROJECTILE
#define FCG_PROJECTILE

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <collisions.h>

enum ProjectileType
{
    PROJ_HITSCAN,
    PROJ_BULLET,
    PROJ_MELEE_INVISIBLE,
};

struct Projectile
{
    ProjectileType type;
    HitboxType hit_type;

    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 p_size;

    float speed;
    float lifespan;

    int damage;

    void setProjectileData(ProjectileType type);

    AABB   getHitbox();
    Sphere getHitsphere();
    Ray    getHitscan();

    void update(float deltaTime);
    bool isDead();
};

#endif // FCG_PROJECTILE
