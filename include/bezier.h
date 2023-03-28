#ifndef FCG_BEZIER
#define FCG_BEZIER

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
DECLARAÇÃO DE FUNÇÕES
*/

// Dado 4 pontos em 3D e um parâmetro t [0.0, 1.0],
// calcula a posição em t de um ponto na curva de Bézier cúbica definida pelos 4 pontos
glm::vec3 CubicBezier(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, float t);

#endif // FCG_BEZIER

