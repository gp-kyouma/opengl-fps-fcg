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

Enemy EnemyData::buildEnemy()
{
    Enemy enemy;

    enemy.setEnemyData(type);

    enemy.dmgCooldown = 0.0f;

    enemy.grounded    = false;

    enemy.seesPlayer  = false;

    enemy.pos         = pos;
    enemy.view        = view;
    enemy.velocity    = glm::vec3(0.0f,0.0f,0.0f);
    enemy.move_dir    = enemy.view;

    return enemy;
}

bool EnemyData::checkValid()
{
    //check if pos is valid (not inf or nan)
    if (!vec3_finite(pos))
        return false;

    //view valid
    if (!vec3_finite(view))
        return false;

    if (glm::length(view) == 0.0f)
        view = glm::vec3(0.0f,0.0f,1.0f);//silently fix shhhh

    return true;
}

void EnemyData::setValidDefaults()
{
    //check if pos is valid (not inf or nan)
    if (!vec3_finite(pos))
        pos = glm::vec3(0.0f,0.0f,0.0f);

    //view valid
    if (!vec3_finite(view))
        view = glm::vec3(0.0f,0.0f,1.0f);
    else if (glm::length(view) == 0.0f)
        view = glm::vec3(0.0f,0.0f,1.0f);
    else
        view = normalize_vec3(view);
}

Obstacle ObstacleData::buildObstacle()
{
    Obstacle ob;

    ob.setObstacleData(type);

    ob.pos    = pos;
    ob.e_size = e_size;

    return ob;
}

bool ObstacleData::checkValid()
{
    //check if pos is valid (not inf or nan)
    if (!vec3_finite(pos))
        return false;

    //check if size is valid (not inf or nan)
    if (!vec3_finite(e_size))
        return false;

    //check if size is valid (not degenerate)
    if (e_size.x <= 0 || e_size.y <= 0 || e_size.z <= 0)
        return false;

    return true;
}

void ObstacleData::setValidDefaults()
{
    //check if pos is valid (not inf or nan)
    if (!vec3_finite(pos))
        pos = glm::vec3(0.0f,0.0f,0.0f);

    //check if size is valid (not inf or nan)
    if (!vec3_finite(e_size))
        e_size = glm::vec3(1.0f,1.0f,1.0f);

    //check if size is valid (not degenerate)
    if (e_size.x <= 0 || e_size.y <= 0 || e_size.z <= 0)
        e_size = glm::vec3(1.0f,1.0f,1.0f);
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

glm::vec3 Level::getLevelCenter()
{
    float midheight = (levelCeiling + levelFloor) / 2.0f;
    return glm::vec3(0.0f,midheight,0.0f);
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

bool Level::checkValidBoundaries()
{
    //check for nan or inf
    if (!std::isfinite(levelWidth) || !std::isfinite(levelLength) || !std::isfinite(levelFloor) || !std::isfinite(levelCeiling))
        return false;

    //check for degenerate
    glm::vec3 levelsize = getAABB().getSize();

    if (levelsize.x <= 0 || levelsize.y <= 0 || levelsize.z <= 0)
        return false;

    return true;
}

std::vector<std::string> Level::checkValid()
{
    std::vector<std::string> ret;
    ret.clear();

    if (level_name.empty())
        ret.push_back("Empty level name");

    if (!vec3_finite(player_pos))
        ret.push_back("Invalid player starting position");

    if (!vec3_finite(player_view))
        ret.push_back("Invalid player starting view dir");

    if (glm::length(player_view) == 0.0f)
        ret.push_back("Null player starting view dir");

    if (!checkValidBoundaries())
        ret.push_back("Invalid level boundaries");

    for (int i = 0; i < (int)obstacles.size(); i++)
        if (!obstacles[i].checkValid())
            ret.push_back("Obstacle #" + std::to_string(i) + " is invalid");

    for (int i = 0; i < (int)enemies.size(); i++)
        if (!enemies[i].checkValid())
            ret.push_back("Enemy #" + std::to_string(i) + " is invalid");

    return ret;
}

void Level::setValidDefaults()
{
    if (level_name.empty())
        level_name = "level";

    if (!vec3_finite(player_pos))
        player_pos = glm::vec3(0.0f,0.0f,0.0f);

    if (!vec3_finite(player_view))
        player_view = glm::vec3(0.0f,0.0f,1.0f);
    else if (glm::length(player_view) == 0.0f)
        player_view = glm::vec3(0.0f,0.0f,1.0f);
    else
        player_view = normalize_vec3(player_view);

    if (!checkValidBoundaries())
    {
        levelWidth    = 10.0f;
        levelLength   = 10.0f;
        levelFloor    = 0.0f;
        levelCeiling  = 10.0f;
    }

    for (int i = 0; i < (int)obstacles.size(); i++)
        obstacles[i].setValidDefaults();

    for (int i = 0; i < (int)enemies.size(); i++)
        enemies[i].setValidDefaults();
}
