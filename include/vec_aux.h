#ifndef FCG_VEC_AUX
#define FCG_VEC_AUX

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Fun��es que transformam vec3s em pontos ou vetores para renderiza��o (vec4s)
glm::vec4 Ponto(glm::vec3 v);
glm::vec4 Vetor(glm::vec3 v);
// Fun��o que trasnforma um vec4 de volta em vec3
glm::vec3 toVec3(glm::vec4 v);

// Fun��es que calculam theta e phi a partir de um vetor
float getTheta(glm::vec3 v);
float getPhi(glm::vec3 v);

#endif // FCG_VEC_AUX
