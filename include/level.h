#ifndef FCG_LEVEL
#define FCG_LEVEL

#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obstacle.h"
#include "enemy.h"

//JSON
#include "vec_aux.h"//for vec3 json
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

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
    std::string level_name;

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

//for export reasons
void exportLevelToFile(Level level);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnemyData, pos, type);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Obstacle, pos, e_size, type);//Yes this should be its own obstacledata struct. Don't worry about it
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Level, level_name, player_pos, player_view, levelWidth, levelLength, levelFloor, levelCeiling, obstacles, enemies);

#endif // FCG_LEVEL
