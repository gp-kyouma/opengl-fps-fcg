#ifndef FCG_COLLISIONS
#define FCG_COLLISIONS

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
STRUCTS/CLASSES
*/

// Estrutura que descreve uma AABB (axis-aligned bounding box) de um objeto
struct AABB
{
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
};

// Estrutura que descreve uma esfera que envolve um objeto
struct BoundingSphere
{
    glm::vec3 center;
    float radius;
};

// ...e pelo menos mais uma

/*
DECLARAÇÃO DE FUNÇÕES
*/

// funções auxiliares de AABB
glm::vec3 getAABBSize(AABB aabb);
glm::vec3 getAABBCenter(AABB aabb);

// todo:
// AABB x AABB
// Sphere x AABB
// ...e pelo menos mais uma

#endif // FCG_COLLISIONS
