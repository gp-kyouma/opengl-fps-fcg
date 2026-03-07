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

// some color constants
glm::vec3 COLOR_WHITE   = glm::vec3(1.0f,   1.0f,   1.0f);
glm::vec3 COLOR_GREY    = glm::vec3(0.05f,  0.05f,  0.05f);//does not map to [0,255] as i expected it to...?
glm::vec3 COLOR_BLACK   = glm::vec3(0.0f,   0.0f,   0.0f);

glm::vec3 COLOR_RED     = glm::vec3(1.0f,   0.0f,   0.0f);
glm::vec3 COLOR_GREEN   = glm::vec3(0.0f,   1.0f,   0.0f);
glm::vec3 COLOR_BLUE    = glm::vec3(0.0f,   0.0f,   1.0f);

glm::vec3 COLOR_CYAN    = glm::vec3(0.0f,   1.0f,   1.0f);
glm::vec3 COLOR_MAGENTA = glm::vec3(1.0f,   0.0f,   1.0f);
glm::vec3 COLOR_YELLOW  = glm::vec3(1.0f,   1.0f,   0.0f);

void drawAABB(AABB aabb) // para razões de debug
{
    glm::vec3 aabb_center = aabb.getCenter();
    glm::vec3 aabb_size   = aabb.getSize();

    glm::mat4 model = Matrix_Translate(aabb_center.x, aabb_center.y, aabb_center.z) *
                      Matrix_Scale(aabb_size.x, aabb_size.y, aabb_size.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseColor(COLOR_WHITE);
    setSpecularColor(COLOR_BLACK);
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
    setDiffuseColor(COLOR_WHITE);
    setSpecularColor(COLOR_BLACK);
    glLineWidth(2.0f);
    DrawVirtualObject("crosshair");
    glUniform1i(g_ignore_lighting_uniform, false);
}

// Desenha uma progress bar
// (usado para barra de HP e barra de cooldown)
// Cor (do vetor de cores) depende da % da barra
// position começa de 0, cada incremento aumenta a posição vertical da barra (this is jank)
void drawBar(float value, float maxValue, float aspect, std::vector<glm::vec3> colors, int position)
{
    // REMINDER THAT "SQUARE" IS SIZE 2 [-1,1], NOT SIZE 1 [-0.5,0.5]
    // SO SCALING BY 0.5 IS ACTUALLY SETTING SIZE TO 1
    glm::vec2 bgSize = glm::vec2(0.25f,0.05f);
    float barEdge    = 0.015f;

    bgSize.x /= aspect;

    glm::vec2 translate = glm::vec2(1.0f - (bgSize.x), -1.0f + (bgSize.y)*(position*2+1));

    // primeiro desenha o fundo
    glm::mat4 model = Matrix_Translate(translate.x, translate.y, 0.0f) *
                      Matrix_Scale(bgSize.x, bgSize.y, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseColor(COLOR_GREY);
    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("square");

    // depois desenha a barra
    float barRatio = value / maxValue;
    glm::vec2 barSize = glm::vec2(bgSize.x - barEdge, bgSize.y - barEdge);

    model = Matrix_Translate(translate.x, translate.y, 0.0f)              *
            Matrix_Translate((1.0f - (barRatio)) * barSize.x, 0.0f, 0.0f) *
            Matrix_Scale(barRatio, 1.0f, 1.0f)                            *
            Matrix_Scale(barSize.x, barSize.y, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    //set appropriate color
    int numColors = colors.size();
    if (numColors == 1 || barRatio >= 1.0f)
        setDiffuseColor(colors[0]);
    else if (barRatio <= 0.0f)
        setDiffuseColor(colors[numColors-1]);
    else
    {
        int index = numColors - (std::ceil(barRatio * numColors));

        if (index >= numColors)
            index = numColors-1;

        setDiffuseColor(colors[index]);
    }

    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("square");
    glUniform1i(g_ignore_lighting_uniform, false);
}

//Overload de drawBar para barras de 1 cor
void drawBar(float value, float maxValue, float aspect, glm::vec3 color, int position)
{
    std::vector<glm::vec3> oneColor = {color};
    drawBar(value, maxValue, aspect, oneColor, position);
}

void drawBanner(float aspect, std::string tex) // used for game over, you won, etc...
{
    glm::vec2 bannerSize = glm::vec2(0.8f,0.6f); // 3:4

    glm::mat4 model = Matrix_Scale(bannerSize.x / aspect, bannerSize.y, 1.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    glUniform1i(g_ignore_lighting_uniform, true);
    setDiffuseTexture(tex);
    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("square");
    glUniform1i(g_ignore_lighting_uniform, false);
}

void drawColorCompare(float aspect)//debug
{
    glm::vec2 bannerSize = glm::vec2(0.1f,0.2f);
    glm::vec2 translate = glm::vec2(-0.4f,0.0f);

    std::vector<glm::vec3> colors = {COLOR_WHITE, COLOR_GREY, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW};

    glm::mat4 modelscale = Matrix_Scale(bannerSize.x / aspect, bannerSize.y, 1.0f);
    glm::mat4 model;

    setSpecularColor(COLOR_BLACK);

    glUniform1i(g_ignore_lighting_uniform, true);

    for (int i = 0; i < 9; i++)
    {
        model = Matrix_Translate(translate.x, translate.y, 0.0f) * modelscale;

        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

        setDiffuseColor(colors[i]);
        DrawVirtualObject("square");

        translate.x += 0.1;
    }

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
    setSpecularColor(COLOR_GREY);
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
    float width  = (obstacle.e_size.x);
    float length = (obstacle.e_size.z);
    float height = (obstacle.e_size.y);

    glm::mat4 model;
    model = Matrix_Translate(obstacle.pos.x, obstacle.pos.y, obstacle.pos.z) *
            Matrix_Scale(width, height, length);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    bool useSixSideTexture = false;
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
            setDiffuseTexture("box");
            setSpecularColor(COLOR_BLACK);
            break;
        case OBSTACLE_DICE:
            useSixSideTexture = true;
            setDiffuseTexture("dice");
            setSpecularColor(COLOR_BLACK);
            break;
        default: break;
    }

    if (useSixSideTexture)
        DrawVirtualObject("cube_tex");
    else
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
    glm::vec4 horizontal_displace =  u*displace.x;
    glm::vec4 forward_displace    = -w*displace.z;

    glm::mat4 model;

    // arma melee tem uma animação extra
    // (v3: now with ACTUAL swings!)

    if (type == WPN_SWORD)
    {
        float melee_rotate; // 1 is pointed forward, 0 is pointed up
        float cooldown_percent = player.wpnCooldown / player.getCurrentWeapon().cooldown;

        float wide_rotate = player.wpnAnimation;//aim...

        //NONE OF THIS WILL MAKE SENSE IF I REPURPOSE WPNANIMATION FOR AIM
        //SO WATCH OUT FOR THAT
        /*
        if (player.wpnAnimation < 1.0f)
            melee_rotate = player.wpnAnimation;
        else
            melee_rotate = fabs((cooldown_percent * 2.0f) - 1.0f); //1-0-1
        */

        //proper swing (might be too slow)
        //sword needs noAim(?)
        melee_rotate = fabs((cooldown_percent * 2.0f) - 1.0f); //1-0-1
        melee_rotate = -melee_rotate + 1.0f; //0-1-0

        // mr 0, wr 0 = left
        // mr 0, wr 1 = middle
        // mr 1, wr 0 = middle
        // mr 1, wr 1 = right
        horizontal_displace *= (1.0f - melee_rotate - (wide_rotate*1.375));

        model = Matrix_Rotate_X(wide_rotate * -pi2 * 0.875f) * Matrix_Rotate_Z(-pi2 + melee_rotate * pi2);
    }
    else
    {
        model = Matrix_Identity();
        horizontal_displace *= (1.0f - player.wpnAnimation);
    }

    glm::vec4 weapon_pos = Ponto(player.pos)+vertical_displace+horizontal_displace+forward_displace;
    weapon_pos.y += player.neck;

    model = Matrix_Translate(weapon_pos.x,weapon_pos.y,weapon_pos.z) *
            Matrix_Rotate_Y(theta)  *
            Matrix_Rotate_X(-phi)   *
            Matrix_Rotate_Y(pi2)    *
            model                   *
            Matrix_Scale(scale.x, scale.y, scale.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    switch (type)
    {
        case WPN_SWORD:
            setDiffuseTexture("sword");
            setSpecularTexture("sword_spec");
            DrawVirtualObject("sword");
            break;
        case WPN_PISTOL:
            setDiffuseTexture("pistol");
            setSpecularColor(COLOR_BLACK);
            DrawVirtualObject("pistol");
            break;
        case WPN_SHOTGUN:
            setDiffuseTexture("shotgun");
            setSpecularColor(COLOR_BLACK);
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

    float width  = (proj.e_size.x);
    float length = (proj.e_size.z);
    float height = (proj.e_size.y);

    glm::mat4 model;

    if (proj.type == PROJ_BULLET)
        model = Matrix_Rotate_Y(pi);
    else
        model = Matrix_Identity();

    model = Matrix_Translate(proj.pos.x, proj.pos.y, proj.pos.z) *
            Matrix_Rotate_Y(getTheta(proj.view))  *
            Matrix_Rotate_X(-getPhi(proj.view))   *
            model                                *
            Matrix_Scale(width, height, length);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

    switch (proj.type)
    {
        case PROJ_HITSCAN:
            //setDiffuseColor(COLOR_BLUE);
            //setSpecularColor(COLOR_WHITE);
            setDiffuseColor(COLOR_YELLOW);
            setSpecularColor(COLOR_RED);
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
            setDiffuseColor(COLOR_GREY);

            if (isInCooldown)
                setSpecularColor(COLOR_RED);
            else
                setSpecularColor(COLOR_WHITE);

            DrawVirtualObject("skeleton");
            break;
        case ENEMY_BIG_SKELETON:
            setDiffuseColor(COLOR_BLACK);

            if (isInCooldown)
                setSpecularColor(COLOR_RED);
            else
                setSpecularColor(COLOR_GREY);

            DrawVirtualObject("skeleton");
            break;
        case ENEMY_MINOTAUR:
            setDiffuseTexture("minotaur");

            if (isInCooldown)
                setSpecularColor(COLOR_RED);
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

// Escrevemos na tela um tempo (em segundos).
// hud == true: desenha no canto superior direito
// hud == false: desenha perto do meio
void drawTimer(GLFWwindow* window, float timer, bool hud)
{
    char buffer[20] = "??:??:???";

    int seconds = std::floor(timer);
    int minutes = (seconds/60);
    int hours   = (minutes/60);
    int m_seconds = (timer - seconds) * 1000;

    seconds = seconds % 60;
    minutes = minutes % 60;

    if (hours == 0)
        snprintf(buffer, 20, "%02d:%02d:%03d", minutes, seconds, m_seconds);
    else if (hours < 99)
        snprintf(buffer, 20, "%02d:%02d:%02d:%03d", hours, minutes, seconds, m_seconds);
    else
        snprintf(buffer, 20, "Bro are you ok?");

    if (hud)
        DrawString(window, buffer, 1.0f, 1.0f, TEXTPOS_TOP, TEXTPOS_RIGHT, 3.0f, false, COLOR_WHITE);
    else
        DrawString(window, buffer, 0.0f, -0.75f, TEXTPOS_CENTER, TEXTPOS_CENTER, 4.5f, false, COLOR_WHITE);
}
