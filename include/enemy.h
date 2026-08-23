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

enum EnemyType
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

    void update(float deltaTime); // merged 3 functions into 1 lol. they were tiny, it's ok
    void updateView(glm::vec3 player_pos);
    bool isWithinRange(glm::vec3 player_pos);

    void resetHealth();//currently unused
    void takeDamage(int dmg);
    bool isDead();

    void apply_kb(float power, glm::vec3 direction);

    void draw(); //Defined in draw.cpp
};

#endif // FCG_ENEMY
