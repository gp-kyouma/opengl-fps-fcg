#include "game.h"

#include <cstdio>
#include <cstdlib>

#include "matrices.h"

#include "collisions.h"
#include "input.h"
#include "draw.h"
#include "vec_aux.h"

// protótipos de funções auxiliares
void playerWithinLevel(Player &player, Level level);

void Game::Init()
{
    // LEVELS
    Level test_level;   // placeholder for test purposes
    test_level.levelLength = 20.0f;//10 goes up to 40
    test_level.levelWidth  = 20.0f;//8  goes up to 40
    test_level.player_pos  = glm::vec3(0.0f,6.0f,0.0f);
    test_level.player_view = glm::vec3(0.0f,0.0f,1.0f);

    Obstacle ob1, ob2, ob3, ob4, ob5, ob6;

    ob1.o_size = glm::vec3(1.0f,1.0f,1.0f);
    ob1.pos    = glm::vec3(3.0f,1.5f,4.0f);
    ob1.type   = OBSTACLE_BOX;

    ob2.o_size = glm::vec3(3.0f,0.5f,3.0f);
    ob2.pos    = glm::vec3(1.0f,3.5f,0.0f);
    ob2.type   = OBSTACLE_PLATFORM;

    ob3.o_size = glm::vec3(1.5f,0.5f,1.5f);
    ob3.pos    = glm::vec3(-0.5f,5.5f,-3.5f);
    ob3.type   = OBSTACLE_PLATFORM;

    ob4.o_size = glm::vec3(1.5f,0.5f,1.5f);
    ob4.pos    = glm::vec3(-1.5f,7.5f,-5.5f);
    ob4.type   = OBSTACLE_PLATFORM;

    ob5.o_size = glm::vec3(1.0f,8.0f,4.0f);
    ob5.pos    = glm::vec3(-2.0f,5.0f,-2.0f);
    ob5.type   = OBSTACLE_WALL;

    ob6.o_size = glm::vec3(1.0f,10.0f,4.0f);
    ob6.pos    = glm::vec3(-5.0f,6.0f,-2.0f);
    ob6.type   = OBSTACLE_WALL;

    test_level.obstacles.push_back(ob1);
    test_level.obstacles.push_back(ob2);
    test_level.obstacles.push_back(ob3);
    test_level.obstacles.push_back(ob4);
    test_level.obstacles.push_back(ob5);
    test_level.obstacles.push_back(ob6);

    level_queue.push(test_level);

    //placeholder
    obstacles = level_queue.front().obstacles;

    // PLAYER
    player.pos  = level_queue.front().player_pos;
    player.view = level_queue.front().player_view;

    player.p_size = glm::vec3(1.0f,2.0f,1.0f);
    player.neck   = 0.5f;

    player.grounded   = false;
    player.y_velocity = 0.0f;

    player.speed = 2.0f;

    player.health = player.maxHealth;
    player.dmgCooldown = 0.0f;
    player.wpnCooldown = 0.0f;
    player.currentWeapon  = 0; // should be melee

    // TEMPO
    prevTime = (float)glfwGetTime();
}

void Game::Update()
{
    // TEMPO
    // calcula próximo delta time
    updateDeltaTime();

    // PLAYER
    // faz a movimentação do jogador em função dos inputs do teclado
    player.setView(g_CameraTheta, g_CameraPhi);
    player.doPlayerMovement(deltaTime);

    // testa colisão com obstáculos
    player.grounded = false;

    bool col_result;
    glm::vec3 resolve;

    for (unsigned int i = 0; i < obstacles.size(); i++)
    {
        col_result = Collide(player.getAABB(),obstacles[i].getAABB(),resolve);
        if (col_result)
        {
            player.movePos(resolve);
            if (resolve.y != 0) // colisão vertical
            {
                if (resolve.y > 0) // colisão em cima
                {
                    player.grounded = true;
                    if (player.y_velocity < 0)
                        player.y_velocity = 0.0f;
                }
                else // colisão em baixo
                    if (player.y_velocity > 0)
                        player.y_velocity = 0.0f;
            }
        }
    }


    // testa colisão com a fase
    playerWithinLevel(player, level_queue.front());

    // todo: everything else xdddzs
}

