#include "draw.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrices.h"


void drawAABB(AABB aabb) // para razões de debug
{
    glm::vec3 aabb_center = aabb.getCenter();
    glm::vec3 aabb_size   = aabb.getSize();

    glm::mat4 model = Matrix_Translate(aabb_center.x, aabb_center.y, aabb_center.z) *
                      Matrix_Scale(aabb_size.x, aabb_size.y, aabb_size.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    setDiffuseTexture("white");
    setSpecularTexture("white");
    glLineWidth(4.0f);
    DrawVirtualObject("cube_edges");
}

void drawFloor(Level level)
{
    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);

    glm::mat4 model = Matrix_Translate(0.0f,level.levelFloor,0.0f) *
                      Matrix_Scale(halfWidth, 1.0f, halfLength);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    setTextureRepeat(halfWidth,halfLength);
    setDiffuseTexture("floor");
    setSpecularTexture("black");
    DrawVirtualObject("the_plane");
    resetTextureRepeat();
}

void drawWall(Level level, CardinalDirection direction)
{
    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);
    float halfHeight = ((level.levelCeiling - level.levelFloor)/2.0f);
    const float pi2  = 3.141592f/2;

    glm::mat4 model;

    switch (direction)
    {
        case NORTH:
            model = Matrix_Translate(0.0f, level.levelFloor + halfHeight, halfLength) *
                    Matrix_Rotate_X(-pi2) * // -90graus
                    Matrix_Scale(halfWidth, 1.0f, halfHeight) *
                    Matrix_Rotate_Y(pi2*2); // 180graus
            setTextureRepeat(halfWidth,halfHeight);
            break;
        case SOUTH:
            model = Matrix_Translate(0.0f, level.levelFloor + halfHeight, -halfLength) *
                    Matrix_Rotate_X(pi2) *  // 90graus
                    Matrix_Scale(halfWidth, 1.0f, halfHeight);
            setTextureRepeat(halfWidth,halfHeight);
            break;
        case EAST:
            model = Matrix_Translate(-halfWidth, level.levelFloor + halfHeight, 0.0f) *
                    Matrix_Rotate_Z(-pi2) * // -90graus
                    Matrix_Scale(halfHeight, 1.0f, halfLength) *
                    Matrix_Rotate_Y(pi2);   // 90graus
            setTextureRepeat(halfHeight,halfLength);
            break;
        case WEST:
            model = Matrix_Translate(halfWidth, level.levelFloor + halfHeight, 0.0f) *
                    Matrix_Rotate_Z(pi2) *  // 90graus
                    Matrix_Scale(halfHeight, 1.0f, halfLength) *
                    Matrix_Rotate_Y(-pi2);  // -90graus
            setTextureRepeat(halfHeight,halfLength);
            break;
        default: break;
    }

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    setDiffuseTexture("wall");
    setSpecularTexture("black");
    DrawVirtualObject("the_plane");
    resetTextureRepeat();
}

void drawObstacle(Obstacle obstacle)
{
    float width  = (obstacle.o_size.x);
    float length = (obstacle.o_size.z);
    float height = (obstacle.o_size.y);

    glm::mat4 model;
    model = Matrix_Translate(obstacle.pos.x, obstacle.pos.y, obstacle.pos.z) *
            Matrix_Scale(width, height, length);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    switch (obstacle.type)
    {
        case OBSTACLE_PLATFORM:
            setTextureRepeat(obstacle.o_size.x,obstacle.o_size.z);
            setDiffuseTexture("floor");
            setSpecularTexture("black");
            break;
        case OBSTACLE_WALL:
            if (width > length)
                setTextureRepeat(width,height);
            else
                setTextureRepeat(length,height);
            setDiffuseTexture("wall");
            setSpecularTexture("black");
            break;
        case OBSTACLE_BOX:
            resetTextureRepeat();
            setDiffuseTexture("box");
            setSpecularTexture("black");
            break;
        default: break;
    }

    DrawVirtualObject("cube");
    resetTextureRepeat();
}
