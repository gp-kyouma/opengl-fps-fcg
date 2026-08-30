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

enum WeaponEffect//TODO turn this into a stringkey
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
    std::string type;
    std::string dd_key;// draw data key

    std::string proj_type;

    int damage;
    float cooldown;

    int spread;

    glm::vec3 aim_displace;
    bool forced_aim = false; // arma precisa estar mirada (sem hipfire)
    float aim_speed;
    float drw_speed;

    WeaponEffect effect; // efeito extra causado pela arma//This should be a map of strings and...strings?

    std::vector<Projectile> fire(glm::vec3 pos, glm::vec3 dir);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Weapon, type, dd_key, proj_type, damage, cooldown, spread, aim_displace, forced_aim, aim_speed, drw_speed, effect);

#endif // FCG_WEAPON
