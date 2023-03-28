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
            speed       = 1.5f;     // 0.5 --> 1 rattle me bones
            health      = 50;
            damage      = 10;       // 5 --> 10 it's not meant to be dark souls but still, too easy
            followRange = 20.0f;    // 10 --> 20 skellingtons too blind for melee
            break;
        case ENEMY_BIG_SKELETON:
            model_size  = glm::vec3(1.5f,3.0f,1.5f);
            hitbox_size = glm::vec3(1.5f,3.0f,1.5f);
            speed       = 1.0f;     // 0.35 --> 0.75 now not quite so slow
            health      = 250;       // very tanky, but maybe that's good
            damage      = 20;       // 10 --> 20 upgrade
            followRange = 30.0f;    // 20 --> naturally gotta be buffed as well
            break;
        case ENEMY_MINOTAUR:
            model_size  = glm::vec3(2.5f,5.0f,1.5f);
            hitbox_size = glm::vec3(2.0f,5.0f,2.0f);
            speed       = 0.8f;
            health      = 600;
            damage      = 45;       // 15 -->45 upgrade
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

