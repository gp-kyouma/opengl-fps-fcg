#ifndef FCG_INPUT
#define FCG_INPUT

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo s�o espec�ficos de C++

#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Cria��o de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Cria��o de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"
#include "utils.h"

/*
DECLARA��O DE FUN��ES
*/

// Fun��es callback para comunica��o com o sistema operacional e intera��o do
// usu�rio. Veja mais coment�rios nas defini��es das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

/*
DECLARA��O DE VARI�VEIS GLOBAIS
*/

// Raz�o de propor��o da janela (largura/altura). Veja fun��o FramebufferSizeCallback().
extern float g_ScreenRatio;

// "g_LeftMouseButtonPressed = true" se o usu�rio est� com o bot�o esquerdo do mouse
// pressionado no momento atual. Veja fun��o MouseButtonCallback().
extern bool g_LeftMouseButtonPressed;
extern bool g_RightMouseButtonPressed; // An�logo para bot�o direito do mouse
extern bool g_MiddleMouseButtonPressed; // An�logo para bot�o do meio do mouse

// "g_WKeyPressed = true" se o usu�rio est� com a tecla W
// pressionada no momento atual
// An�logo para as outras teclas
extern bool g_WKeyPressed;
extern bool g_AKeyPressed;
extern bool g_SKeyPressed;
extern bool g_DKeyPressed;
extern bool g_SpaceBarKeyPressed;

// Vari�vel que segura qual tecla num�rica o usu�rio pressionou por �ltimo, -1
extern int g_LastNumberPressed;

// Vari�veis que definem a c�mera em coordenadas esf�ricas, controladas pelo
// usu�rio atrav�s do mouse (veja fun��o CursorPosCallback()).
extern float g_CameraTheta; // �ngulo no plano ZX em rela��o ao eixo Z
extern float g_CameraPhi;   // �ngulo em rela��o ao eixo Y

// Vari�vel que controla se a informa��o extra ser� mostrada na tela.
extern bool g_ShowInfo;

#endif // FCG_INPUT
