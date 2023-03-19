#ifndef FCG_RENDERER
#define FCG_RENDERER

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
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
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
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

// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
};

/*
DECLARAÇÃO DE FUNÇÕES
*/

// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constrói representação de um ObjModel como malha de triângulos para renderização
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso não existam.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
void LoadTextureImage(const char* filename, std::string name); // Função que carrega imagens de textura e guarda ID no dicionário
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Função para debugging

// Constrói as arestas de um cubo para futura renderização.
// (usado para desenhar AABBs)
void BuildCubeEdgesAndAddToVirtualScene();

// Constrói as linhas de uma cruz para futura renderização.
// (usado para desenhar a mira)
void BuildCrosshairAndAddToVirtualScene();

// Constrói uma linha de comprimento 1, iniciando na origem e apontando para +z, para futura renderização.
// (usado para desenhar hitscan)
void BuildLineAndAddToVirtualScene();

// Constrói os triângulos de um quadrado para futura renderização.
// (usado para desenhar a barra de vida)
void BuildSquareAndAddToVirtualScene();

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Funções abaixo renderizam como texto na janela OpenGL algumas
// informações do programa.
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// 'Liga' a textura com nome correspondente no dicionário
void setDiffuseTexture(std::string name);
void setSpecularTexture(std::string name);

// determina repetição de textura
void setTextureRepeat(float u, float v);
void resetTextureRepeat();

/*
DECLARAÇÃO DE VARIÁVEIS GLOBAIS
*/

// Pilha que guardará as matrizes de modelagem.
extern std::stack<glm::mat4>  g_MatrixStack;

// Dicionário (map) que associa um ID de textura a um nome
extern std::map<std::string, GLuint> g_TextureMap;

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
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

// Número de texturas carregadas pela função LoadTextureImage()
extern GLuint g_NumLoadedTextures;

#endif // FCG_RENDERER
