#include "vec_aux.h"

// Funções que transformam vec3s em pontos ou vetores para renderização (vec4s)
glm::vec4 Ponto(glm::vec3 v)
{
    return {v.x, v.y, v.z, 1.0f};
}

glm::vec4 Vetor(glm::vec3 v)
{
    return {v.x, v.y, v.z, 0.0f};
}

// Função que trasnforma um vec4 de volta em vec3
glm::vec3 toVec3(glm::vec4 v)
{
    return {v.x, v.y, v.z};
}
