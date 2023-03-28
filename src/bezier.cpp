#include "bezier.h"

// Dado 4 pontos em 3D e um parâmetro t [0.0, 1.0],
// calcula a posição em t de um ponto na curva de Bézier cúbica definida pelos 4 pontos
glm::vec3 CubicBezier(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, float t)
{
    glm::vec3 c12 = p1 + t*p2 - t*p1;
    glm::vec3 c23 = p2 + t*p3 - t*p2;
    glm::vec3 c34 = p3 + t*p4 - t*p3;

    glm::vec3 c123 = c12 + t*c23 - t*c12;
    glm::vec3 c234 = c23 + t*c34 - t*c23;

    glm::vec3 c1234 = c123 + t*c234 - t*c123;

    return c1234;
}
