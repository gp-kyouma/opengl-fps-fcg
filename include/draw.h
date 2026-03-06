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
void drawColorCompare(float aspect);//debug

void drawCrosshair(float aspect);
void drawBar(float value, float maxValue, float aspect, std::vector<glm::vec3> colors, int position); //n colors
void drawBar(float value, float maxValue, float aspect, glm::vec3 color, int position); //1 color
void drawBanner(float aspect, std::string tex); // used for game over, you won, etc...

void drawFloor(Level level);
void drawWall(Level level, CardinalDirection direction);
void drawObstacle(Obstacle obstacle);
void drawWeapon(Player player, WeaponType type, float theta, float phi);
void drawProjectile(Projectile proj);
void drawEnemy(Enemy enemy);

void drawTimer(GLFWwindow* window, float timer, bool hud);

#endif // FCG_DRAW