void Game::Draw(GLFWwindow* window)
{
    // Aqui executamos as operações de renderização

    // (INICIALIZAÇÃO)

    // Definimos a cor do "fundo" do framebuffer como *cinza*.  Tal cor é
    // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
    // Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    //
    //           R     G     B     A
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
    // e também resetamos todos os pixels do Z-buffer (depth buffer).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pedimos para a GPU utilizar o programa de GPU criado (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(g_GpuProgramID);

    // Variáveis da câmera virtual
    glm::vec4 camera_pos  = Ponto(player.pos);  // Ponto "c", centro da câmera
    glm::vec4 view_vector = Vetor(player.view);
    glm::vec4 up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eixo Y global)

    camera_pos.y += player.neck; // ajusta altura da câmera

    // Computamos a matriz "View" utilizando os parâmetros do jogador para
    // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::mat4 view = Matrix_Camera_View(camera_pos, view_vector, up_vector);

    // Agora computamos a matriz de Projeção.
    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -60.0f; // Posição do "far plane"

    // Projeção Perspectiva.
    // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = 3.141592 / 3.0f;
    glm::mat4 projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // reseta repetição de texturas
    resetTextureRepeat();

    // (/INICIALIZAÇÃO)

    // (DESENHA OBJETOS)

    drawFloor(level_queue.front());
    drawWall(level_queue.front(), NORTH);
    drawWall(level_queue.front(), SOUTH);
    drawWall(level_queue.front(), EAST);
    drawWall(level_queue.front(), WEST);

    for (unsigned int i = 0; i < obstacles.size(); i++)
        drawObstacle(obstacles[i]);

    // todo: draw enemies
    // todo: draw projectiles

    // se g_ShowInfo = true, mostra as AABBs na tela
    if (g_ShowInfo)
    {
        drawAABB(player.getAABB());
        // e outras...
    }

    // Os objetos a seguir sempre serão desenhados na frente; desativa o z-buffer
    // (aka: as armas não atravessam paredes)
    glDisable(GL_DEPTH_TEST);

    //PLACEHOLDER
    drawWeapon(player, WPN_PISTOL, g_CameraTheta, g_CameraPhi);

    // Últimas coisas são desenhadas diretamente em NDC
    // Desativa matrizes de view e projeção
    view       = Matrix_Identity();
    projection = Matrix_Identity();

    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // Desenha mira
    drawCrosshair(g_ScreenRatio);

    // Desenha barra de vida
    drawBar((float)player.health, (float)player.maxHealth, g_ScreenRatio, "green", "yellow", "red", 0);

    // Desenha barra de cooldown da arma se estiver em cooldown
    if (player.wpnCooldown > 0)
        drawBar(player.wpnCooldown, 10.0f, g_ScreenRatio, "white", "white", "white", 1);//10.0f is placeholder

    // Reativa o z-buffer
    glEnable(GL_DEPTH_TEST);

    // (/DESENHA OBJETOS)

    // Imprimimos na tela informação sobre o número de quadros renderizados
    // por segundo (frames per second).
    TextRendering_ShowFramesPerSecond(window);

    // O framebuffer onde OpenGL executa as operações de renderização não
    // é o mesmo que está sendo mostrado para o usuário, caso contrário
    // seria possível ver artefatos conhecidos como "screen tearing". A
    // chamada abaixo faz a troca dos buffers, mostrando para o usuário
    // tudo que foi renderizado pelas funções acima.
    // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
    glfwSwapBuffers(window);
}

// Checa se o jogador está dentro dos limites da fase e ajusta sua posição se não estiver
// Como não é uma função entre dois corpos (AABB x AABB, etc) fica fora de collisions.cpp
void playerWithinLevel(Player &player, Level level)
{
    AABB hitbox = player.getAABB();

    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);

    glm::vec3 halfSize = (player.p_size / 2.0f);

    if (hitbox.aabb_min.x < -halfWidth)
    {
        player.pos.x = -halfWidth + halfSize.x;
    }
    else if (hitbox.aabb_max.x > halfWidth)
    {
        player.pos.x =  halfWidth - halfSize.x;
    }

    if (hitbox.aabb_min.z < -halfLength)
    {
        player.pos.z = -halfLength + halfSize.z;
    }
    else if (hitbox.aabb_max.z > halfLength)
    {
        player.pos.z =  halfLength - halfSize.z;
    }

    if (hitbox.aabb_min.y < level.levelFloor)
    {
        player.pos.y = level.levelFloor + halfSize.y;
        player.grounded = true;
        player.y_velocity = 0.0f;
    }
}

// Calcula o time step atual
void Game::updateDeltaTime()
{
    float currentTime = (float)glfwGetTime();
    deltaTime = currentTime - prevTime;
    prevTime  = currentTime;
}
