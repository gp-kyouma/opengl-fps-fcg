#include "weapon.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "vec_aux.h"
#include "matrices.h"

std::vector<Projectile> Weapon::fire(glm::vec3 pos, glm::vec3 dir)
{
    Projectile new_proj;
    std::vector<Projectile> result;
    result.clear();

    new_proj.setProjectileData(proj_type);

    if (new_proj.damage_override)
        new_proj.damage = damage;

    new_proj.pos = pos;
    new_proj.view = dir;

    result.push_back(new_proj);

    if (has_effect(SCATTER) || spread > 0)
    {
        // cria novos projéteis/faz alterações
        // baseadas no projétil que acabou de ser atirado:

        // obtém o "sistema de coordenadas" do projétil
        glm::vec4 u,v,w;
        calculate_uvw(dir,u,v,w);

        if (has_effect(SCATTER))
        {
            // dynamic rotation around main proj
            float angle = glm::radians(effects[SCATTER].float_value);
            int spread_count = effects[SCATTER].int_value;

            float rotation = glm::radians(360.0f / (float)spread_count);
            glm::vec4 revolve = Matrix_Rotate(angle, (spread_count % 2) ? u : v) * Vetor(new_proj.view);

            for (int i = 0; i < spread_count; i++)
            {
                Projectile spread = new_proj;
                spread.view = toVec3(Matrix_Rotate(rotation * i, w) * revolve);
                result.push_back(spread);
            }
        }

        if (spread > 0)
        {
            for (unsigned int i = 0; i < result.size(); i++)
                result[i].doRandomSpread(spread, u, v);
        }
    }

    return result;
}

bool Weapon::has_effect(WeaponEffect ef)
{
    return (effects.count(ef));
}
