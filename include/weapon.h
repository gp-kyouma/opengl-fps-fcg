#ifndef FCG_WEAPON
#define FCG_WEAPON

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "projectile.h"

#include <vector>

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

enum WeaponEffect
{
    NO_EFFECT,
    SCATTER,
    AIM_SLOWDOWN,
};

enum WeaponState
{
    WPNSTATE_DRAW,
    WPNSTATE_READY,
    WPNSTATE_COOLDOWN,
    //WPNSTATE_RELOAD,
};

struct Weapon
{
    WeaponType wpn_type; // define o modelo e animação
    ProjectileType proj_type;

    int damage;
    float cooldown;

    int spread;

    glm::vec3 aim_displace;
    bool forced_aim = false; // arma precisa estar mirada (sem hipfire)
    float aim_speed;
    float drw_speed;

    WeaponEffect effect; // efeito extra causado pela arma

    std::vector<Projectile> fire(glm::vec3 pos, glm::vec3 dir);
};

#endif // FCG_WEAPON
