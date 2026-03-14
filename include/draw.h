#ifndef FCG_DRAW
#define FCG_DRAW

#include <string>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"

#include "renderer.h"
#include "collisions.h"
#include "player.h"
#include "obstacle.h"
#include "level.h"
#include "weapon.h"
#include "projectile.h"
#include "enemy.h"

/*
CONSTANTES GLOBAIS
*/
extern glm::vec3 COLOR_WHITE;
extern glm::vec3 COLOR_GREY;
extern glm::vec3 COLOR_BLACK;

extern glm::vec3 COLOR_RED;
extern glm::vec3 COLOR_GREEN;
extern glm::vec3 COLOR_BLUE;

extern glm::vec3 COLOR_CYAN;
extern glm::vec3 COLOR_MAGENTA;
extern glm::vec3 COLOR_YELLOW;

/*
DECLARAÇÃO DE FUNÇÕES
*/

void drawAABB(AABB aabb); // para razões de debug
void drawPoint(glm::vec3 point); // para razões de debug
void drawAxes(glm::vec3 center, glm::vec4 u, glm::vec4 v, glm::vec4 w); // para razões de debug
void drawColorCompare(float aspect);//debug

void drawCrosshair(float aspect);
void drawBarNDC(float value, float maxValue, float aspect, std::vector<glm::vec3> colors, int position); //n colors
void drawBarNDC(float value, float maxValue, float aspect, glm::vec3 color, int position); //1 color
void drawBarBillboard(glm::mat4 view, glm::vec3 pos, float value, float maxValue, std::vector<glm::vec3> colors); //n colors
void drawBarBillboard(glm::mat4 view, glm::vec3 pos, float value, float maxValue, glm::vec3 color); //1 color
void drawBanner(float aspect, std::string tex); // used for game over, you won, etc...

void drawColorFade(glm::vec3 color, float alpha);
void drawTextureFade(std::string tex, float alpha);

void drawFloor(Level level);
void drawWall(Level level, CardinalDirection direction);
void drawObstacle(Obstacle obstacle);
void drawWeapon(Player player, WeaponType type, float theta, float phi);
void drawProjectile(Projectile proj);
void drawEnemy(Enemy enemy);

void drawTimer(GLFWwindow* window, float timer, bool hud);
void drawHealth(GLFWwindow* window, Actor& actor);

#endif // FCG_DRAW
