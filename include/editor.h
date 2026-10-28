#ifndef FCG_EDITOR
#define FCG_EDITOR

// Headers abaixo são específicos de C++
#include <string>
#include <vector>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#define GLM_FORCE_CTOR_INIT
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "player.h"
#include "level.h"

/*
STRUCTS/CLASSES
*/

struct ConfirmDialog
{
    bool pending = false;
    bool result  = false;

    void open();
    void render(const char* msg);
};

struct StairbuilderData
{
    AABB bounding;
    CardinalDirection direction;
    int orientation; // 0 = vertical, 1 = horizontal
    int numSteps;
    int thickness;
    bool clamp;
    std::string objType;

    bool checkValid();
    glm::vec3 getStepIncrement();
    AABB getInitialStep();
    std::vector<ObstacleData> buildFull();

private:
    AABB initial_vertical();
    AABB initial_horizontal();
};

struct Editor
{
    // informações de tempo
    float prevTime;
    float deltaTime;

    Player player;//aka the camera

    Level level;

    float viewTheta;
    float viewPhi;

    void updateDeltaTime();
    void Init();
    void Update();
    void DrawGUI(GLFWwindow* window);
    void DrawLevel(GLFWwindow* window);

    bool edited;

    void setEdited();

    std::vector<const char*> obstacleTypes;
    std::vector<const char*> enemyTypes;

    bool obstacleModal;
    bool enemyModal;
    int obstacleIndex;
    int enemyIndex;
    ObstacleData obstacleData;
    EnemyData enemyData;
    int obstacleTypeIndex;
    int enemyTypeIndex;

    bool stairbuilderModal;
    StairbuilderData stairbuilderData;
    int stairbuilderTypeIndex;

    void newLevel();
    void loadLevel(std::string filepath);
    void saveLevel(std::string filepath);
};

#endif // FCG_EDITOR
