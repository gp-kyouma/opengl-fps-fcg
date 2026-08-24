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
            e_size      = glm::vec3(1.0f,2.0f,1.0f);
            speed       = 1.5f;
            maxHealth   = 50;
            damage      = 10;
            followRange = 20.0f;
            kb_power    = 5.0f;
            kb_resist   = 0.0f;
            break;
        case ENEMY_BIG_SKELETON:
            e_size      = glm::vec3(1.5f,3.0f,1.5f);
            speed       = 1.0f;
            maxHealth   = 250;
            damage      = 20;
            followRange = 30.0f;
            kb_power    = 7.5f;
            kb_resist   = 0.25f;
            break;
        case ENEMY_MINOTAUR:
            e_size      = glm::vec3(2.0f,5.0f,2.0f);
            speed       = 0.75f;
            maxHealth   = 600;
            damage      = 35;
            followRange = 40.0f;
            kb_power    = 9.0f;
            kb_resist   = 0.5f;
            break;
    }
    health = maxHealth;
}

AABB Enemy::getHitbox()
{
    AABB result;
    glm::vec3 half = e_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

void Enemy::update(float deltaTime)
{
    //doEnemyMovement
    if (seesPlayer)
    {
        move_dir += view; // movement does not *SNAP* to view, it *converges* to it instead: this could definitely be improved upon
        move_dir.y = 0;
        float move_norm = norm(Vetor(move_dir));
        if (move_norm != 0)
            move_dir /= move_norm;

        pos += move_dir * speed * deltaTime;
    }

    //doEnemyVelocity

    //gravity
    velocity.y -= (Entity::gravity * deltaTime);

    //knockback
    float v_decay = grounded ? Entity::friction : Entity::air_resist;
    float v_m = 0;
    glm::vec3 v_dir;

    glm::vec3 velocity_h = velocity;
    velocity_h.y = 0.0f;

    vec3_to_attributes(velocity_h, v_dir, v_m);

    v_m -= (v_decay * deltaTime);
    if (v_m < 0.0f)
        v_m = 0.0f;

    velocity_h = v_dir * v_m;

    velocity.x = velocity_h.x;
    velocity.z = velocity_h.z;

    pos += (velocity * deltaTime);

    //doDamageCooldown
    decrementTimer(dmgCooldown, deltaTime, 0.0f);
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

void Enemy::resetHealth()//currently unused
{
    health = maxHealth;
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

void Enemy::apply_kb(float power, glm::vec3 direction)
{
    if (dmgCooldown == 0.0f)
        velocity += direction * (power * (1.0f - kb_resist));
}
