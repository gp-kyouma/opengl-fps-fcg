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

// W
glm::mat4 Matrix_Billboard(glm::mat4 view, glm::vec3 world_pos, glm::vec3 local_offset, glm::vec3 billboard_size)
{
    glm::mat4 transview = glm::transpose(view);

    transview[3][0] = world_pos.x;
    transview[3][1] = world_pos.y;
    transview[3][2] = world_pos.z;

    transview[0][3] = 0.0f;
    transview[1][3] = 0.0f;
    transview[2][3] = 0.0f;

    return transview * Matrix_Translate(local_offset.x, local_offset.y, local_offset.z) * Matrix_Scale(billboard_size.x, billboard_size.y, billboard_size.z);
}

void drawAABB(AABB aabb) // para razões de debug
{
    glm::vec3 aabb_center = aabb.getCenter();
    glm::vec3 aabb_size   = aabb.getSize();

    glm::mat4 model = Matrix_Translate(aabb_center.x, aabb_center.y, aabb_center.z) *
                      Matrix_Scale(aabb_size.x, aabb_size.y, aabb_size.z);

    setModelMatrix(model);

    setIgnoreLighting(true);
    setDiffuseColor(COLOR_WHITE);
    setSpecularColor(COLOR_BLACK);
    glLineWidth(4.0f);
    DrawVirtualObject("cube_edges");
    setIgnoreLighting(false);
}

void drawPoint(glm::vec3 point) // para razões de debug
{
    float point_size = 0.015f;

    glm::mat4 model = Matrix_Translate(point.x, point.y, point.z) * Matrix_Scale(point_size, point_size, point_size);

    setModelMatrix(model);

    setIgnoreLighting(true);
    setDiffuseColor(COLOR_CYAN);
    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("the_sphere");
    setIgnoreLighting(false);
}

void drawAxes(glm::vec3 center, glm::vec4 u, glm::vec4 v, glm::vec4 w) // para razões de debug
{
    glm::vec3 u3, v3, w3;
    float axis_size = 0.25f;

    u3 = toVec3(u);
    v3 = toVec3(v);
    w3 = toVec3(w);

    glm::mat4 model_center = Matrix_Translate(center.x, center.y, center.z);
    glm::mat4 model_scaler = Matrix_Scale(axis_size, axis_size, axis_size);

    glm::mat4 model_u = model_center * Matrix_Rotate_Y(getTheta(u3))  * Matrix_Rotate_X(-getPhi(u3)) * model_scaler;
    glm::mat4 model_v = model_center * Matrix_Rotate_Y(getTheta(v3))  * Matrix_Rotate_X(-getPhi(v3)) * model_scaler;
    glm::mat4 model_w = model_center * Matrix_Rotate_Y(getTheta(w3))  * Matrix_Rotate_X(-getPhi(w3)) * model_scaler;

    glLineWidth(6.0f);
    setSpecularColor(COLOR_BLACK);
    setIgnoreLighting(true);

    setModelMatrix(model_u);
    setDiffuseColor(COLOR_RED);
    DrawVirtualObject("line");

    setModelMatrix(model_v);
    setDiffuseColor(COLOR_GREEN);
    DrawVirtualObject("line");

    setModelMatrix(model_w);
    setDiffuseColor(COLOR_BLUE);
    DrawVirtualObject("line");

    setIgnoreLighting(false);
}

void drawCrosshair(float aspect)
{
    const float crosshair_size = 0.0625f; // 1/16 // make this dynamic, aka reuse this for sniper aiming // or maybe just change the color??

    glm::mat4 model = Matrix_Scale(crosshair_size / aspect, crosshair_size, 1.0f);

    setModelMatrix(model);

    setIgnoreLighting(true);
    setDiffuseColor(COLOR_WHITE);
    setSpecularColor(COLOR_BLACK);
    glLineWidth(2.0f);
    DrawVirtualObject("crosshair");
    setIgnoreLighting(false);
}

