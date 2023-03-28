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
#include "vec_aux.h"

void drawAABB(AABB aabb) // para razões de debug
{
    glm::vec3 aabb_center = aabb.getCenter();
    glm::vec3 aabb_size   = aabb.getSize();

    glm::mat4 model = Matrix_Translate(aabb_center.x, aabb_center.y, aabb_center.z) *
                      Matrix_Scale(aabb_size.x, aabb_size.y, aabb_size.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseTexture("white");
    setSpecularTexture("black");
    glLineWidth(4.0f);
    DrawVirtualObject("cube_edges");
    glUniform1i(g_ignore_lighting_uniform, false);
}

void drawCrosshair(float aspect)
{
    const float crosshair_size = 0.0625f; // 1/16

    glm::mat4 model = Matrix_Scale(crosshair_size / aspect, crosshair_size, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseTexture("white");
    setSpecularTexture("black");
    glLineWidth(2.0f);
    DrawVirtualObject("crosshair");
    glUniform1i(g_ignore_lighting_uniform, false);
}

// Desenha uma progress bar
// (usado para barra de HP e barra de cooldown)
// tex1 é usada quando       value/maxvalue > 2/3
// tex2 é usada quando 2/3 > value/maxvalue > 1/3
// tex3 é usada quando 1/3 > value/maxvalue
// position começa de 0, cada incremento aumenta a posição vertical da barra
void drawBar(float value, float maxValue, float aspect, std::string tex1, std::string tex2, std::string tex3, int position)
{
    glm::vec2 bgSize = glm::vec2(0.25f,0.05f);
    float barEdge    = 0.015f;

    bgSize.x /= aspect;

    glm::vec2 translate = glm::vec2(1.0f - (bgSize.x), -1.0f + (bgSize.y)*(position*2+1));

    // primeiro desenha o fundo
    glm::mat4 model = Matrix_Translate(translate.x, translate.y, 0.0f) *
                      Matrix_Scale(bgSize.x, bgSize.y, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseTexture("grey");
    setSpecularTexture("black");
    DrawVirtualObject("square");

    // depois desenha a barra
    float barRatio = value / maxValue;
    glm::vec2 barSize = glm::vec2(bgSize.x - barEdge, bgSize.y - barEdge);

    model = Matrix_Translate(translate.x, translate.y, 0.0f)              *
            Matrix_Translate((1.0f - (barRatio)) * barSize.x, 0.0f, 0.0f) *
            Matrix_Scale(barRatio, 1.0f, 1.0f)                            *
            Matrix_Scale(barSize.x, barSize.y, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    if (barRatio >= 0.66f)
        setDiffuseTexture(tex1);
    else if (barRatio < 0.66f && barRatio > 0.33f)
        setDiffuseTexture(tex2);
    else
        setDiffuseTexture(tex3);

    setSpecularTexture("black");
    DrawVirtualObject("square");
    glUniform1i(g_ignore_lighting_uniform, false);
}

void drawBanner(float aspect, std::string tex) // used for game over, you won, etc...
{
    glm::vec2 bannerSize = glm::vec2(0.8f,0.6f); // 3:4

    glm::mat4 model = Matrix_Scale(bannerSize.x / aspect, bannerSize.y, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseTexture(tex);
    setSpecularTexture("black");
    DrawVirtualObject("square");
    glUniform1i(g_ignore_lighting_uniform, false);
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
    setSpecularTexture("grey");
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
            setTextureRepeat(halfLength,halfHeight);
            break;
        case WEST:
            model = Matrix_Translate(halfWidth, level.levelFloor + halfHeight, 0.0f) *
                    Matrix_Rotate_Z(pi2) *  // 90graus
                    Matrix_Scale(halfHeight, 1.0f, halfLength) *
                    Matrix_Rotate_Y(-pi2);  // -90graus
            setTextureRepeat(halfLength,halfHeight);
            break;
        default: break;
    }

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    setDiffuseTexture("wall");
    setSpecularTexture("wall_spec");
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
            if (width > length)
                setTextureRepeat(width/length,1);
            else
                setTextureRepeat(1,length/width);
            setDiffuseTexture("platform");
            setSpecularTexture("platform_spec");
            break;
        case OBSTACLE_WALL:
            if (width > length)
                setTextureRepeat(width,height);
            else
                setTextureRepeat(length,height);
            setDiffuseTexture("wall_obstacle");
            setSpecularTexture("wall_obstacle_spec");
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

void drawWeapon(Player player, WeaponType type, float theta, float phi)
{
    const float pi2 = 1.57079632679;

    glm::vec4 v_up = glm::vec4(0.0f,1.0f,0.0f,0.0f);  // Vetor "up" fixado para apontar para o "céu" (eixo Y global)

    glm::vec4 w = Vetor(-player.view);
    glm::vec4 u = crossproduct(v_up,w);

    w = w / norm(w);
    u = u / norm(u);

    glm::vec4 v = crossproduct(w,u);

    glm::vec3 displace = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale    = glm::vec3(0.0f, 0.0f, 0.0f);

    switch (type)
    {
        case WPN_SWORD:
            displace = glm::vec3(0.3f,  0.15f, 0.5f);
            scale    = glm::vec3(0.02f, 0.02f, 0.02f);
            break;
        case WPN_PISTOL:
            displace = glm::vec3(0.3f,  0.15f, 0.4f);
            scale    = glm::vec3(0.02f, 0.02f, 0.02f);
            break;
        case WPN_SHOTGUN:
            displace = glm::vec3(0.3f,  0.15f, 0.65f);
            scale    = glm::vec3(0.01f, 0.01f, 0.01f);
            break;
        case WPN_MINIGUN:
            displace = glm::vec3(0.3f,  0.13f, 0.4f);
            scale    = glm::vec3(0.50f, 0.50f, 0.50f);
            break;
        case WPN_SNIPER:
            displace = glm::vec3(0.3f,  0.20f, 0.54f);
            scale    = glm::vec3(0.02f, 0.02f, 0.02f);
            break;
        default: break;
    }

    glm::vec4 vertical_displace   = -v*displace.y;
    glm::vec4 horizontal_displace =  u*displace.x * (1.0f - player.wpnAnimation);
    glm::vec4 forward_displace    = -w*displace.z;

    glm::vec4 weapon_pos = Ponto(player.pos)+vertical_displace+horizontal_displace+forward_displace;
    weapon_pos.y += player.neck;

    glm::mat4 model;

    // arma melee tem uma animação extra
    if (type == WPN_SWORD)
        model = Matrix_Rotate_Z(-pi2 + player.wpnAnimation * pi2);
    else
        model = Matrix_Identity();

    model = Matrix_Translate(weapon_pos.x,weapon_pos.y,weapon_pos.z) *
            Matrix_Rotate_Y(theta)  *
            Matrix_Rotate_X(-phi)   *
            Matrix_Rotate_Y(pi2)    *
            model                   *
            Matrix_Scale(scale.x, scale.y, scale.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    resetTextureRepeat();

    switch (type)
    {
        case WPN_SWORD:
            setDiffuseTexture("sword");
            setSpecularTexture("sword_spec");
            DrawVirtualObject("sword");
            break;
        case WPN_PISTOL:
            setDiffuseTexture("pistol");
            setSpecularTexture("black");
            DrawVirtualObject("pistol");
            break;
        case WPN_SHOTGUN:
            setDiffuseTexture("shotgun");
            setSpecularTexture("black");
            DrawVirtualObject("shotgun");
            break;
        case WPN_MINIGUN:
            setDiffuseTexture("minigun");
            setSpecularTexture("minigun_spec");
            DrawVirtualObject("minigun");
            break;
        case WPN_SNIPER:
            setDiffuseTexture("sniper");
            setSpecularTexture("sniper_spec");
            DrawVirtualObject("sniper");
            break;
        default: break;
    }
}

void drawProjectile(Projectile proj)
{
    if (proj.type == PROJ_MELEE_INVISIBLE)
        return;

    const float pi = 3.141592f;

    float width  = (proj.p_size.x);
    float length = (proj.p_size.z);
    float height = (proj.p_size.y);

    glm::mat4 model;

    if (proj.type == PROJ_BULLET)
        model = Matrix_Rotate_Y(pi);
    else
        model = Matrix_Identity();

    model = Matrix_Translate(proj.pos.x, proj.pos.y, proj.pos.z) *
            Matrix_Rotate_Y(getTheta(proj.dir))  *
            Matrix_Rotate_X(-getPhi(proj.dir))   *
            model                                *
            Matrix_Scale(width, height, length);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    switch (proj.type)
    {
        case PROJ_HITSCAN:
            setDiffuseTexture("blue");
            setSpecularTexture("white");
            glLineWidth(4.0f);
            DrawVirtualObject("line");
            break;
        case PROJ_BULLET:
            glUniform1i(g_use_spherical_uv_uniform, true);
            setDiffuseTexture("silver");
            setSpecularTexture("silver");
            DrawVirtualObject("the_sphere");
            glUniform1i(g_use_spherical_uv_uniform, false);
            break;
        default: break;
    }
}

void drawEnemy(Enemy enemy)
{
    const float pi2 = 1.57079632679;

    glm::vec3 og_size;
    switch (enemy.type)
    {

        case ENEMY_SKELETON:
        case ENEMY_BIG_SKELETON:
            og_size = glm::vec3(3.2f,7.2f,3.2f);
            break;
        case ENEMY_MINOTAUR:
            og_size = glm::vec3(1.0f,1.9f,0.6f);
            break;
        default: break;
    }

    glm::mat4 model = Matrix_Translate(enemy.pos.x, enemy.pos.y, enemy.pos.z) *
                      Matrix_Rotate_Y(getTheta(enemy.view) + pi2)             *
                      Matrix_Resize(og_size, enemy.model_size);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    bool isInCooldown = enemy.dmgCooldown > 0.0f;

    // inimigos são desenhados usando GOURAUD
    glUniform1i(g_use_gouraud_uniform, true);

    switch (enemy.type)
    {
        case ENEMY_SKELETON:
            setDiffuseTexture("white");

            if (isInCooldown)
                setSpecularTexture("red");
            else
                setSpecularTexture("grey");

            DrawVirtualObject("skeleton");
            break;
        case ENEMY_BIG_SKELETON:
            setDiffuseTexture("black");

            if (isInCooldown)
                setSpecularTexture("red");
            else
                setSpecularTexture("white");

            DrawVirtualObject("skeleton");
            break;
        case ENEMY_MINOTAUR:
            setDiffuseTexture("minotaur");

            if (isInCooldown)
                setSpecularTexture("red");
            else
                setSpecularTexture("minotaur_spec");

            DrawVirtualObject("minotaur");

            setDiffuseTexture("pants");

            if (!isInCooldown)
                setSpecularTexture("pants_spec");

            DrawVirtualObject("pants");
            break;
        default: break;
    }

    glUniform1i(g_use_gouraud_uniform, false);
}
