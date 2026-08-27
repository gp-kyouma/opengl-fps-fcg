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

void Level::createTestLevel()
{
    level_name = "level_test";

    obstacles.clear();
    enemies.clear();

    levelLength = 20.0f;// goes up to 40
    levelWidth  = 20.0f;// goes up to 40
    player_pos  = glm::vec3(0.0f,6.0f,0.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4, ob5, ob6;

    ob1.e_size = glm::vec3(1.0f,1.0f,1.0f);
    ob1.pos    = glm::vec3(3.0f,1.5f,4.0f);
    ob1.type   = OBSTACLE_BOX;

    ob2.e_size = glm::vec3(3.0f,0.5f,3.0f);
    ob2.pos    = glm::vec3(1.0f,3.5f,0.0f);
    ob2.type   = OBSTACLE_PLATFORM;

    ob3.e_size = glm::vec3(1.5f,0.5f,1.5f);
    ob3.pos    = glm::vec3(-0.5f,5.5f,-3.5f);
    ob3.type   = OBSTACLE_PLATFORM;

    ob4.e_size = glm::vec3(1.5f,0.5f,1.5f);
    ob4.pos    = glm::vec3(-1.5f,7.5f,-5.5f);
    ob4.type   = OBSTACLE_PLATFORM;

    ob5.e_size = glm::vec3(1.0f,8.0f,4.0f);
    ob5.pos    = glm::vec3(-2.0f,5.0f,-2.0f);
    ob5.type   = OBSTACLE_WALL;

    ob6.e_size = glm::vec3(1.0f,10.0f,4.0f);
    ob6.pos    = glm::vec3(-5.0f,6.0f,-2.0f);
    ob6.type   = OBSTACLE_WALL;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);
    obstacles.push_back(ob3);
    obstacles.push_back(ob4);
    obstacles.push_back(ob5);
    obstacles.push_back(ob6);

    EnemyData test_enemy1;
    test_enemy1.pos  = glm::vec3(6.0f, 6.0f, 6.0f);
    test_enemy1.type = ENEMY_SKELETON;

    EnemyData test_enemy2;
    test_enemy2.pos  = glm::vec3(-6.0f, 6.0f, 6.0f);
    test_enemy2.type = ENEMY_BIG_SKELETON;

    enemies.push_back(test_enemy1);
    enemies.push_back(test_enemy2);

    setLevelWalls();
}