// Dado as model matrixes relevantes, desenha uma progress bar
// Cor (do vetor de cores) depende da % da barra
void drawBarArbitrary(glm::mat4 bg_model, glm::mat4 bar_model, float barRatio, glm::vec3 bg_color, std::vector<glm::vec3> bar_colors)
{
    setIgnoreLighting(true);

    setSpecularColor(COLOR_BLACK);

    setModelMatrix(bg_model);
    setDiffuseColor(bg_color);
    DrawVirtualObject("square");

    //set appropriate color
    int numColors = bar_colors.size();
    if (numColors == 1 || barRatio >= 1.0f)
        setDiffuseColor(bar_colors[0]);
    else if (barRatio <= 0.0f)
        setDiffuseColor(bar_colors[numColors-1]);
    else
    {
        int index = numColors - (std::ceil(barRatio * numColors));

        if (index >= numColors)
            index = numColors-1;

        setDiffuseColor(bar_colors[index]);
    }

    setModelMatrix(bar_model);
    DrawVirtualObject("square");

    setIgnoreLighting(false);
}

// Desenha uma progress bar diretamente na tela
// (usado para barra de HP e barra de cooldown)
// position começa de 0, cada incremento aumenta a posição vertical da barra (this is jank)
void drawBarNDC(float value, float maxValue, float aspect, std::vector<glm::vec3> colors, int position)
{
    // REMINDER THAT "SQUARE" IS SIZE 2 [-1,1], NOT SIZE 1 [-0.5,0.5]
    // SO SCALING BY 0.5 IS ACTUALLY SETTING SIZE TO 1
    glm::vec2 bgSize = glm::vec2(0.25f/aspect,0.05f);
    float barEdge    = 0.01f;

    glm::vec2 translate = glm::vec2(1.0f - (bgSize.x), -1.0f + (bgSize.y)*(position*2+1));

    // primeiro desenha o fundo
    glm::mat4 bg_model = Matrix_Translate(translate.x, translate.y, 0.0f) * Matrix_Scale(bgSize.x, bgSize.y, 1.0f);

    // depois desenha a barra
    float barRatio = value / maxValue;
    glm::vec2 barSize = glm::vec2(bgSize.x - barEdge, bgSize.y - barEdge);

    glm::mat4 bar_model = Matrix_Translate(((1.0f - (barRatio)) * barSize.x) + translate.x, translate.y, 0.0f) *
                          Matrix_Scale(barSize.x * barRatio, barSize.y, 1.0f);

    drawBarArbitrary(bg_model, bar_model, barRatio, COLOR_GREY, colors);
}

//Overload de drawBarNDC para barras de 1 cor
void drawBarNDC(float value, float maxValue, float aspect, glm::vec3 color, int position)
{
    std::vector<glm::vec3> oneColor = {color};
    drawBarNDC(value, maxValue, aspect, oneColor, position);
}

// Desenha uma progress bar em uma posição do mundo
void drawBarBillboard(glm::mat4 view, glm::vec3 pos, float value, float maxValue, std::vector<glm::vec3> colors)
{
    glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 bbsize  = glm::vec3(0.5f, 0.1f, 1.0f);

    glm::mat4 bg_model = Matrix_Billboard(view, pos, offset, bbsize);

    float barRatio = value / maxValue;

    offset.x  = (1.0f - (barRatio)) * bbsize.x;//right
    bbsize.x *= barRatio;

    glm::mat4 bar_model = Matrix_Billboard(view, pos, offset, bbsize);

    glDepthMask(GL_FALSE);
    drawBarArbitrary(bg_model, bar_model, barRatio, COLOR_BLACK, colors);
    glDepthMask(GL_TRUE);
}

//Overload de drawBarBillboard para barras de 1 cor
void drawBarBillboard(glm::mat4 view, glm::vec3 pos, float value, float maxValue, glm::vec3 color)
{
    std::vector<glm::vec3> oneColor = {color};
    drawBarBillboard(view, pos, value, maxValue, oneColor);
}

void drawBanner(float aspect, std::string tex) // used for game over, you won, etc...
{
    glm::vec2 bannerSize = glm::vec2(0.8f,0.6f); // 4:3

    glm::mat4 model = Matrix_Scale(bannerSize.x / aspect, bannerSize.y, 1.0f);

    setModelMatrix(model);

    setIgnoreLighting(true);
    setDiffuseTexture(tex);
    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("square");
    setIgnoreLighting(false);
}

void drawColorFade(glm::vec3 color, float alpha)
{
    glm::mat4 model = Matrix_Identity();

    setModelMatrix(model);

    setIgnoreLighting(true);
    setAlphaValue(alpha);
    setDiffuseColor(color);
    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("square");
    resetAlphaValue();
    setIgnoreLighting(false);
}

