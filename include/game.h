#ifndef FCG_GAME
#define FCG_GAME

#include <deque>
#include <vector>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#define GLM_FORCE_CTOR_INIT
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "player.h"
#include "level.h"
#include "obstacle.h"
#include "weapon.h"
#include "projectile.h"
#include "enemy.h"

/*
STRUCTS/CLASSES
*/

// Estrutura que descreve o estado do jogo
struct Game
{
    // informações de tempo
    float prevTime;
    float deltaTime;
    float levelTime;
    float totalTime;

    Player player;

    // vetor de obstáculos
    std::vector<Obstacle> obstacles;

    // vetor de inimigos
    std::vector<Enemy> enemies;

    // vetor de projéteis
    std::vector<Projectile> projectiles;

    // fila de níveis que o jogo tem
    Level level;
    std::deque<std::string> levelList;

    // próximo nível a carregar
    // string vazia = avança na levelList
    std::string nextlevel;

    // se o jogador está morto, ao fim de fase, ou durante a cutscene final:
    // pula o update do jogador, inimigos, etc.
    bool noUpdate(); // getter
    bool levelCleared;

    // atributos referentes à cutscene de fim de jogo
    bool cutscene;
    float cutsceneStep; // determina em qual parte da cutscene está; [0.0f, 6.0f]
    glm::vec3 cutscenePos;

    void updateDeltaTime();

    void Init();
    void Update();
    void Draw(GLFWwindow* window);

    void initCutscene();
    void updateCutscene();
    void drawCutscene(GLFWwindow* window);

    bool isGameOver();
    bool isLevelClear();
    bool isLastLevel();
    void checkLevelEnd();
    void checkStartCutscene();

    void initCurrentLevel();
    void loadNextLevel();
    void loadGotoLevel(std::string levelname);

    void doConsoleCommand(std::string cmd);

private:
    void loadLevelFile(std::string levelname);
};

#endif // FCG_GAME
