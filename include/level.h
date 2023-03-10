#ifndef FCG_LEVEL
#define FCG_LEVEL

#include <vector>

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obstacle.h"

/*
STRUCTS/CLASSES
*/

// todo:
// obstacle data, enemy data...

struct Level
{
    glm::vec3 player_pos;    // posi��o inicial do jogador ao iniciar a fase
    glm::vec3 player_view;   // dire��o onde o jogador est� olhando ao iniciar a fase

    float levelWidth;  // largura da fase no eixo X
    float levelLength; // comprimento da fase no eixo Z
    const float levelFloor   = 1.0f;    // n�vel do ch�o
    const float levelCeiling = 11.0f;   // altura m�xima da fase

    // obst�culos da fase
    std::vector<Obstacle> obstacles;

    // etc outros atributos ... todo
};

#endif // FCG_LEVEL
