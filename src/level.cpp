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
    enemy.y_velocity  = 0.0f;

    enemy.pos         = pos;
    enemy.view        = glm::vec3(0.0f,0.0f,1.0f);

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

void Level::createTestLevel()
{
    levelLength = 20.0f;// goes up to 40
    levelWidth  = 20.0f;// goes up to 40
    player_pos  = glm::vec3(0.0f,6.0f,0.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4, ob5, ob6;

    ob1.o_size = glm::vec3(1.0f,1.0f,1.0f);
    ob1.pos    = glm::vec3(3.0f,1.5f,4.0f);
    ob1.type   = OBSTACLE_BOX;

    ob2.o_size = glm::vec3(3.0f,0.5f,3.0f);
    ob2.pos    = glm::vec3(1.0f,3.5f,0.0f);
    ob2.type   = OBSTACLE_PLATFORM;

    ob3.o_size = glm::vec3(1.5f,0.5f,1.5f);
    ob3.pos    = glm::vec3(-0.5f,5.5f,-3.5f);
    ob3.type   = OBSTACLE_PLATFORM;

    ob4.o_size = glm::vec3(1.5f,0.5f,1.5f);
    ob4.pos    = glm::vec3(-1.5f,7.5f,-5.5f);
    ob4.type   = OBSTACLE_PLATFORM;

    ob5.o_size = glm::vec3(1.0f,8.0f,4.0f);
    ob5.pos    = glm::vec3(-2.0f,5.0f,-2.0f);
    ob5.type   = OBSTACLE_WALL;

    ob6.o_size = glm::vec3(1.0f,10.0f,4.0f);
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
}

void Level::createLevel1() //long corridor, sword + shotgun, skeletons drop!
{
    obstacles.clear();
    enemies.clear();

    levelLength = 30.0f;// goes up to 40
    levelWidth  = 10.0f;// goes up to 40
    player_pos  = glm::vec3(0.0f,1.0f,-15.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4;

    ob1.o_size = glm::vec3(5.0f,4.0f,1.0f);
    ob1.pos    = glm::vec3(1.0f,1.5f,-10.0f);
    ob1.type   = OBSTACLE_WALL;
    ob2.o_size = glm::vec3(1.0f,1.0f,1.0f);
    ob2.pos    = glm::vec3(-3.0f,1.5f,6.0f);
    ob2.type   = OBSTACLE_BOX;
    ob3.o_size = glm::vec3(1.0f,1.0f,1.0f);
    ob3.pos    = glm::vec3(3.0f,1.5f,6.0f);
    ob3.type   = OBSTACLE_BOX;
    ob4.o_size = glm::vec3(9.0f,0.5f,5.0f);
    ob4.pos    = glm::vec3(0.0f,4.5f,15.0f);
    ob4.type   = OBSTACLE_PLATFORM;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);
    obstacles.push_back(ob3);
    obstacles.push_back(ob4);

    EnemyData enemy1, enemy2, enemy3, enemy4, enemy5, enemy6;
    enemy1.pos  = glm::vec3(1.0f, 2.0f, -8.0f); //sneaky
    enemy1.type = ENEMY_SKELETON;
    enemy2.pos  = glm::vec3(3.0f, 2.0f, 8.0f);  //escort 1
    enemy2.type = ENEMY_SKELETON;
    enemy3.pos  = glm::vec3(0.0f, 2.0f, 8.0f);  //big guy
    enemy3.type = ENEMY_BIG_SKELETON;
    enemy4.pos  = glm::vec3(-3.0f, 2.0f, 8.0f); //escort 2
    enemy4.type = ENEMY_SKELETON;
    enemy5.pos  = glm::vec3(-3.0f, 5.0f, 15.0f);//platformer 1
    enemy5.type = ENEMY_SKELETON;
    enemy6.pos  = glm::vec3(3.0f, 5.0f, 15.0f); //platformer 2
    enemy6.type = ENEMY_SKELETON;

    enemies.push_back(enemy1);
    enemies.push_back(enemy2);
    enemies.push_back(enemy3);
    enemies.push_back(enemy4);
    enemies.push_back(enemy5);
    enemies.push_back(enemy6);


}

void Level::createLevel2() // surprise room, skelehorde, minigun?
{
    obstacles.clear();
    enemies.clear();

    levelLength = 20.0f;// goes up to 40
    levelWidth  = 20.0f;// goes up to 40
    player_pos  = glm::vec3(15.0f,1.0f,-15.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4, ob5, ob6;

    ob1.o_size = glm::vec3(1.0f,1.0f,2.0f);
    ob1.pos    = glm::vec3(-12.0f,1.5f,6.0f);
    ob1.type   = OBSTACLE_BOX;
    ob2.o_size = glm::vec3(1.0f,1.0f,2.0f);
    ob2.pos    = glm::vec3(3.0f,1.5f,6.0f);
    ob2.type   = OBSTACLE_BOX;
    ob3.o_size = glm::vec3(1.0f,6.0f,2.0f);
    ob3.pos    = glm::vec3(4.0f,1.5f,10.0f);
    ob3.type   = OBSTACLE_WALL;
    ob4.o_size = glm::vec3(1.0f,6.0f,2.0f);
    ob4.pos    = glm::vec3(-4.0f,1.5f,10.0f);
    ob4.type   = OBSTACLE_WALL;
    ob5.o_size = glm::vec3(20.0f,8.0f,2.0f);
    ob5.pos    = glm::vec3(6.0f,1.5f,-6.0f);
    ob5.type   = OBSTACLE_WALL;
    ob6.o_size = glm::vec3(2.0f,8.0f,20.0f);
    ob6.pos    = glm::vec3(-6.0f,1.5f,6.0f);
    ob6.type   = OBSTACLE_WALL;

    obstacles.push_back(ob1);
    obstacles.push_back(ob2);
    obstacles.push_back(ob3);
    obstacles.push_back(ob4);
    obstacles.push_back(ob5);
    obstacles.push_back(ob6);

    EnemyData enemy1, enemy2, enemy3, enemy4, enemy5, enemy6, enemy7, enemy8, enemy9; //horde!! + 1 hidden boneyman
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
    enemy6.type = ENEMY_SKELETON;

    enemies.push_back(enemy1);
    enemies.push_back(enemy2);
    enemies.push_back(enemy3);
    enemies.push_back(enemy4);
    enemies.push_back(enemy5);
    enemies.push_back(enemy6);
    enemies.push_back(enemy7);
    enemies.push_back(enemy8);
    enemies.push_back(enemy9);


}

void Level::createLevel3() //very thin corridor, start at platform, fall into surprise, sniper penetration lesson
{
    obstacles.clear();
    enemies.clear();

    levelLength = 30.0f;// goes up to 40
    levelWidth  = 4.0f;// goes up to 40
    player_pos  = glm::vec3(0.0f,8.0f,-15.0f);
    player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3; // you vill drop from ze platform
    ob1.o_size = glm::vec3(6.0f,0.5f,6.0f);
    ob1.pos    = glm::vec3(0.0f,7.0f,-16.0f);
    ob1.type   = OBSTACLE_PLATFORM;
    ob2.o_size = glm::vec3(6.0f,3.0f,3.0f);
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

}

void Level::createBossLevel()
{
    //IDK
}
