#ifndef FCG_OBSTACLE
#define FCG_OBSTACLE

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"
#include "entity.h"

/*
STRUCTS/CLASSES
*/

enum ObstacleType
{
    OBSTACLE_PLATFORM,
    OBSTACLE_BOX,
    OBSTACLE_WALL,
    OBSTACLE_DICE,
    OBSTACLE_INVISIBLE_WALL
};

struct Obstacle : Entity
{
    //Entity attributes omitted

    ObstacleType type;// tipo do obstáculo

    AABB getHitbox();
    void update(float deltaTime);//unimplemented... for now.

    void apply_kb(float power, glm::vec3 direction);//unimplemented... for now.

    void draw(); //Defined in draw.cpp
};

Obstacle AABBtoObstacle(AABB aabb, ObstacleType type);

#endif // FCG_OBSTACLE
