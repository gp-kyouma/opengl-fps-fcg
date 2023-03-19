#ifndef FCG_RENDERER
#define FCG_RENDERER

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo s�o espec�ficos de C++
#include <stack>
#include <map>
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

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "matrices.h"

/*
STRUCTS/CLASSES
*/

// Estrutura que representa um modelo geom�trico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor l� o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
};

/*
DECLARA��O DE FUN��ES
*/

// Declara��o de fun��es utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declara��o de v�rias fun��es utilizadas em main().  Essas est�o definidas
// logo ap�s a defini��o de main() neste arquivo.
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constr�i representa��o de um ObjModel como malha de tri�ngulos para renderiza��o
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso n�o existam.
void LoadShadersFromFiles(); // Carrega os shaders de v�rtice e fragmento, criando um programa de GPU
void LoadTextureImage(const char* filename, std::string name); // Fun��o que carrega imagens de textura e guarda ID no dicion�rio
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Fun��o utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Fun��o para debugging

// Constr�i as arestas de um cubo para futura renderiza��o.
// (usado para desenhar AABBs)
void BuildCubeEdgesAndAddToVirtualScene();

// Constr�i as linhas de uma cruz para futura renderiza��o.
// (usado para desenhar a mira)
void BuildCrosshairAndAddToVirtualScene();

// Constr�i uma linha de comprimento 1, iniciando na origem e apontando para +z, para futura renderiza��o.
// (usado para desenhar hitscan)
void BuildLineAndAddToVirtualScene();

// Constr�i os tri�ngulos de um quadrado para futura renderiza��o.
// (usado para desenhar a barra de vida)
void BuildSquareAndAddToVirtualScene();

// Declara��o de fun��es auxiliares para renderizar texto dentro da janela
// OpenGL. Estas fun��es est�o definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Fun��es abaixo renderizam como texto na janela OpenGL algumas
// informa��es do programa.
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// 'Liga' a textura com nome correspondente no dicion�rio
void setDiffuseTexture(std::string name);
void setSpecularTexture(std::string name);

// determina repeti��o de textura
void setTextureRepeat(float u, float v);
void resetTextureRepeat();

/*
DECLARA��O DE VARI�VEIS GLOBAIS
*/

// Pilha que guardar� as matrizes de modelagem.
extern std::stack<glm::mat4>  g_MatrixStack;

// Dicion�rio (map) que associa um ID de textura a um nome
extern std::map<std::string, GLuint> g_TextureMap;

// Vari�veis que definem um programa de GPU (shaders). Veja fun��o LoadShadersFromFiles().
extern GLuint g_GpuProgramID;
extern GLint g_model_uniform;
extern GLint g_view_uniform;
extern GLint g_projection_uniform;
extern GLint g_object_id_uniform;
extern GLint g_bbox_min_uniform;
extern GLint g_bbox_max_uniform;
extern GLint g_repeat_uniform;
extern GLint g_ignore_lighting_uniform;
extern GLint g_use_gouraud_uniform;
extern GLint g_use_spherical_uv_uniform;
extern GLuint g_diffuse_texture_image_uniform;
extern GLuint g_specular_texture_image_uniform;

// N�mero de texturas carregadas pela fun��o LoadTextureImage()
extern GLuint g_NumLoadedTextures;

#endif // FCG_RENDERER
