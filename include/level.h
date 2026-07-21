#ifndef FCG_LEVEL
#define FCG_LEVEL

#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obstacle.h"
#include "enemy.h"

/*
STRUCTS/CLASSES
*/

struct EnemyData
{
    glm::vec3 pos;
    EnemyType type;

    Enemy buildEnemy();
};

struct Level
{
    glm::vec3 player_pos;    // posição inicial do jogador ao iniciar a fase
    glm::vec3 player_view;   // direção onde o jogador está olhando ao iniciar a fase

    float levelWidth;  // largura da fase no eixo X
    float levelLength; // comprimento da fase no eixo Z
    float levelFloor   = 1.0f;    // nível do chão
    float levelCeiling = 11.0f;   // altura máxima da fase

    AABB levelWalls[6]; // AABBs usadas para colisão com as paredes (pra não ter que implementar planos)//(This will not go on the json)

    // obstáculos da fase
    std::vector<Obstacle> obstacles;

    // inimigos da fase
    std::vector<EnemyData> enemies;

    AABB getAABB();
    void setLevelWalls();

    // funções que geram cada fase
    void createTestLevel();
    void createLevel1();
    void createLevel2();
    void createLevel3();
    void createBossLevel();
};

#endif // FCG_LEVEL
