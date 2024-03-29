#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// gouraud
in vec3 gouraudDiffuse;
in vec3 gouraudSpecular;
in vec3 gouraudAmbient;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImageDiffuse;
uniform sampler2D TextureImageSpecular;

uniform vec2 repeat;

// se true, ignora cálculos de iluminação e usa textura diretamente
uniform bool ignoreLighting;

// usa modelo de iluminação Gouraud (vértice) se true
// usa Phong (pixel) se false
uniform bool useGouraud;

// Usa projeção esférica de texturas se true
uniform bool useSphericalUV;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term;

    // Termo ambiente
    vec3 ambient_term;

    // Termo especular utilizando o modelo de iluminação de BLINN-Phong
    vec3 blinn_phong_specular_term;

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    if (useSphericalUV)
    {
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        vec4 plinha = bbox_center + (position_model - bbox_center)/length(position_model - bbox_center);
        vec4 pvetor = plinha - bbox_center;

        float theta = atan(pvetor.x,pvetor.z);
        float phi   = asin(pvetor.y);

        U = (theta + M_PI) / (2*M_PI);
        V = (phi + M_PI_2) / M_PI;
    }
    else
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x * repeat.x;
        V = texcoords.y * repeat.y;
    }

    // Obtemos a refletância difusa a partir da leitura da imagem TextureImageDiffuse
    vec3 Kd = texture(TextureImageDiffuse, vec2(U,V)).rgb;

    // Obtemos a refletância especular a partir da leitura da imagem TextureImageSpecular
    vec3 Ks = texture(TextureImageSpecular, vec2(U,V)).rgb;

    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // distância da câmera ao ponto
    float camera_dist = length(camera_position - p);
    // decaimento de luz à base da distância
    float coeficiente = 1/(0.5*camera_dist + 0.05*pow(camera_dist,2));

    if (useGouraud) // usa modelo de Gouraud calculado em shader_vertex
    {
        lambert_diffuse_term      = Kd*gouraudDiffuse;
        ambient_term              = Kd*gouraudAmbient;
        blinn_phong_specular_term = Ks*gouraudSpecular;
    }
    else    // usa modelo de Phong
    {
        // Normal do fragmento atual, interpolada pelo rasterizador a partir das
        // normais de cada vértice.
        vec4 n = normalize(normal);

        // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
        vec4 l = //normalize(vec4(0.0,1.0,0.0,0.0));
                 normalize(camera_position - p);

        // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - p);

        // Vetor que define o sentido da reflexão especular ideal.
        vec4 r = -l + 2*n*dot(n,l);

        // Half vector (blinn-phong)
        vec4 h = normalize(v+l);

        // Expoente especular para o modelo de iluminação de Phong
        float q = 2.0;//placeholder
        float qlinha = 4*q; // conversão para blinn-phong

        // Espectro da fonte de iluminação
        vec3 I = vec3(1.0,1.0,1.0); // PREENCHA AQUI o espectro da fonte de luz

        // Espectro da luz ambiente
        vec3 Ia = vec3(0.08,0.08,0.08); // PREENCHA AQUI o espectro da luz ambiente

        // Termo difuso utilizando a lei dos cossenos de Lambert
        lambert_diffuse_term = Kd*I*max(0,dot(n,l));

        // Termo ambiente
        ambient_term = Kd*Ia;

        // Termo especular utilizando o modelo de iluminação de BLINN-Phong
        blinn_phong_specular_term = Ks*I*pow(max(0,dot(n,h)),qlinha);
    }

    if (ignoreLighting)
        color.rgb = Kd; // direto da textura
    else
        color.rgb = (lambert_diffuse_term + ambient_term + blinn_phong_specular_term) * coeficiente;

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}

