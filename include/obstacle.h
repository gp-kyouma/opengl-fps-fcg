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

enum ObstacleType//TODO turn this into a stringkey//will have to MANUALLY replace enum ids with strings in the level jsons once that's implemented. fun.
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

    void setObstacleData(ObstacleType type);

    AABB getHitbox();
    void update(float deltaTime);//unimplemented... for now.

    void apply_kb(float power, glm::vec3 direction);//unimplemented... for now.

    void draw(); //Defined in draw.cpp
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Obstacle, type, dd_key);

#endif // FCG_OBSTACLE
