#ifndef FCG_ENTITY
#define FCG_ENTITY

#include <glm/vec4.hpp>

#include "collisions.h"

/*
STRUCTS/CLASSES
*/

struct Entity // All game objects with position and collision, may or may not move or be affected by gravity
{
    glm::vec3 pos;      // world position of entity
    glm::vec3 view;     // vector associated with entity, usually the direction the entity is "looking" at
    glm::vec3 e_size;   // size of entity

    float speed = 0.0f; // speed of entity (if it moves)

    // gravity related attributes
    bool grounded = false;
    float y_velocity = 0.0f;
    static constexpr float gravity = 10.0f;

    virtual void update(float deltaTime) = 0;
    virtual AABB getHitbox() = 0;

    virtual void draw() = 0;
};

struct Actor : Entity // All entities with a health pool
{
    int health;         // current health of actor
    int maxHealth;      // base health of actor
    float dmgCooldown;  // time until actor can take damage again

    virtual void resetHealth() = 0;
    virtual void takeDamage(int dmg) = 0;
    virtual bool isDead() = 0;
};

#endif // FCG_ENTITY
