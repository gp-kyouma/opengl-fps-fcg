#ifndef FCG_DRAW
#define FCG_DRAW

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"

#include "renderer.h"
#include "collisions.h"
#include "player.h"
#include "level.h"

/*
ENUMS
*/

enum CardinalDirection
{
    NORTH,  // z+
    SOUTH,  // z-
    EAST,   // x-
    WEST,   // x+
    UP,     // y+
    DOWN,   // y-
};

/*
DECLARAÇÃO DE FUNÇÕES
*/

void drawAABB(AABB aabb); // para razões de debug

void drawFloor(Level level);
void drawWall(Level level, CardinalDirection direction);

// draw weapon, draw enemy, draw projectile, draw obstacle, etc etc...

#endif // FCG_DRAW
