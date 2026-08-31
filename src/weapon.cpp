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

    new_proj.pos = pos;
    new_proj.view = dir;
    new_proj.damage = damage;

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
            const float pi = 3.141592f;

            // old (manual spread)
            /*
            const float pi24 = pi / 24.0f;
            Projectile spread1 = new_proj;
            Projectile spread2 = new_proj;
            Projectile spread3 = new_proj;
            Projectile spread4 = new_proj;

            Projectile spread5 = new_proj;
            Projectile spread6 = new_proj;
            spread1.view = toVec3(Matrix_Rotate( pi24, v) * Matrix_Rotate( pi24/2, u) * Vetor(spread1.view));
            spread2.view = toVec3(Matrix_Rotate( pi24, v) * Matrix_Rotate(-pi24/2, u) * Vetor(spread2.view));
            spread3.view = toVec3(Matrix_Rotate(-pi24, v) * Matrix_Rotate( pi24/2, u) * Vetor(spread3.view));
            spread4.view = toVec3(Matrix_Rotate(-pi24, v) * Matrix_Rotate(-pi24/2, u) * Vetor(spread4.view));

            spread5.view = toVec3(Matrix_Rotate( pi24*2, v) * Vetor(spread5.view));
            spread6.view = toVec3(Matrix_Rotate(-pi24*2, v) * Vetor(spread6.view));

            result.push_back(spread1);
            result.push_back(spread2);
            result.push_back(spread3);
            result.push_back(spread4);

            result.push_back(spread5);
            result.push_back(spread6);
            */

            // new (dynamic rotation around main proj)
            float angle = pi / 24.0f;
            int spread_count = 6;
            float rotation = (pi*2) / (float)spread_count;

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
    if (std::find(effects.begin(), effects.end(), ef) != effects.end())
        return true;

    return false;
}
