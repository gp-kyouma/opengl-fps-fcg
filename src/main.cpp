//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   TRABALHO FINAL
//
//  Gabriel Peres da Silva
//  Joel Guilherme da Silva Sobral Filho
//  2022/2
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
#include <iostream>
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
#include "draw_data.h"

#define ASSET_PATH "../../assets"

void LoadAllTextures()
{
    std::string path = ASSET_PATH;
    path += "/textures";
    std::string listpath = path + "/texture_list.json";

    // LOAD TEXTURE LIST FROM FILE
    std::ifstream f_tex_list(listpath);

    if ( f_tex_list.fail() )
    {
        std::cout << "texture_list.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    json j_tex_list = json::parse(f_tex_list);
    auto tex_list = j_tex_list.get<std::map<std::string,std::string>>();

    for (const auto& tex : tex_list) {
        // LOAD TEXTURE FROM FILE
        std::string key = tex.first;
        std::string filepath = tex.second;
        LoadTextureImage(path + filepath, key);
    }

    f_tex_list.close();
}

void LoadAllModels()
{
    std::string path = ASSET_PATH;
    path += "/models";
    std::string listpath = path + "/model_list.json";

    // LOAD MODEL LIST FROM FILE
    std::ifstream f_model_list(listpath);

    if ( f_model_list.fail() )
    {
        std::cout << "model_list.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    json j_model_list = json::parse(f_model_list);
    auto model_list = j_model_list.get<std::vector<std::string>>();

    for (const auto& modelname : model_list) {
        // LOAD MODEL FROM FILE
        std::string fullpath = path + modelname;
        ObjModel model(fullpath.c_str());
        ComputeNormals(&model);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    f_model_list.close();

    //hardcoded
    BuildCubeEdgesAndAddToVirtualScene();
    BuildCrosshairAndAddToVirtualScene();
    BuildLineAndAddToVirtualScene();
    BuildSquareAndAddToVirtualScene();
}

void LoadAllDrawData()
{
    //todo
}

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    if (!glfwInit())
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
    // de pixels
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "HALL OF THE MINOTAUR v1.1.2", NULL, NULL);
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

    // Seta posição da janela no centro da tela
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glfwSetWindowPos(window, (mode->width/2) - (width/2), (mode->height/2) - (height/2));//30 pixels off

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

    LoadAllTextures();
    LoadAllModels();
    //LoadAllDrawData();//?

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

