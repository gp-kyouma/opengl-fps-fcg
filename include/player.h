#ifndef FCG_PLAYER
#define FCG_PLAYER

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"

/*
STRUCTS/CLASSES
*/

// Estrutura que descreve o jogador
struct Player
{
    glm::vec3 pos;    // posição do jogador no mundo
    glm::vec3 p_size; // tamanho da hitbox do jogador
    glm::vec3 view;   // onde o jogador está olhando
    float neck;       // offset no eixo y em relação a world_pos que define onde a câmera vai ser posicionada

    bool grounded;    // indica se o jogador está tocando o chão
    float y_velocity; // velocidade do jogador no eixo y

    float speed;      // velocidade em que o jogador se locomove

    // etc outros atributos ... todo

    void setView(float theta, float phi);
    void movePos(glm::vec3 offset);

    AABB getAABB();

    void doPlayerMovement(float deltaTime);

    // etc outros métodos ... todo
};

#endif // FCG_PLAYER
