#include "enemy.h"

#include "timer_aux.h"
#include "vec_aux.h"
#include "matrices.h"

void Enemy::setEnemyData(EnemyType type)
{
    this->type = type;

    switch (type)
    {
        case ENEMY_SKELETON:
            model_size  = glm::vec3(1.0f,2.0f,1.0f);
            hitbox_size = glm::vec3(1.0f,2.0f,1.0f);
            speed       = 0.5f;
            health      = 50;
            damage      = 5;
            followRange = 15.0f;
            break;
        case ENEMY_MINOTAUR:
            model_size  = glm::vec3(2.5f,5.0f,1.5f);
            hitbox_size = glm::vec3(2.0f,5.0f,2.0f);
            speed       = 1.0f;
            health      = 500;
            damage      = 15;
            followRange = 40.0f;
            break;
    }
}

AABB Enemy::getAABB()
{
    AABB result;
    glm::vec3 half = hitbox_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

void Enemy::doEnemyMovement(float deltaTime)
{
    pos += view * speed * deltaTime;
}

void Enemy::doEnemyGravity(float deltaTime)
{
    const float gravity = 5.0f;
    if (!grounded)
        y_velocity -= (gravity * deltaTime);

    pos.y += (y_velocity * deltaTime);
}

void Enemy::updateView(glm::vec3 player_pos)
{
    glm::vec3 view_vec = (player_pos - pos);
    view_vec = view_vec / norm(Vetor(view_vec));
    view = view_vec;
}

bool Enemy::isWithinRange(glm::vec3 player_pos)
{
    return (distance(pos, player_pos) <= followRange);
}

void Enemy::doDamageCooldown(float deltaTime)
{
    decrementTimer(dmgCooldown, deltaTime, 0.0f);
}

void Enemy::takeDamage(int dmg)
{
    if (dmgCooldown == 0.0f)
    {
        health -= dmg;
        if (health < 0)
            health = 0;

        dmgCooldown = 0.155f;
    }
}

bool Enemy::isDead()
{
    return (health == 0);
}

