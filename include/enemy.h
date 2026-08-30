#ifndef FCG_ENEMY
#define FCG_ENEMY

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"
#include "entity.h"

/*
STRUCTS/CLASSES
*/

enum EnemyType//TODO turn this into a stringkey
{
    ENEMY_SKELETON,
    ENEMY_BIG_SKELETON,
    ENEMY_MINOTAUR,
};

struct Enemy : Actor
{
    //Entity + Actor attributes omitted

    EnemyType type;

    glm::vec3 move_dir; // direction of movement, converges to view

    int damage;

    bool seesPlayer;
    float followRange;

    void setEnemyData(EnemyType type);

    AABB getHitbox();

    void update(float deltaTime);
    void updateView(glm::vec3 player_pos);
    bool isWithinRange(glm::vec3 player_pos);

    void resetHealth();//currently unused
    void takeDamage(int dmg);
    bool isDead();

    void apply_kb(float power, glm::vec3 direction);

    void draw(); //Defined in draw.cpp
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Enemy, type, dd_key, e_size, speed, maxHealth, damage, followRange, kb_power, kb_resist);

#endif // FCG_ENEMY
