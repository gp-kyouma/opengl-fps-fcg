#ifndef FCG_OBSTACLE
#define FCG_OBSTACLE

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
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
    ObstacleType type;// tipo do obst�culo
    glm::vec3 pos;    // posi��o do obst�culo no mundo
    glm::vec3 o_size; // tamanho da hitbox do obst�culo

    AABB getAABB();
};

#endif // FCG_OBSTACLE
