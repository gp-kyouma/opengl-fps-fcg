#ifndef FCG_COLLISIONS
#define FCG_COLLISIONS

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
STRUCTS/CLASSES
*/

enum CardinalDirection
{
    NORTH,  // z+
    SOUTH,  // z-
    EAST,   // x-
    WEST,   // x+
    UP,     // y+
    DOWN,   // y-
};

// Estrutura que descreve uma AABB (axis-aligned bounding box) de um objeto
struct AABB
{
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;

    // fun��es auxiliares
    glm::vec3 getSize();
    glm::vec3 getCenter();
};

// Estrutura que descreve uma esfera que envolve um objeto
struct Sphere
{
    glm::vec3 center;
    float radius;
};

// ...e pelo menos mais uma
// probably Ray

/*
DECLARA��O DE FUN��ES
*/

// AABB x AABB
bool Collide(AABB aabb1, AABB aabb2);

// AABB x AABB com par�metro extra de sa�da
// (vetor de deslocamento que deve ser aplicado a aabb1 para resolver a colis�o, se houver)
bool Collide(AABB aabb1, AABB aabb2, glm::vec3 &desloc);

// Sphere x AABB
bool Collide(Sphere sphere, AABB aabb);

// todo:
// ...e pelo menos mais uma
// probably Ray x AABB

#endif // FCG_COLLISIONS
