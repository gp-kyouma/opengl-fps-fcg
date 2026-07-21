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

    if (effect == SCATTER || spread > 0)
    {
        // cria novos projéteis/faz alterações
        // baseadas no projétil que acabou de ser atirado:

        // obtém o "sistema de coordenadas" do projétil
        glm::vec4 u,v,w;
        calculate_uvw(dir,u,v,w);

        const float pi24 = 3.141592f / 24.0f;

        if (effect == SCATTER)
        {
            Projectile spread1 = new_proj;
            Projectile spread2 = new_proj;
            Projectile spread3 = new_proj;
            Projectile spread4 = new_proj;

            Projectile spread5 = new_proj;
            Projectile spread6 = new_proj;

            // rotaciona

            // old (radial spread)
            /*
            spread1.dir = toVec3(Matrix_Rotate( pi24*2, v) * Vetor(spread1.dir));
            spread2.dir = toVec3(Matrix_Rotate( pi24,   v) * Vetor(spread2.dir));
            spread3.dir = toVec3(Matrix_Rotate(-pi24,   v) * Vetor(spread3.dir));
            spread4.dir = toVec3(Matrix_Rotate(-pi24*2, v) * Vetor(spread4.dir));
            */

            // new (x-spread)
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
        }

        if (spread > 0)
        {
            for (unsigned int i = 0; i < result.size(); i++)
                result[i].doRandomSpread(spread, u, v);
        }
    }

    return result;
}
