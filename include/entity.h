#ifndef FCG_ENTITY
#define FCG_ENTITY

#include <glm/vec4.hpp>

#include "collisions.h"

/*
STRUCTS/CLASSES
*/

struct Entity // All game objects with position and collision, may or may not move or be affected by gravity
{
    glm::vec3 pos;
    glm::vec3 view;
    glm::vec3 e_size;

    float speed;

    bool grounded;
    float y_velocity;

    virtual void update(float deltaTime) = 0;
    virtual AABB getAABB() = 0;

    virtual void draw() = 0;//Hmm.
};

struct Actor : Entity // All entities with a health pool
{
    int health;
    int maxHealth;
    float dmgCooldown;

    virtual void resetHealth() = 0;
    virtual void takeDamage(int dmg) = 0;
    virtual bool isDead() = 0;
};

#endif // FCG_ENTITY