void Level::createLevel1()
{
    level_name = "level_1";

    obstacles.clear();
    enemies.clear();

    levelLength = 30.0f;
    levelWidth  = 10.0f;
    player_pos  = glm::vec3(0.0f,1.0f,-15.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4;

    ob1.e_size = glm::vec3(5.0f,4.0f,1.0f);
    ob1.pos    = glm::vec3(1.0f,1.5f,-10.0f);
    ob1.type   = OBSTACLE_WALL;
    ob2.e_size = glm::vec3(1.0f,1.0f,1.0f);
    ob2.pos    = glm::vec3(-3.0f,1.5f,6.0f);
    ob2.type   = OBSTACLE_BOX;
    ob3.e_size = glm::vec3(1.0f,1.0f,1.0f);
    ob3.pos    = glm::vec3(3.0f,1.5f,6.0f);
    ob3.type   = OBSTACLE_BOX;
    ob4.e_size = glm::vec3(9.0f,0.5f,5.0f);
    ob4.pos    = glm::vec3(0.0f,4.5f,15.0f);
    ob4.type   = OBSTACLE_PLATFORM;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);
    obstacles.push_back(ob3);
    obstacles.push_back(ob4);

    Obstacle obDice;

    obDice.e_size = glm::vec3(1.0f,1.0f,1.0f);
    obDice.pos    = glm::vec3(2.0f,1.5f,-12.0f);
    obDice.type   = OBSTACLE_DICE;

    obstacles.push_back(obDice);

    AABB boxbox;
    boxbox.aabb_min = glm::vec3(-3.5f,1.0f,-12.5f);
    boxbox.aabb_max = glm::vec3(-3.0f,3.0f,-11.0f);

    obstacles.push_back(AABBtoObstacle(boxbox, OBSTACLE_WALL));

    for (int i = 0; i < 6; i++)
    {
        Obstacle step;
        step.e_size = glm::vec3(1.0f,1.0f,0.5f);
        step.pos = glm::vec3(-1.0f, 0.75f + (i * 0.25f), -13.25f + (i * 0.5f));
        step.type = OBSTACLE_PLATFORM;
        obstacles.push_back(step);
    }

    EnemyData enemy1, enemy2, enemy3, enemy4, enemy5, enemy6;
    enemy1.pos  = glm::vec3(1.0f, 2.0f, -8.0f);
    enemy1.type = ENEMY_SKELETON;
    enemy2.pos  = glm::vec3(3.0f, 2.0f, 8.0f);
    enemy2.type = ENEMY_SKELETON;
    enemy3.pos  = glm::vec3(0.0f, 2.0f, 8.0f);
    enemy3.type = ENEMY_BIG_SKELETON;
    enemy4.pos  = glm::vec3(-3.0f, 2.0f, 8.0f);
    enemy4.type = ENEMY_SKELETON;
    enemy5.pos  = glm::vec3(-3.0f, 5.0f, 15.0f);
    enemy5.type = ENEMY_SKELETON;
    enemy6.pos  = glm::vec3(3.0f, 5.0f, 15.0f);
    enemy6.type = ENEMY_SKELETON;

    //cage for big skeleton
    Obstacle cage1, cage2, cage3, cage4;
    cage1.type = cage2.type = cage3.type = cage4.type = OBSTACLE_WALL;

    cage1.e_size = cage2.e_size = glm::vec3(2.5f,  0.25f, 0.25f);
    cage3.e_size = cage4.e_size = glm::vec3(0.25f, 0.25f, 2.5f);

    cage1.pos    = glm::vec3(0.0f, 1.125f, 6.875f);
    cage2.pos    = glm::vec3(0.0f, 1.125f, 9.125f);
    cage3.pos    = glm::vec3(1.125f, 1.375f, 8.0f);
    cage4.pos    = glm::vec3(-1.125f, 1.375f, 8.0f);

    obstacles.push_back(cage1);
    obstacles.push_back(cage2);
    obstacles.push_back(cage3);
    obstacles.push_back(cage4);

    enemies.push_back(enemy1);
    enemies.push_back(enemy2);
    enemies.push_back(enemy3);
    enemies.push_back(enemy4);
    enemies.push_back(enemy5);
    enemies.push_back(enemy6);

    setLevelWalls();
}

