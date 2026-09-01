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

enum WeaponEffect
{
    NO_EFFECT,
    SCATTER,
    AIM_SLOWDOWN,
};

struct WeaponEffectParams // this is stupid lol. but it works
{
    int int_value;
    float float_value;
    bool bool_value;
    std::string string_value;
    glm::vec3 vec3_value;
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

    std::map<WeaponEffect,WeaponEffectParams> effects;// efeito(s) extra(s) causado(s) pela arma

    std::vector<Projectile> fire(glm::vec3 pos, glm::vec3 dir);
    bool has_effect(WeaponEffect ef);
};

NLOHMANN_JSON_SERIALIZE_ENUM( WeaponEffect, {
    {NO_EFFECT, "none"},
    {SCATTER, "scatter"},
    {AIM_SLOWDOWN, "aim_slow"},
});

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WeaponEffectParams, int_value, float_value, bool_value, string_value, vec3_value);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Weapon, type, dd_key, proj_type, damage, cooldown, spread, aim_displace, forced_aim, aim_speed, drw_speed, effects);

#endif // FCG_WEAPON
