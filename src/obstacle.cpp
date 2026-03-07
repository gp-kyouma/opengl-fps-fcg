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

AABB Obstacle::getHitbox()
{
    AABB result;
    glm::vec3 half = e_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

void Obstacle::update(float deltaTime){}//unimplemented... for now.
