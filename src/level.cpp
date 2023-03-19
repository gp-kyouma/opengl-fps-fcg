#include "level.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo s�o espec�ficos de C++

#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Cria��o de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Cria��o de janelas do sistema operacional

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

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
}
