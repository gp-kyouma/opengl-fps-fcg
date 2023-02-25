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
    glm::vec3 aabb_center = getAABBCenter(aabb);
    glm::vec3 aabb_size   = getAABBSize(aabb);

    glm::mat4 model = Matrix_Translate(aabb_center.x, aabb_center.y, aabb_center.z) *
                      Matrix_Scale(aabb_size.x, aabb_size.y, aabb_size.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_render_as_white_uniform, true);
    glLineWidth(4.0f);
    DrawVirtualObject("cube_edges");
    glUniform1i(g_render_as_white_uniform, false);
}

void drawFloor(Level level)
{
    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);

    glm::mat4 model = Matrix_Translate(0.0f,level.levelFloor,0.0f) *
                      Matrix_Scale(halfWidth, 1.0f, halfLength);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    setTexture("floor");
    DrawVirtualObject("the_plane");
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
            break;
        case SOUTH:
            model = Matrix_Translate(0.0f, level.levelFloor + halfHeight, -halfLength) *
                    Matrix_Rotate_X(pi2) *  // 90graus
                    Matrix_Scale(halfWidth, 1.0f, halfHeight);
            break;
        case EAST:
            model = Matrix_Translate(-halfWidth, level.levelFloor + halfHeight, 0.0f) *
                    Matrix_Rotate_Z(-pi2) * // -90graus
                    Matrix_Scale(halfHeight, 1.0f, halfLength) *
                    Matrix_Rotate_Y(pi2);   // 90graus
            break;
        case WEST:
            model = Matrix_Translate(halfWidth, level.levelFloor + halfHeight, 0.0f) *
                    Matrix_Rotate_Z(pi2) *  // 90graus
                    Matrix_Scale(halfHeight, 1.0f, halfLength) *
                    Matrix_Rotate_Y(-pi2);  // -90graus
            break;
        default: break;
    }

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    setTexture("wall");
    DrawVirtualObject("the_plane");
}
