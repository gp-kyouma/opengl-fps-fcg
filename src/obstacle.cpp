#include "obstacle.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++

#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "gamedata.h"

void Obstacle::setObstacleData(std::string type)
{
    *this = g_GameData_Obstacles[type];
}

AABB Obstacle::getHitbox()
{
    AABB result;
    glm::vec3 half = e_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

void Obstacle::update(float deltaTime){}//unimplemented... for now.
void Obstacle::apply_kb(float power, glm::vec3 direction){}//unimplemented... for now.
/* for future reference
Obstacle AABBtoObstacle(AABB aabb, ObstacleType type)
{
    Obstacle result;
    result.pos = aabb.getCenter();
    result.e_size = aabb.getSize();
    result.type = type;
    return result;
}
*/
