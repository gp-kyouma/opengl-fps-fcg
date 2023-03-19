#ifndef FCG_COLLISIONS
#define FCG_COLLISIONS

// Headers da biblioteca GLM: criação de matrizes e vetores.
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

enum HitboxTypes
{
    BOX,
    SPHERE,
    RAY
};

// Estrutura que descreve uma AABB (axis-aligned bounding box) de um objeto
struct AABB
{
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;

    // funções auxiliares
    glm::vec3 getSize();
    glm::vec3 getCenter();
};

// Estrutura que descreve uma esfera que envolve um objeto
struct Sphere
{
    glm::vec3 center;
    float radius;
};

// Estrutura que descreve um raio partindo de um ponto
struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

/*
DECLARAÇÃO DE FUNÇÕES
*/

// AABB x AABB
bool Collide(AABB aabb1, AABB aabb2);

// AABB x AABB com parâmetro extra de saída
// (vetor de deslocamento que deve ser aplicado a aabb1 para resolver a colisão, se houver)
bool Collide(AABB aabb1, AABB aabb2, glm::vec3 &desloc);

// Sphere x AABB
bool Collide(Sphere sphere, AABB aabb);

// todo:
// Ray x AABB
// return closest distance as extra parameter maybe idk

#endif // FCG_COLLISIONS
