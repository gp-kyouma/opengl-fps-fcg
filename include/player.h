#ifndef FCG_PLAYER
#define FCG_PLAYER

#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"
#include "weapon.h"
//#include "projectile.h"

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

    float speed;      // velocidade em que o jogador se move

    const int maxHealth = 100;  // o valor máximo de vida que o jogador pode ter; reseta a cada fase
    int health;         // vida atual do jogador
    float dmgCooldown;  // tempo em segundos até que o jogador possa receber dano de novo

    int currentWeapon;  // índice no vetor de armas que determina a arma equipada
    float wpnCooldown;  // tempo em segundos até que o jogador possa atirar de novo
    float wpnAnimation; // valor entre 0.0f e 1.0f, defina a animação da arma quando está pressionada o botão esquerdo do mouse

    std::vector<Weapon> weapons;    // armas disponíveis ao jogador

    // etc outros atributos ... todo

    void setView(float theta, float phi);
    void movePos(glm::vec3 offset);

    AABB getAABB();

    void doPlayerMovement(float deltaTime);
    void doDamageCooldown(float deltaTime);
    void doWeaponAnimation(float deltaTime);
    void doWeaponCooldown(float deltaTime);
    void doWeaponSwitch();

    Weapon getCurrentWeapon();

    //todo:
    //bool fire(Projectile &new_proj);

    void resetHealth();
    void takeDamage(int dmg);
    bool isDead();

    // etc outros métodos ... todo
};

#endif // FCG_PLAYER
