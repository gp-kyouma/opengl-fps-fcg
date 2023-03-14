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

/*
DECLARAÇÃO DE FUNÇÕES
*/

void drawAABB(AABB aabb); // para razões de debug

void drawCrosshair(float aspect);
void drawBar(float value, float maxValue, float aspect, std::string tex1, std::string tex2, std::string tex3, int position);

void drawFloor(Level level);
void drawWall(Level level, CardinalDirection direction);
void drawObstacle(Obstacle obstacle);
void drawWeapon(Player player, WeaponType type, float theta, float phi);

// draw enemy, draw projectile, etc etc...

#endif // FCG_DRAW
