#ifndef FCG_PLAYER
#define FCG_PLAYER

#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "collisions.h"
#include "weapon.h"
#include "projectile.h"
#include "entity.h"

/*
STRUCTS/CLASSES
*/

// Estrutura que descreve o jogador
struct Player : Actor
{
    //Entity + Actor attributes omitted

    float neck;       // offset no eixo y em relação a pos que define onde a câmera vai ser posicionada

    int currentWeapon;  // índice no vetor de armas que determina a arma equipada
    float wpnCooldown;  // tempo em segundos até que o jogador possa atirar de novo
    float wpnAnimation; // valor entre 0.0f e 1.0f, define a animação da arma quando está pressionado o botão esquerdo do mouse

    std::vector<Weapon> weapons;    // armas disponíveis ao jogador

    void setView(float theta, float phi);

    AABB getHitbox();

    void update(float deltaTime);
    void doWeaponSwitch();

    Weapon getCurrentWeapon();

    bool fire(Projectile &new_proj);

    void resetHealth();
    void takeDamage(int dmg);
    bool isDead();

private: // individual parts of update(deltaTime) because i don't want to merge all these things into 1 function, that's cringe
    void doPlayerMovement(float deltaTime);
    void doDamageCooldown(float deltaTime);
    void doWeaponAnimation(float deltaTime);
    void doWeaponCooldown(float deltaTime);
};

#endif // FCG_PLAYER
