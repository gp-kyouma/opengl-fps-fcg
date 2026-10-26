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
    // if type doesn't exist in gamedata, set first type as default
    if (g_GameData_Obstacles.count(type) == 0)
        type = g_GameData_Obstacles.begin()->first;

    *this = g_GameData_Obstacles[type];
}

AABB Obstacle::getHitbox()
{
    return AABB::from_pos_size(pos, e_size);
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
