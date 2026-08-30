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

struct Projectile : Entity
{
    //Entity attributes omitted

    HitboxType hit_type;

    float lifespan;
    float base_lifespan;//does not need to exist once global map exists

    int damage;

    void setProjectileData(std::string type);
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

NLOHMANN_JSON_SERIALIZE_ENUM( HitboxType, {
    {BOX, "box"},
    {SPHERE, "sphere"},
    {RAY, "ray"},
    {POINT_3D, "point"},
});

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Projectile, type, dd_key, hit_type, e_size, speed, base_lifespan, damage, kb_power);

#endif // FCG_PROJECTILE
