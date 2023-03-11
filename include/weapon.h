#ifndef FCG_WEAPON
#define FCG_WEAPON

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
STRUCTS/CLASSES
*/

enum WeaponType
{
    WPN_SWORD,
    WPN_PISTOL,
    WPN_SHOTGUN,
    WPN_MINIGUN,
    WPN_SNIPER,
};

struct Weapon
{
    WeaponType wpn_type; // define o modelo e animação
    //todo:
    //ProjectileType proj_type;

    int damage;
    float cooldown;

    //todo:
    //Projectile fire(glm::vec3 pos, glm::vec3 dir);
};

#endif // FCG_WEAPON
