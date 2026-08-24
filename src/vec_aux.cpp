#include "vec_aux.h"

#include <math.h>

#include "matrices.h"

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

// Função que calcula a distância entre 2 pontos
float distance(glm::vec3 p1, glm::vec3 p2)
{
    return norm(Ponto(p2) - Ponto(p1));
}

// Função que calcula um sistema de coordenadas DE CÂMERA a partir de um vetor de view
void calculate_uvw(glm::vec3 view, glm::vec4& u, glm::vec4& v, glm::vec4& w, bool no_up)
{
    glm::vec4 v_up = glm::vec4(0.0f,1.0f,0.0f,0.0f);  // Vetor "up" fixado para apontar para o "céu" (eixo Y global)

    w = Vetor(-view);//Hmm.
    u = crossproduct(v_up,w);

    if (no_up)
    {
        w.y = 0.0f;
        u.y = 0.0f;
    }

    normalize_vec4(w);
    normalize_vec4(u);

    v = crossproduct(w,u);
}

// Função que normaliza um vec4 (não nulo)
void normalize_vec4(glm::vec4& v)
{
    float norm_v = norm(v);
    if (norm_v != 0)
        v = v / norm_v;
}

void vec3_to_attributes(glm::vec3 v, glm::vec3& dir, float& m)
{
    if (glm::length(v) != 0.0f)
    {
        dir = glm::normalize(v);
        m = glm::length(v);
    }
}
