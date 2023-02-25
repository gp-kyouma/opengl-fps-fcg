#include "player.h"

#include <cstdio>
#include <cstdlib>

#include "input.h"
#include "vec_aux.h"

#include "matrices.h"

void Player::setView(float theta, float phi)
{
    view.x = cos(phi)*sin(theta);
    view.y = sin(phi);
    view.z = cos(phi)*cos(theta);
}

void Player::movePos(glm::vec3 offset)
{
    pos += offset;
}

AABB Player::getAABB()
{
    AABB result;
    glm::vec3 half = p_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

void Player::doPlayerMovement(float deltaTime)
{
    // jogador anda na direção onde está olhando
    glm::vec4 v_up = glm::vec4(0.0f,1.0f,0.0f,0.0f);  // Vetor "up" fixado para apontar para o "céu" (eixo Y global)

    glm::vec4 w = Vetor(-view);
    glm::vec4 u = crossproduct(v_up,w);

    // faz com que o jogador não possa andar pra cima
    w.y = 0.0f;
    u.y = 0.0f;

    w = w / norm(w);
    u = u / norm(u);

    glm::vec4 offset = glm::vec4(0.0f,0.0f,0.0f,0.0f);

    if (g_WKeyPressed)
    {
        offset -= w;
    }
    if (g_SKeyPressed)
    {
        offset += w;
    }
    if (g_AKeyPressed)
    {
        offset -= u;
    }
    if (g_DKeyPressed)
    {
        offset += u;
    }

    if (norm(offset) != 0) // evita divisão por 0
        offset = offset / norm(offset);

    // jump mechanic
    const float gravity = 5.0f;
    const float jump_force = 4.5f;

    if (g_SpaceBarKeyPressed && grounded)
    {
        grounded = false;
        y_velocity = jump_force;
    }
    else if (!grounded)
    {
        y_velocity -= (gravity * deltaTime);
    }

    offset   *= (speed      * deltaTime);
    offset.y += (y_velocity * deltaTime);

    movePos(toVec3(offset));
}