void drawTextureFade(std::string tex, float alpha)
{
    glm::mat4 model = Matrix_Identity();

    setModelMatrix(model);

    setIgnoreLighting(true);
    setAlphaValue(alpha);
    setDiffuseTexture(tex);
    setSpecularColor(COLOR_BLACK);
    DrawVirtualObject("square");
    resetAlphaValue();
    setIgnoreLighting(false);
}

void drawColorCompare(float aspect)//debug
{
    glm::vec2 bannerSize = glm::vec2(0.1f,0.2f);
    glm::vec2 translate = glm::vec2(-0.4f,0.0f);

    std::vector<glm::vec3> colors = {COLOR_WHITE, COLOR_GREY, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW};

    glm::mat4 modelscale = Matrix_Scale(bannerSize.x / aspect, bannerSize.y, 1.0f);
    glm::mat4 model;

    setSpecularColor(COLOR_BLACK);

    setIgnoreLighting(true);

    for (int i = 0; i < 9; i++)
    {
        model = Matrix_Translate(translate.x, translate.y, 0.0f) * modelscale;

        setModelMatrix(model);

        setDiffuseColor(colors[i]);
        DrawVirtualObject("square");

        translate.x += 0.1;
    }

    setIgnoreLighting(false);
}

void drawFloor(Level level)
{
    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);

    glm::mat4 model = Matrix_Translate(0.0f,level.levelFloor,0.0f) *
                      Matrix_Scale(halfWidth, 1.0f, halfLength);

    setModelMatrix(model);

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

    setModelMatrix(model);

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

    setModelMatrix(model);

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
        case OBSTACLE_INVISIBLE_WALL:
            return;//PLACEHOLDER
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

    glm::vec4 u,v,w;
    calculate_uvw(player.view,u,v,w);

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

    glm::mat4 model = Matrix_Identity();

    // forced aim, held up while not aiming
    if (type == WPN_MINIGUN)
        model = Matrix_Rotate_Z(-pi2 + player.wpnAnimation * pi2);

    // arma melee tem uma animação extra
    // (v3: now with ACTUAL swings!)

    if (type == WPN_SWORD)
    {
        float melee_rotate; // 1 is pointed forward, 0 is pointed up
        float cooldown_percent = (player.wpnState == WPNSTATE_COOLDOWN) ? (player.wpnCooldown / player.getCurrentWeapon().cooldown) : 0.0f;

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
        horizontal_displace *= (1.0f - player.wpnAnimation);

    glm::vec4 weapon_pos = Ponto(player.pos)+vertical_displace+horizontal_displace+forward_displace;
    weapon_pos.y += player.neck;

    model = Matrix_Translate(weapon_pos.x,weapon_pos.y,weapon_pos.z) *
            Matrix_Rotate_Y(theta)  *
            Matrix_Rotate_X(-phi)   *
            Matrix_Rotate_Y(pi2)    *
            model                   *
            Matrix_Scale(scale.x, scale.y, scale.z);

    setModelMatrix(model);

    if (player.wpnState == WPNSTATE_DRAW)
        setAlphaValue(0.5f);

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

    resetAlphaValue();
}

void drawProjectile(Projectile proj)
{
    if (proj.type == PROJ_MELEE_INVISIBLE)
        return;

    const float pi = 3.141592f;

    float width  = (proj.e_size.x);
    float length = (proj.e_size.z);
    float height = (proj.e_size.y);

    glm::mat4 model = Matrix_Identity();

    glm::vec3 proj_pos = proj.pos;

    if (proj.type == PROJ_BULLET)
        model = Matrix_Rotate_Y(pi); // texture seam facing towards enemy

    if (proj.type == PROJ_HITSCAN)
    {
        // simulate bullet moving
        float move_speed = 50.0f;//should be the same as max length
        float wall_embed = 0.125f;
        float move_factor = (0.15f - proj.lifespan) * move_speed;

        if (move_factor > length-wall_embed)
        {
            move_factor = length-wall_embed;
            length  = wall_embed+wall_embed; // bullet embedded into whatever it hit
        }
        else length = 1.0f; // bullet "length"

        proj_pos = proj.pos + proj.view * move_factor;
    }

    model = Matrix_Translate(proj_pos.x, proj_pos.y, proj_pos.z) *
            Matrix_Rotate_Y(getTheta(proj.view))  *
            Matrix_Rotate_X(-getPhi(proj.view))   *
            model                                 *
            Matrix_Scale(width, height, length);

    setModelMatrix(model);

    switch (proj.type)
    {
        case PROJ_HITSCAN:
            setDiffuseColor(COLOR_BLUE);
            setSpecularColor(COLOR_WHITE);
            glLineWidth(4.0f);
            DrawVirtualObject("line");
            break;
        case PROJ_BULLET:
            setUseSphericalUV(true);
            setDiffuseTexture("silver");
            setSpecularTexture("silver");
            DrawVirtualObject("the_sphere");
            setUseSphericalUV(false);
            break;
        default: break;
    }
}

