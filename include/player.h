#ifndef FCG_PLAYER
#define FCG_PLAYER

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
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
    glm::vec3 pos;    // posi��o do jogador no mundo
    glm::vec3 p_size; // tamanho da hitbox do jogador
    glm::vec3 view;   // onde o jogador est� olhando
    float neck;       // offset no eixo y em rela��o a world_pos que define onde a c�mera vai ser posicionada

    bool grounded;    // indica se o jogador est� tocando o ch�o
    float y_velocity; // velocidade do jogador no eixo y

    float speed;      // velocidade em que o jogador se move

    const float maxHealth = 100;    // o valor m�ximo de vida que o jogador pode ter; reseta a cada fase
    float health;       // vida atual do jogador
    float dmgCooldown;  // tempo em segundos at� que o jogador possa receber dano de novo

    int currentWeapon;  // �ndice no vetor de armas que determina a arma equipada
    float wpnCooldown;  // tempo em segundos at� que o jogador possa atirar de novo

    // etc outros atributos ... todo
    // vetor de armas vai aqui mesmo

    void setView(float theta, float phi);
    void movePos(glm::vec3 offset);

    AABB getAABB();

    void doPlayerMovement(float deltaTime);

    //todo:
    //Weapon getCurrentWeapon();
    //bool fire(Projectile new_proj);

    // etc outros m�todos ... todo
};

#endif // FCG_PLAYER
