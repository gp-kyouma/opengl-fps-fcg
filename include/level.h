#ifndef FCG_LEVEL
#define FCG_LEVEL

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
STRUCTS/CLASSES
*/

struct Level
{
    glm::vec3 player_pos;    // posição inicial do jogador ao iniciar a fase
    glm::vec3 player_view;   // direção onde o jogador está olhando ao iniciar a fase

    float levelWidth;  // largura da fase no eixo X
    float levelLength; // comprimento da fase no eixo Z
    const float levelFloor   = 1.0f;    // nível do chão
    const float levelCeiling = 11.0f;   // altura máxima da fase

    // etc outros atributos ... todo
};

#endif // FCG_LEVEL
