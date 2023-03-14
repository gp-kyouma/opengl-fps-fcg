#ifndef FCG_GAME
#define FCG_GAME

#include <queue>
#include <vector>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "player.h"
#include "level.h"
#include "obstacle.h"
#include "weapon.h"

/*
STRUCTS/CLASSES
*/

// Estrutura que descreve o estado do jogo
struct Game
{
    // informações de tempo
    float prevTime;
    float deltaTime;

    Player player;

    // vetor de obstáculos
    std::vector<Obstacle> obstacles;

    // vetor de inimigos
    // todo

    // vetor de projéteis
    // todo

    // fila de níveis que o jogo tem
    std::queue<Level> level_queue;

    // se o jogador está morto ou durante a cutscene final, pula o update do jogador, inimigos, etc.
    //bool noUpdate;

    // atributos referentes à cutscene de fim de jogo
    //bool endCutscene;
    //float cutsceneStep;

    // etc outros atributos ... todo?

    void updateDeltaTime();
    void Init();
    void Update();
    void Draw(GLFWwindow* window);
    //void loadTopLevel();
};

#endif // FCG_GAME