void drawEnemy(Enemy enemy)
{
    const float pi2 = 1.57079632679;

    glm::vec3 og_size;
    glm::vec3 model_size;
    switch (enemy.type)
    {
        case ENEMY_SKELETON:
            og_size = glm::vec3(3.2f,7.2f,3.2f);
            model_size = glm::vec3(1.0f,2.0f,1.0f);
            break;
        case ENEMY_BIG_SKELETON:
            og_size = glm::vec3(3.2f,7.2f,3.2f);
            model_size = glm::vec3(1.5f,3.0f,1.5f);
            break;
        case ENEMY_MINOTAUR:
            og_size = glm::vec3(1.0f,1.9f,0.6f);
            model_size = glm::vec3(2.5f,5.0f,1.5f);
            break;
        default: break;
    }

    glm::mat4 model = Matrix_Translate(enemy.pos.x, enemy.pos.y, enemy.pos.z) *
                      Matrix_Rotate_Y(getTheta(enemy.view) + pi2)             *
                      Matrix_Resize(og_size, model_size);

    setModelMatrix(model);

    bool isInCooldown = enemy.dmgCooldown > 0.0f;

    // inimigos são desenhados usando GOURAUD
    setUseGouraud(true);

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

    setUseGouraud(false);
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

// Escrevemos na tela a vida atual de um ator.
void drawHealth(GLFWwindow* window, Actor& actor)
{
    char buffer[10] = "9999/9999";

    snprintf(buffer, 10, "%d/%d", actor.health, actor.maxHealth);

    DrawString(window, buffer, 0.995f, -0.97f, TEXTPOS_BOTTOM, TEXTPOS_RIGHT, 3.0f, false, COLOR_BLACK);
}

// Escrevemos na tela a posição de uma entidade
void drawPosition(GLFWwindow* window, Entity& entity)
{
    char bufferX[20] = "X = -???.??????";
    char bufferY[20] = "Y = -???.??????";
    char bufferZ[20] = "Z = -???.??????";

    snprintf(bufferX, 20, "X = %+f", entity.pos.x);
    snprintf(bufferY, 20, "Y = %+f", entity.pos.y);
    snprintf(bufferZ, 20, "Z = %+f", entity.pos.z);

    DrawString(window, bufferX, 1.0f, 0.80f, TEXTPOS_CENTER, TEXTPOS_RIGHT, 2.0f, false, COLOR_WHITE);
    DrawString(window, bufferY, 1.0f, 0.75f, TEXTPOS_CENTER, TEXTPOS_RIGHT, 2.0f, false, COLOR_WHITE);
    DrawString(window, bufferZ, 1.0f, 0.70f, TEXTPOS_CENTER, TEXTPOS_RIGHT, 2.0f, false, COLOR_WHITE);
    DrawString(window, "GROUNDED", 1.0f, 0.65f, TEXTPOS_CENTER, TEXTPOS_RIGHT, 2.0f, false, entity.grounded ? COLOR_GREEN : COLOR_RED);//show if grounded
}

void Enemy::draw()
{
    drawEnemy(*this);
    if (this->dmgCooldown > 0.0f)
    {
        glm::vec3 bar_pos = this->pos;
        bar_pos.y = this->getHitbox().aabb_max.y + 0.25f;
        std::vector<glm::vec3> healthColors = {COLOR_GREEN, COLOR_GREEN, COLOR_YELLOW, COLOR_RED};
        drawBarBillboard(g_CurrentViewMatrix, bar_pos, (float)this->health, (float)this->maxHealth, healthColors);
    }
}

void Obstacle::draw()
{
    drawObstacle(*this);
}

void Projectile::draw()
{
    drawProjectile(*this);
}

void Player::draw(){}//unimplemented
