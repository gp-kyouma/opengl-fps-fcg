#ifndef FCG_INPUT
#define FCG_INPUT

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++

#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"
#include "utils.h"

/*
DECLARAÇÃO DE FUNÇÕES
*/

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

/*
DECLARAÇÃO DE VARIÁVEIS GLOBAIS
*/

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
extern float g_ScreenRatio;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
extern bool g_LeftMouseButtonPressed;
extern bool g_RightMouseButtonPressed; // Análogo para botão direito do mouse
extern bool g_MiddleMouseButtonPressed; // Análogo para botão do meio do mouse

// "g_KeyPressed[key] = true" se o usuário está com a tecla [key]
// pressionada no momento atual
extern std::map<int, bool> g_KeyPressed;

// tecla numérica pressionada nesse frame
extern bool g_NumberKeyPressed;

// Variável que segura qual tecla numérica o usuário pressionou por último
extern int g_LastNumberPressed;

// Variável que segura a última direção de movimento da rodinha do mouse
extern int g_LastScrollDirection;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()).
extern float g_CameraTheta; // Ângulo no plano ZX em relação ao eixo Z
extern float g_CameraPhi;   // Ângulo em relação ao eixo Y

// Variável que controla se a informação extra será mostrada na tela.
extern bool g_ShowInfo;

#endif // FCG_INPUT
