#include "level.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++

#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

Enemy EnemyData::buildEnemy()
{
    Enemy enemy;

    enemy.setEnemyData(type);

    enemy.dmgCooldown = 0.0f;

    enemy.grounded    = false;

    enemy.seesPlayer  = false;

    enemy.pos         = pos;
    enemy.view        = glm::vec3(0.0f,0.0f,1.0f);
    enemy.velocity    = glm::vec3(0.0f,0.0f,0.0f);
    enemy.move_dir    = enemy.view;

    return enemy;
}

AABB Level::getAABB()
{
    AABB result;
    float halfWidth  = (levelWidth /2.0f);
    float halfLength = (levelLength/2.0f);

    result.aabb_max = glm::vec3(halfWidth,  levelCeiling, halfLength);
    result.aabb_min = glm::vec3(-halfWidth, levelFloor,  -halfLength);

    return result;
}

void Level::setLevelWalls()
{
    AABB floor, ceiling, positiveX, negativeX, positiveZ, negativeZ;

    float epsilon    = 0.125f;
    float thickness  = 0.5f + epsilon;
    float halfWidth  = (levelWidth /2.0f) + epsilon;
    float halfLength = (levelLength/2.0f) + epsilon;

    floor.aabb_max = glm::vec3(halfWidth,  levelFloor-epsilon, halfLength);
    floor.aabb_min = glm::vec3(-halfWidth, levelFloor-thickness,  -halfLength);

    ceiling.aabb_max = glm::vec3(halfWidth,  levelCeiling+thickness, halfLength);
    ceiling.aabb_min = glm::vec3(-halfWidth, levelCeiling+epsilon,  -halfLength);

    positiveX.aabb_max = glm::vec3(halfWidth+thickness,  levelCeiling, halfLength);
    positiveX.aabb_min = glm::vec3(halfWidth, levelFloor,  -halfLength);

    negativeX.aabb_max = glm::vec3(-halfWidth,  levelCeiling, halfLength);
    negativeX.aabb_min = glm::vec3(-halfWidth-thickness, levelFloor,  -halfLength);

    positiveZ.aabb_max = glm::vec3(halfWidth,  levelCeiling, halfLength+thickness);
    positiveZ.aabb_min = glm::vec3(-halfWidth, levelFloor,   halfLength);

    negativeZ.aabb_max = glm::vec3(halfWidth,  levelCeiling, -halfLength);
    negativeZ.aabb_min = glm::vec3(-halfWidth, levelFloor,   -halfLength-thickness);

    levelWalls[0] = floor;
    levelWalls[1] = ceiling;
    levelWalls[2] = positiveX;
    levelWalls[3] = negativeX;
    levelWalls[4] = positiveZ;
    levelWalls[5] = negativeZ;
}

//for export reasons
void exportLevelToFile(Level level)
{
    json j = level;
    std::cout << j.dump() << std::endl << std::endl;
}
