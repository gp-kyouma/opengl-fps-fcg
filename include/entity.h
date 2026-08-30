#ifndef FCG_ENTITY
#define FCG_ENTITY

#include <string>

#include <glm/vec4.hpp>

#include "collisions.h"

//JSON
#include "vec_aux.h"//for vec3 json
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

/*
STRUCTS/CLASSES
*/

struct Entity // All game objects with position and collision, may or may not move or be affected by gravity
{
    std::string type;

    glm::vec3 pos;      // world position of entity
    glm::vec3 view;     // vector associated with entity, usually the direction the entity is "looking" at
    glm::vec3 e_size;   // size of entity

    float speed = 0.0f; // speed of entity (if it moves)

    // current velocity of entity (gravity, knockback)
    glm::vec3 velocity;

    // gravity related attributes
    bool grounded = false;
    static constexpr float gravity = 10.0f;

    // knockback related attributes
    float kb_power  = 1.0f;
    float kb_resist = 0.0f; //percentage
    static constexpr float air_resist = 3.0f;
    static constexpr float friction = 10.0f;

    // draw data key
    std::string dd_key;

    virtual void apply_kb(float power, glm::vec3 direction) = 0;

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
