#ifndef FCG_ENEMY
#define FCG_ENEMY

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"

/*
STRUCTS/CLASSES
*/

enum EnemyType
{
    ENEMY_SKELETON,
    ENEMY_MINOTAUR,
    // maybe more. imagine
};

struct Enemy
{
    EnemyType type;

    glm::vec3 pos;
    glm::vec3 view;
    glm::vec3 model_size;
    glm::vec3 hitbox_size;

    float speed;
    float dmgCooldown;

    int health;
    int damage;

    bool grounded;
    float y_velocity;

    float followRange;

    void setEnemyData(EnemyType type);

    AABB getAABB();

    void doEnemyMovement(float deltaTime);
    void doEnemyGravity(float deltaTime);
    void updateView(glm::vec3 player_pos);
    bool isWithinRange(glm::vec3 player_pos);

    void doDamageCooldown(float deltaTime);

    void takeDamage(int dmg);
    bool isDead();
};

#endif // FCG_ENEMY
