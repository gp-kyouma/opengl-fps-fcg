#ifndef FCG_OBSTACLE
#define FCG_OBSTACLE

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <collisions.h>

/*
STRUCTS/CLASSES
*/

enum ObstacleType
{
    OBSTACLE_PLATFORM,
    OBSTACLE_BOX,
    OBSTACLE_WALL,
};

struct Obstacle
{
    ObstacleType type;// tipo do obstáculo
    glm::vec3 pos;    // posição do obstáculo no mundo
    glm::vec3 o_size; // tamanho da hitbox do obstáculo

    AABB getAABB();
};

#endif // FCG_OBSTACLE
