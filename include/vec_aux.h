#ifndef FCG_VEC_AUX
#define FCG_VEC_AUX

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

//JSON
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

//VEC3 TO/FROM JSON
namespace glm {
    void to_json(json& j, const vec3& v);
    void from_json(const json& j, vec3& v);
} // namespace glm

// Funções que transformam vec3s em pontos ou vetores para renderização (vec4s)
glm::vec4 Ponto(glm::vec3 v);
glm::vec4 Vetor(glm::vec3 v);
// Função que trasnforma um vec4 de volta em vec3
glm::vec3 toVec3(glm::vec4 v);

// Funções que calculam theta e phi a partir de um vetor
float getTheta(glm::vec3 v);
float getPhi(glm::vec3 v);

// Função que calcula a distância entre 2 pontos
float distance(glm::vec3 p1, glm::vec3 p2);

// Função que calcula um sistema de coordenadas DE CÂMERA a partir de um vetor de view
void calculate_uvw(glm::vec3 view, glm::vec4& u, glm::vec4& v, glm::vec4& w, bool no_up = false);

// Função que normaliza um vec4 (não nulo) (inplace)
void normalize_vec4_inplace(glm::vec4& v);

// Função que normaliza um vec3 (não nulo)
glm::vec3 normalize_vec3(glm::vec3 v);

//Função que separa um vec3 arbitrário em direção e magnitude
void vec3_to_attributes(glm::vec3 v, glm::vec3& dir, float& m);

#endif // FCG_VEC_AUX