void Level::createLevel2()
{
    level_name = "level_2";

    obstacles.clear();
    enemies.clear();

    levelLength = 20.0f;
    levelWidth  = 20.0f;
    player_pos  = glm::vec3(15.0f,1.0f,-15.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4, ob5, ob6;

    ob1.e_size = glm::vec3(1.0f,1.0f,2.0f);
    ob1.pos    = glm::vec3(-12.0f,1.5f,6.0f);
    ob1.type   = OBSTACLE_BOX;
    ob2.e_size = glm::vec3(1.0f,1.0f,2.0f);
    ob2.pos    = glm::vec3(3.0f,1.5f,6.0f);
    ob2.type   = OBSTACLE_BOX;
    ob3.e_size = glm::vec3(1.0f,6.0f,2.0f);
    ob3.pos    = glm::vec3(4.0f,1.5f,10.0f);
    ob3.type   = OBSTACLE_WALL;
    ob4.e_size = glm::vec3(1.0f,6.0f,2.0f);
    ob4.pos    = glm::vec3(-4.0f,1.5f,10.0f);
    ob4.type   = OBSTACLE_WALL;
    ob5.e_size = glm::vec3(20.0f,8.0f,2.0f);
    ob5.pos    = glm::vec3(6.0f,1.5f,-6.0f);
    ob5.type   = OBSTACLE_WALL;
    ob6.e_size = glm::vec3(2.0f,8.0f,20.0f);
    ob6.pos    = glm::vec3(-6.0f,1.5f,6.0f);
    ob6.type   = OBSTACLE_WALL;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);
    obstacles.push_back(ob3);
    obstacles.push_back(ob4);
    obstacles.push_back(ob5);
    obstacles.push_back(ob6);

    EnemyData enemy1, enemy2, enemy3, enemy4, enemy5, enemy6, enemy7, enemy8, enemy9;
    enemy1.pos  = glm::vec3(8.0f, 2.0f, 6.0f);
    enemy1.type = ENEMY_SKELETON;
    enemy2.pos  = glm::vec3(-8.0f, 2.0f, 6.0f);
    enemy2.type = ENEMY_SKELETON;
    enemy3.pos  = glm::vec3(4.0f, 2.0f, 8.0f);
    enemy3.type = ENEMY_SKELETON;
    enemy4.pos  = glm::vec3(-4.0f, 2.0f, 8.0f);
    enemy4.type = ENEMY_SKELETON;
    enemy5.pos  = glm::vec3(2.0f, 2.0f, 10.0f);
    enemy5.type = ENEMY_SKELETON;
    enemy6.pos  = glm::vec3(-2.0f, 2.0f, 10.0f);
    enemy6.type = ENEMY_SKELETON;
    enemy7.pos  = glm::vec3(3.0f, 2.0f, 9.0f);
    enemy7.type = ENEMY_SKELETON;
    enemy8.pos  = glm::vec3(-3.0f, 2.0f, 9.0f);
    enemy8.type = ENEMY_SKELETON;
    enemy9.pos  = glm::vec3(0.0f, 2.0f, 9.0f);
    enemy9.type = ENEMY_SKELETON;

    enemies.push_back(enemy1);
    enemies.push_back(enemy2);
    enemies.push_back(enemy3);
    enemies.push_back(enemy4);
    enemies.push_back(enemy5);
    enemies.push_back(enemy6);
    enemies.push_back(enemy7);
    enemies.push_back(enemy8);
    enemies.push_back(enemy9);

    setLevelWalls();
}

void Level::createLevel3()
{
    level_name = "level_3";

    obstacles.clear();
    enemies.clear();

    levelLength = 30.0f;
    levelWidth  = 4.0f;
    player_pos  = glm::vec3(0.0f,8.0f,-15.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2;
    ob1.e_size = glm::vec3(6.0f,0.5f,6.0f);
    ob1.pos    = glm::vec3(0.0f,7.0f,-16.0f);
    ob1.type   = OBSTACLE_PLATFORM;
    ob2.e_size = glm::vec3(6.0f,3.0f,3.0f);
    ob2.pos    = glm::vec3(0.0f,8.0f,-10.0f);
    ob2.type   = OBSTACLE_WALL;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);


    EnemyData enemy1, enemy2, enemy3, enemy4, enemy5, enemy6, enemy7, enemy8;
    enemy1.pos  = glm::vec3(1.0f, 2.0f, 6.0f);
    enemy1.type = ENEMY_SKELETON;
    enemy2.pos  = glm::vec3(-1.0f, 2.0f, 6.0f);
    enemy2.type = ENEMY_SKELETON;
    enemy3.pos  = glm::vec3(2.0f, 2.0f, 8.0f);
    enemy3.type = ENEMY_BIG_SKELETON;
    enemy4.pos  = glm::vec3(-2.0f, 2.0f, 8.0f);
    enemy4.type = ENEMY_BIG_SKELETON;
    enemy5.pos  = glm::vec3(1.0f, 2.0f, 10.0f);
    enemy5.type = ENEMY_SKELETON;
    enemy6.pos  = glm::vec3(-1.0f, 2.0f, 10.0f);
    enemy6.type = ENEMY_SKELETON;
    enemy7.pos  = glm::vec3(3.0f, 2.0f, 12.0f);
    enemy7.type = ENEMY_BIG_SKELETON;
    enemy8.pos  = glm::vec3(-3.0f, 2.0f, 12.0f);
    enemy8.type = ENEMY_BIG_SKELETON;

    enemies.push_back(enemy1);
    enemies.push_back(enemy2);
    enemies.push_back(enemy3);
    enemies.push_back(enemy4);
    enemies.push_back(enemy5);
    enemies.push_back(enemy6);
    enemies.push_back(enemy7);
    enemies.push_back(enemy8);

    setLevelWalls();
}

