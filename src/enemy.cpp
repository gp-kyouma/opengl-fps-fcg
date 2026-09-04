#include "enemy.h"

#include "timer_aux.h"
#include "vec_aux.h"
#include "matrices.h"
#include "gamedata.h"

void Enemy::setEnemyData(std::string type)
{
    *this = g_GameData_Enemies[type];
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
    view_vec = normalize_vec3(view_vec);
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
