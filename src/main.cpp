//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 5
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers locais, definidos na pasta "include/"
#include "scene.h"
#include "renderer.h"
#include "input.h"
#include "game.h"

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Esconde o mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    LoadShadersFromFiles();

    /*
    carregamento de todos os assets
    */

    // Carregamos imagem para ser utilizada como textura
    LoadTextureImage("../../data/obstacles/stone_floor.jpg",         "floor");
    LoadTextureImage("../../data/obstacles/japanese_stone_wall.jpg", "wall");
    LoadTextureImage("../../data/obstacles/square_floor.jpg",        "platform");
    LoadTextureImage("../../data/obstacles/medieval_blocks.jpg",     "wall_obstacle");
    LoadTextureImage("../../data/obstacles/box.jpg",                 "box");

    LoadTextureImage("../../data/obstacles/japanese_stone_wall_spec_manual.jpg", "wall_spec");
    LoadTextureImage("../../data/obstacles/square_floor_spec_manual.jpg",        "platform_spec");
    LoadTextureImage("../../data/obstacles/medieval_blocks_spec.jpg",            "wall_obstacle_spec");

    LoadTextureImage("../../data/full_white.jpg",  "white");
    LoadTextureImage("../../data/full_grey.jpg",   "grey");
    LoadTextureImage("../../data/full_black.jpg",  "black");
    LoadTextureImage("../../data/full_green.jpg",  "green");
    LoadTextureImage("../../data/full_yellow.jpg", "yellow");
    LoadTextureImage("../../data/full_red.jpg",    "red");

    LoadTextureImage("../../data/weapons/bullet_silver.jpg", "silver");

    LoadTextureImage("../../data/weapons/pistol.png",       "pistol");
    LoadTextureImage("../../data/weapons/sniper.png",       "sniper");
    LoadTextureImage("../../data/weapons/sniper_spec.png",  "sniper_spec");
    LoadTextureImage("../../data/weapons/minigun.png",      "minigun");
    LoadTextureImage("../../data/weapons/minigun_spec.png", "minigun_spec");
    LoadTextureImage("../../data/weapons/sword.png",        "sword");
    LoadTextureImage("../../data/weapons/sword_spec.png",   "sword_spec");
    LoadTextureImage("../../data/weapons/shotgun.png",      "shotgun");

    LoadTextureImage("../../data/enemies/Minotaur_diffuse.jpg", "minotaur");
    LoadTextureImage("../../data/enemies/Pants_diffuse.jpg", "pants");

    ObjModel planemodel("../../data/obstacles/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel cubemodel("../../data/obstacles/cube.obj");
    ComputeNormals(&cubemodel);
    BuildTrianglesAndAddToVirtualScene(&cubemodel);

    ObjModel pistol("../../data/weapons/pistol.obj");
    ComputeNormals(&pistol);
    BuildTrianglesAndAddToVirtualScene(&pistol);

    ObjModel sniper("../../data/weapons/sniper.obj");
    ComputeNormals(&sniper);
    BuildTrianglesAndAddToVirtualScene(&sniper);

    ObjModel minigun("../../data/weapons/minigun.obj");
    ComputeNormals(&minigun);
    BuildTrianglesAndAddToVirtualScene(&minigun);

    ObjModel sword("../../data/weapons/sword.obj");
    ComputeNormals(&sword);
    BuildTrianglesAndAddToVirtualScene(&sword);

    ObjModel shotgun("../../data/weapons/shotgun.obj");
    ComputeNormals(&shotgun);
    BuildTrianglesAndAddToVirtualScene(&shotgun);

    ObjModel sphere("../../data/weapons/sphere.obj");
    ComputeNormals(&sphere);
    BuildTrianglesAndAddToVirtualScene(&sphere);

    ObjModel mino("../../data/enemies/Minotaur.obj");
    ComputeNormals(&mino);
    BuildTrianglesAndAddToVirtualScene(&mino);

    ObjModel skeleton("../../data/enemies/skeleton.obj");
    ComputeNormals(&skeleton);
    BuildTrianglesAndAddToVirtualScene(&skeleton);

    BuildCubeEdgesAndAddToVirtualScene();
    BuildCrosshairAndAddToVirtualScene();
    BuildLineAndAddToVirtualScene();
    BuildSquareAndAddToVirtualScene();

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUniform1i(g_ignore_lighting_uniform, false);
    glUniform1i(g_use_gouraud_uniform, false);
    glUniform1i(g_use_spherical_uv_uniform, false);

    srand(time(0));

    // GAME
    Game game;
    game.Init();

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        game.Update();
        game.Draw(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