void Level::createBossLevel()
{
    level_name = "level_boss";

    obstacles.clear();
    enemies.clear();

    levelLength = 26.0f;
    levelWidth  = 26.0f;
    player_pos  = glm::vec3(0.0f,2.0f,-10.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4, ob5, ob6, ob7, ob8;

    // platforms
    ob1.e_size = glm::vec3(26.0f,0.5f,3.0f);
    ob1.pos    = glm::vec3(0.0f,5.75f,-11.5f);
    ob1.type   = OBSTACLE_PLATFORM;
    ob2.e_size = glm::vec3(26.0f,0.5f,3.0f);
    ob2.pos    = glm::vec3(0.0f,5.75f,11.5f);
    ob2.type   = OBSTACLE_PLATFORM;
    ob3.e_size = glm::vec3(3.0f,0.5f,20.0f);
    ob3.pos    = glm::vec3(-11.5f,5.75f,0.0f);
    ob3.type   = OBSTACLE_PLATFORM;
    ob4.e_size = glm::vec3(3.0f,0.5f,20.0f);
    ob4.pos    = glm::vec3(11.5f,5.75f,0.0f);
    ob4.type   = OBSTACLE_PLATFORM;

    // "pillars"
    ob5.e_size = glm::vec3(1.0f,10.0f,1.0f);
    ob5.pos    = glm::vec3(7.0f,6.0f,7.0f);
    ob5.type   = OBSTACLE_WALL;
    ob6.e_size = glm::vec3(1.0f,10.0f,1.0f);
    ob6.pos    = glm::vec3(7.0f,6.0f,-7.0f);
    ob6.type   = OBSTACLE_WALL;
    ob7.e_size = glm::vec3(1.0f,10.0f,1.0f);
    ob7.pos    = glm::vec3(-7.0f,6.0f,7.0f);
    ob7.type   = OBSTACLE_WALL;
    ob8.e_size = glm::vec3(1.0f,10.0f,1.0f);
    ob8.pos    = glm::vec3(-7.0f,6.0f,-7.0f);
    ob8.type   = OBSTACLE_WALL;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);
    obstacles.push_back(ob3);
    obstacles.push_back(ob4);
    obstacles.push_back(ob5);
    obstacles.push_back(ob6);
    obstacles.push_back(ob7);
    obstacles.push_back(ob8);

    EnemyData enemy1, enemy2, enemy3;
    enemy1.pos  = glm::vec3(0.0f, 8.5f, 11.0f);
    enemy1.type = ENEMY_MINOTAUR;
    enemy2.pos  = glm::vec3(-10.0f, 7.5f, 6.0f);
    enemy2.type = ENEMY_BIG_SKELETON;
    enemy3.pos  = glm::vec3( 10.0f, 7.5f, 6.0f);
    enemy3.type = ENEMY_BIG_SKELETON;

    enemies.push_back(enemy1);
    enemies.push_back(enemy2);
    enemies.push_back(enemy3);

    setLevelWalls();
}

//for export reasons
void exportLevelToFile(Level level)
{
    json j = level;
    std::cout << j.dump() << std::endl << std::endl;
}
