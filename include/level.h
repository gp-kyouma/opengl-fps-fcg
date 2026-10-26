#ifndef FCG_LEVEL
#define FCG_LEVEL

#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#define GLM_FORCE_CTOR_INIT
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
    glm::vec3 view = glm::vec3(0.0f,0.0f,1.0f);
    std::string type;

    Enemy buildEnemy();

    bool checkValid();
    void setValidDefaults(); // se há erros, seta pra valores default
};

struct ObstacleData
{
    glm::vec3 pos;
    glm::vec3 e_size = glm::vec3(1.0f,1.0f,1.0f);
    std::string type;

    Obstacle buildObstacle();

    bool checkValid();
    void setValidDefaults(); // se há erros, seta pra valores default
};

struct Level
{
    std::string level_name = "new_level";

    glm::vec3 player_pos;    // posição inicial do jogador ao iniciar a fase
    glm::vec3 player_view = glm::vec3(0.0f,0.0f,1.0f);   // direção onde o jogador está olhando ao iniciar a fase

    float levelWidth    = 10.0f;    // largura da fase no eixo X
    float levelLength   = 10.0f;    // comprimento da fase no eixo Z
    float levelFloor    = 0.0f;     // nível do chão
    float levelCeiling  = 10.0f;    // altura máxima da fase

    AABB levelWalls[6]; // AABBs usadas para colisão com as paredes (pra não ter que implementar planos)

    // obstáculos da fase
    std::vector<ObstacleData> obstacles;

    // inimigos da fase
    std::vector<EnemyData> enemies;

    AABB getAABB();
    glm::vec3 getLevelCenter();
    void setLevelWalls();

    //validations
    bool checkValidBoundaries();
    std::vector<std::string> checkValid(); // retorna lista de erros encontrados
    void setValidDefaults(); // se há erros, seta pra valores default
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnemyData, pos, view, type);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ObstacleData, pos, e_size, type);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Level, level_name, player_pos, player_view, levelWidth, levelLength, levelFloor, levelCeiling, obstacles, enemies);

#endif // FCG_LEVEL
