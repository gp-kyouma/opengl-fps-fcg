#include "weapon.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

Projectile Weapon::fire(glm::vec3 pos, glm::vec3 dir)
{
    Projectile result;

    result.setProjectileData(proj_type);

    result.pos = pos + dir;
    result.dir = dir;
    result.damage = damage;

    return result;
}
