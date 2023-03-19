#include "vec_aux.h"

#include <math.h>

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

// Funções que calculam theta e phi a partir de um vetor
float getTheta(glm::vec3 v)
{
    return (float)atan2(v.x, v.z);

}

float getPhi(glm::vec3 v)
{
    return (float)asin(v.y);
}
