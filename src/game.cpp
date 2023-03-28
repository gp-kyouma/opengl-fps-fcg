#include "game.h"

#include <cstdio>
#include <cstdlib>

#include "matrices.h"

#include "collisions.h"
#include "input.h"
#include "draw.h"
#include "vec_aux.h"
#include "timer_aux.h"
#include "bezier.h"

// prot�tipos de fun��es auxiliares
void playerWithinLevel(Player &player, Level level);
void enemyWithinLevel(Enemy &enemy, Level level);

void Game::Init()
{
    // LEVELS
    Level level;

    //level.createTestLevel();

    //level_queue.push(level);

    level.createLevel1();

    level_queue.push(level);

    level.createLevel2();

    level_queue.push(level);

    level.createLevel3();

    level_queue.push(level);

    loadTopLevel();

    finalCutscene = false;

    // PLAYER

    player.p_size = glm::vec3(1.0f,2.0f,1.0f);
    player.neck   = 0.5f;
    player.speed  = 3.0f;

    player.currentWeapon  =  0; // melee

    // WEAPONS
    Weapon sword;
    Weapon pistol;
    Weapon shotgun;
    Weapon minigun;
    Weapon sniper;

    player.weapons.clear();

    sword.wpn_type  = WPN_SWORD;
    sword.proj_type = PROJ_MELEE_INVISIBLE;
    sword.cooldown  = 0.5f;
    sword.damage    = 25;
    sword.effect    = NO_EFFECT;

    pistol.wpn_type  = WPN_PISTOL;
    pistol.proj_type = PROJ_HITSCAN;
    pistol.cooldown  = 0.625f;
    pistol.damage    = 15;
    pistol.effect    = NO_EFFECT;

    shotgun.wpn_type  = WPN_SHOTGUN;
    shotgun.proj_type = PROJ_HITSCAN;
    shotgun.cooldown  = 0.875f;
    shotgun.damage    = 12; // 8 --> 12 deals actually decent damage
    shotgun.effect    = SCATTER_5;

    minigun.wpn_type  = WPN_MINIGUN;
    minigun.proj_type = PROJ_HITSCAN;
    minigun.cooldown  = 0.10f; // 0.20 --> 0.10 feels way better
    minigun.damage    = 8;
    minigun.effect    = RANDOM_SPREAD_01;

    sniper.wpn_type  = WPN_SNIPER;
    sniper.proj_type = PROJ_BULLET;
    sniper.cooldown  = 1.125f;
    sniper.damage    = 50;
    sniper.effect    = SLOWDOWN;

    player.weapons.push_back(sword);
    player.weapons.push_back(pistol);
    player.weapons.push_back(shotgun);
    player.weapons.push_back(minigun);
    player.weapons.push_back(sniper);

    // TEMPO
    prevTime = (float)glfwGetTime();
}

void Game::Update()
{
    // TEMPO
    // calcula pr�ximo delta time
    updateDeltaTime();

    // aborta o resto dos updates se finalCutscene == true
    if (finalCutscene)
        return;

    // CHECK LEVEL END
    checkLevelEnd();

    // aborta o resto dos updates se noUpdate == true
    if (noUpdate)
        return;

    // PLAYER
    // faz a movimenta��o do jogador em fun��o dos inputs do teclado
    player.setView(g_CameraTheta, g_CameraPhi);
    player.doPlayerMovement(deltaTime);

    // atualiza anima��o da arma
    player.doWeaponAnimation(deltaTime);

    // atualiza arma e cooldown da arma
    player.doWeaponCooldown(deltaTime);
    player.doWeaponSwitch();

    // atualiza cooldown de dano
    player.doDamageCooldown(deltaTime);

    // testa se deve atirar
    Projectile new_proj;
    bool shoot = player.fire(new_proj);
    if (shoot)
    {
        if (player.getCurrentWeapon().effect == SCATTER_5 || player.getCurrentWeapon().effect == RANDOM_SPREAD_01)
        {
            // cria novos proj�teis/faz altera��es
            // baseadas no proj�til que acabou de ser atirado:
            // obt�m o "sistema de coordenadas" do proj�til
            glm::vec4 v_up = glm::vec4(0.0f,1.0f,0.0f,0.0f);  // Vetor "up" fixado para apontar para o "c�u" (eixo Y global)

            glm::vec4 w = Vetor(-player.view);
            glm::vec4 u = crossproduct(v_up,w);

            w = w / norm(w);
            u = u / norm(u);

            glm::vec4 v = crossproduct(w,u);

            const float pi24 = 3.141592f / 24.0f;

            if (player.getCurrentWeapon().effect == SCATTER_5)
            {
                Projectile spread1 = new_proj;
                Projectile spread2 = new_proj;
                Projectile spread3 = new_proj;
                Projectile spread4 = new_proj;

                // rotaciona
                spread1.dir = toVec3(Matrix_Rotate( pi24*2, v) * Vetor(spread1.dir));
                spread2.dir = toVec3(Matrix_Rotate( pi24,   v) * Vetor(spread2.dir));
                spread3.dir = toVec3(Matrix_Rotate(-pi24,   v) * Vetor(spread3.dir));
                spread4.dir = toVec3(Matrix_Rotate(-pi24*2, v) * Vetor(spread4.dir));

                projectiles.push_back(spread1);
                projectiles.push_back(spread2);
                projectiles.push_back(spread3);
                projectiles.push_back(spread4);
            }
            else if (player.getCurrentWeapon().effect == RANDOM_SPREAD_01)
            {
                float offset_random = ((float)(rand() % 20) - (float)(rand() % 20)) / 100.0f;
                new_proj.dir = toVec3(Matrix_Rotate(offset_random*pi24, v) * Matrix_Rotate(offset_random*pi24, u) * Vetor(new_proj.dir));
            }
        }

        projectiles.push_back(new_proj);
    }

    // testa colis�o com obst�culos
    player.grounded = false;

    for (unsigned int i = 0; i < obstacles.size(); i++)
    {
        bool col_result;
        glm::vec3 resolve;

        col_result = Collide(player.getAABB(),obstacles[i].getAABB(),resolve);
        if (col_result)
        {
            player.movePos(resolve);
            if (resolve.y != 0) // colis�o vertical
            {
                if (resolve.y > 0) // colis�o em cima
                {
                    player.grounded = true;
                    if (player.y_velocity < 0)
                        player.y_velocity = 0.0f;
                }
                else // colis�o em baixo
                    if (player.y_velocity > 0)
                        player.y_velocity = 0.0f;
            }
        }
    }

    // testa colis�o com a fase
    playerWithinLevel(player, level_queue.front());

    // PROJECTILES

    // vetor utilizado para contabilizar o dano levado por cada inimigo nesse frame
    std::vector<int> damageTaken(enemies.size(),0);

    unsigned int i_proj = 0;
    while (i_proj < projectiles.size())
    {
        projectiles[i_proj].update(deltaTime);

        // testa colis�o com obst�culos
        for (unsigned int i = 0; i < obstacles.size(); i++)
        {
            float min_dist;
            bool result;
            if (projectiles[i_proj].lifespan > 0.0f)
                switch (projectiles[i_proj].hit_type)
                {
                    case BOX:   // o �nico proj�til que usa box � o melee ent�o ignora
                        break;
                    case SPHERE:
                        result = Collide(projectiles[i_proj].getHitsphere(),obstacles[i].getAABB());
                        if (result)
                            projectiles[i_proj].lifespan = 0.0f;
                        break;
                    case RAY:
                        result = Collide(projectiles[i_proj].getHitscan(),obstacles[i].getAABB(),projectiles[i_proj].p_size.z,min_dist);
                        if (result && min_dist < projectiles[i_proj].p_size.z)
                            projectiles[i_proj].p_size.z = min_dist;
                        break;
                }
        }

        // testa colis�o com inimigos
        float shortest_dist = 100.0f;
        int closest_enemy   = -1;

        for (unsigned int i = 0; i < enemies.size(); i++)
        {
            float min_dist;
            bool result;

            if (projectiles[i_proj].lifespan > 0.0f)
                switch (projectiles[i_proj].hit_type)
                {
                    case BOX:
                        result = Collide(projectiles[i_proj].getHitbox(),enemies[i].getAABB());
                        if (result)
                            damageTaken[i] += (projectiles[i_proj].damage);
                        break;
                    case SPHERE:
                        result = Collide(projectiles[i_proj].getHitsphere(),enemies[i].getAABB());
                        if (result)
                            damageTaken[i] += (projectiles[i_proj].damage);
                        break;
                    case RAY:
                        result = Collide(projectiles[i_proj].getHitscan(),enemies[i].getAABB(),projectiles[i_proj].p_size.z,min_dist);
                        if (result && min_dist < projectiles[i_proj].p_size.z)
                            if (min_dist < shortest_dist)
                            {
                                // isso � feito para que cada raio atinja somente o inimigo mais pr�ximo
                                shortest_dist = min_dist;
                                closest_enemy = i;
                                projectiles[i_proj].p_size.z = min_dist;
                            }
                        break;
                }
        }

        if (closest_enemy != -1)
            damageTaken[closest_enemy] += (projectiles[i_proj].damage);

        // deleta proj�teis "velhos"
        if (projectiles[i_proj].isDead())
            projectiles.erase(projectiles.begin()+i_proj);
        else
            i_proj++;
    }

    // ENEMIES

    // d� o dano contabilizado na fase dos proj�teis
    for (unsigned int i = 0; i < enemies.size(); i++)
        if (damageTaken[i] > 0)
            enemies[i].takeDamage(damageTaken[i]);

    // deleta inimigos cuja vida foi reduzida a 0
    unsigned int i_enemy = 0;
    while (i_enemy < enemies.size())
    {
        if (enemies[i_enemy].isDead())
            enemies.erase(enemies.begin()+i_enemy);
        else
            i_enemy++;
    }

    for (unsigned int i = 0; i < enemies.size(); i++)
    {
        // faz a movimenta��o do inimigo se o jogador estiver perto
        if (enemies[i].isWithinRange(player.pos))
        {
            enemies[i].updateView(player.pos);
            enemies[i].doEnemyMovement(deltaTime);
        }

        // inimgo � afetado por gravidade independente do jogador estar perto
        enemies[i].doEnemyGravity(deltaTime);

        // atualiza cooldown de dano do inimigo
        enemies[i].doDamageCooldown(deltaTime);

        // testa colis�o com obst�culos
        enemies[i].grounded = false;

        bool col_result;
        glm::vec3 resolve;

        for (unsigned int j = 0; j < obstacles.size(); j++)
        {
            col_result = Collide(enemies[i].getAABB(),obstacles[j].getAABB(),resolve);
            if (col_result)
            {
                enemies[i].pos += resolve;
                if (resolve.y > 0) // colis�o em cima
                {
                    enemies[i].grounded = true;
                    enemies[i].y_velocity = 0.0f;
                }
            }
        }

        // testa colis�o com a fase
        enemyWithinLevel(enemies[i], level_queue.front());

        // testa colis�o com o jogador (d� dano)
        col_result = Collide(enemies[i].getAABB(),player.getAABB(),resolve);
        if (col_result)
        {
            enemies[i].pos += resolve/2.0f;
            player.movePos(-resolve/2.0f);
            player.takeDamage(enemies[i].damage);
        }
    }

    // liga noUpdate se estiver numa condi��o de fim de fase
    if (player.isDead() || (enemies.empty()))
        noUpdate = true;

    // CHECK LEVEL END
    checkLevelEnd();
}

void Game::checkLevelEnd()
{
    if (player.isDead() && g_EnterKeyPressed)
    {
        loadTopLevel();
    }
    else if (enemies.empty())   // todos os inimigos derrotados, fim de n�vel
    {
        if (level_queue.size() == 1) // se for o �ltimo n�vel
        {
            initCutscene();
        }
        else if (g_EnterKeyPressed)
        {
            level_queue.pop();
            loadTopLevel();
        }
    }
}

void Game::Draw(GLFWwindow* window)
{
    // Aqui executamos as opera��es de renderiza��o

    // (INICIALIZA��O)

    // Definimos a cor do "fundo" do framebuffer como *cinza*.  Tal cor �
    // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto �:
    // Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
    //
    //           R     G     B     A
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
    // e tamb�m resetamos todos os pixels do Z-buffer (depth buffer).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pedimos para a GPU utilizar o programa de GPU criado (contendo
    // os shaders de v�rtice e fragmentos).
    glUseProgram(g_GpuProgramID);

    // Se estiver na cutscene, entra em DrawCutscene e aborta
    if (finalCutscene)
    {
        drawCutscene(window);
        return;
    }

    // Vari�veis da c�mera virtual
    glm::vec4 camera_pos  = Ponto(player.pos);  // Ponto "c", centro da c�mera
    glm::vec4 view_vector = Vetor(player.view);
    glm::vec4 up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "c�u" (eixo Y global)

    camera_pos.y += player.neck; // ajusta altura da c�mera

    // Computamos a matriz "View" utilizando os par�metros do jogador para
    // definir o sistema de coordenadas da c�mera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::mat4 view = Matrix_Camera_View(camera_pos, view_vector, up_vector);

    // Agora computamos a matriz de Proje��o.
    // Note que, no sistema de coordenadas da c�mera, os planos near e far
    // est�o no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;   // Posi��o do "near plane"
    float farplane  = -100.0f; // Posi��o do "far plane"

    // Proje��o Perspectiva.
    // Para defini��o do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = 3.141592 / 3.0f;
    glm::mat4 projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de v�deo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas s�o
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // reseta repeti��o de texturas
    resetTextureRepeat();

    // (/INICIALIZA��O)

    // (DESENHA OBJETOS)

    drawFloor(level_queue.front());
    drawWall(level_queue.front(), NORTH);
    drawWall(level_queue.front(), SOUTH);
    drawWall(level_queue.front(), EAST);
    drawWall(level_queue.front(), WEST);

    for (unsigned int i = 0; i < obstacles.size(); i++)
        drawObstacle(obstacles[i]);

    for (unsigned int i = 0; i < projectiles.size(); i++)
        drawProjectile(projectiles[i]);

    for (unsigned int i = 0; i < enemies.size(); i++)
        drawEnemy(enemies[i]);

    // se g_ShowInfo = true, mostra as AABBs na tela
    if (g_ShowInfo)
    {
        drawAABB(player.getAABB());

        for (unsigned int i = 0; i < projectiles.size(); i++)
            if (projectiles[i].hit_type == BOX)
                drawAABB(projectiles[i].getHitbox());

        for (unsigned int i = 0; i < enemies.size(); i++)
            drawAABB(enemies[i].getAABB());
    }

    // Resetamos todos os pixels do Z-buffer (depth buffer)
    // Assim a arma n�o atravessa as paredes
    glClear(GL_DEPTH_BUFFER_BIT);

    // Desenha arma
    if (!noUpdate)
        drawWeapon(player, player.getCurrentWeapon().wpn_type, g_CameraTheta, g_CameraPhi);

    // Os objetos a seguir sempre ser�o desenhados na frente; desativa o z-buffer
    glDisable(GL_DEPTH_TEST);

    // �ltimas coisas s�o desenhadas diretamente em NDC
    // Desativa matrizes de view e proje��o
    view       = Matrix_Identity();
    projection = Matrix_Identity();

    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // Desenha mensagem ao usu�rio se estiver morto/acabou fase
    if (player.isDead())
        drawBanner(g_ScreenRatio, "player_dead");
    else if (enemies.empty())
        drawBanner(g_ScreenRatio, "level_clear");
    else
    {
        // Desenha mira
        drawCrosshair(g_ScreenRatio);

        // Desenha barra de vida
        drawBar((float)player.health, (float)player.maxHealth, g_ScreenRatio, "green", "yellow", "red", 0);

        // Desenha barra de cooldown da arma se estiver em cooldown
        if (player.wpnCooldown > 0)
            drawBar(player.wpnCooldown, player.getCurrentWeapon().cooldown, g_ScreenRatio, "white", "white", "white", 1);
    }

    // Reativa o z-buffer
    glEnable(GL_DEPTH_TEST);

    // (/DESENHA OBJETOS)

    // Imprimimos na tela informa��o sobre o n�mero de quadros renderizados
    // por segundo (frames per second).
    TextRendering_ShowFramesPerSecond(window);

    // O framebuffer onde OpenGL executa as opera��es de renderiza��o n�o
    // � o mesmo que est� sendo mostrado para o usu�rio, caso contr�rio
    // seria poss�vel ver artefatos conhecidos como "screen tearing". A
    // chamada abaixo faz a troca dos buffers, mostrando para o usu�rio
    // tudo que foi renderizado pelas fun��es acima.
    // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
    glfwSwapBuffers(window);
}

// Inicializa vari�veis referentes � cutscene
void Game::initCutscene()
{
    finalCutscene = true;
    cutsceneStep  = 0.0f;

    projectiles.clear();
    obstacles.clear();

    EnemyData cutscene_minotaur;
    cutscene_minotaur.pos  = glm::vec3(0.0f, 3.5f, 0.0f);
    cutscene_minotaur.type = ENEMY_MINOTAUR;

    enemies.push_back(cutscene_minotaur.buildEnemy());
}

// Desenha elementos da cutscene
void Game::drawCutscene(GLFWwindow* window)
{
    // Pontos de controle da(s) curva(s) b�zier que a c�mera ir� percorrer
    const glm::vec3 bezier_pts[7] = {glm::vec3( 0.0f, 1.5f, 3.0f),
                                     glm::vec3( 5.5f, 1.5f, 3.0f),
                                     glm::vec3( 5.5f, 2.5f,-3.0f),
                                     glm::vec3( 0.0f, 2.5f,-3.0f),
                                     glm::vec3(-5.5f, 2.5f,-3.0f),
                                     glm::vec3(-5.5f, 3.5f, 5.0f),
                                     glm::vec3( 0.0f, 3.5f, 5.0f)};

    // Guarda o tamanho original do minotauro para futura refer�ncia
    const glm::vec3 minotaur_og_size = glm::vec3(2.5f,5.0f,1.5f);

    // Vari�veis da c�mera virtual
    // A posi��o ser� obtida por curva de b�zier, e o view vector ser� obtido atrav�s de look-at
    glm::vec4 camera_pos;
    glm::vec4 view_vector;
    glm::vec4 up_vector = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "c�u" (eixo Y global)

    // Determina em que fase da cutscene estamos, dependendo do valor de cutsceneStep

    // Determina a posi��o da c�mera
    if (cutsceneStep <= 2.0f)   // entre segundos 0 e 2, faz a primeira curva de b�zier (primeira metade piecewise)
    {
        camera_pos = Ponto(CubicBezier(bezier_pts[0], bezier_pts[1], bezier_pts[2], bezier_pts[3], cutsceneStep/2.0f));
    }
    else if (cutsceneStep > 2.0f && cutsceneStep <= 4.0f) // entre segundos 2 e 4, faz a segunda curva de b�zier (segunda metade piecewise)
    {
        camera_pos = Ponto(CubicBezier(bezier_pts[3], bezier_pts[4], bezier_pts[5], bezier_pts[6], cutsceneStep/2.0f - 1.0f));
    }
    else    // a partir do segundo 4, deixa a c�mera parada no �ltimo ponto
    {
        camera_pos = Ponto(bezier_pts[6]);
    }

    // Encolhe o tamanho do minotauro a partir do segundo 5
    if (cutsceneStep > 5.0f && cutsceneStep <= 6.0f) // entre segundos 5 e 6
    {
        enemies[0].dmgCooldown = 1.0f;  // pra ele brilhar vermelho
        enemies[0].model_size  = minotaur_og_size * (6.0f - cutsceneStep);
    }

    // Define o view vector
    // (c�mera look-at)
    view_vector = Ponto(enemies[0].pos) - camera_pos;
    view_vector = view_vector / norm(view_vector);

    // Computamos a matriz "View" utilizando os par�metros do jogador para
    // definir o sistema de coordenadas da c�mera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::mat4 view = Matrix_Camera_View(camera_pos, view_vector, up_vector);

    // Agora computamos a matriz de Proje��o.
    // Note que, no sistema de coordenadas da c�mera, os planos near e far
    // est�o no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;   // Posi��o do "near plane"
    float farplane  = -50.0f; // Posi��o do "far plane"

    // Proje��o Perspectiva.
    // Para defini��o do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = 3.141592 / 3.0f;
    glm::mat4 projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de v�deo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas s�o
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // reseta repeti��o de texturas
    resetTextureRepeat();

    // Desenha o ch�o e as paredes da fase
    drawFloor(level_queue.front());
    drawWall(level_queue.front(), NORTH);
    drawWall(level_queue.front(), SOUTH);
    drawWall(level_queue.front(), EAST);
    drawWall(level_queue.front(), WEST);

    // Desenha o minotauro
    drawEnemy(enemies[0]);

    // Se tiver acabado a cutscene
    if (cutsceneStep == 6.0f)
    {
        // Os objetos a seguir sempre ser�o desenhados na frente; desativa o z-buffer
        glDisable(GL_DEPTH_TEST);

        // �ltimas coisas s�o desenhadas diretamente em NDC
        // Desativa matrizes de view e proje��o
        glm::mat4 view       = Matrix_Identity();
        glm::mat4 projection = Matrix_Identity();

        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        // Desenha mensagem de fim de jogo
        drawBanner(g_ScreenRatio, "game_clear");

        // Reativa o z-buffer
        glEnable(GL_DEPTH_TEST);
    }

    // O framebuffer onde OpenGL executa as opera��es de renderiza��o n�o
    // � o mesmo que est� sendo mostrado para o usu�rio, caso contr�rio
    // seria poss�vel ver artefatos conhecidos como "screen tearing". A
    // chamada abaixo faz a troca dos buffers, mostrando para o usu�rio
    // tudo que foi renderizado pelas fun��es acima.
    // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
    glfwSwapBuffers(window);

    // Incrementa cutsceneStep
    incrementTimer(cutsceneStep, deltaTime, 6.0f);
}

// Checa se o jogador est� dentro dos limites da fase e ajusta sua posi��o se n�o estiver
// Como n�o � uma fun��o entre dois corpos (AABB x AABB, etc) fica fora de collisions.cpp
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

// Checa se o inimigo est� dentro dos limites da fase e ajusta sua posi��o se n�o estiver
// Como n�o � uma fun��o entre dois corpos (AABB x AABB, etc) fica fora de collisions.cpp
void enemyWithinLevel(Enemy &enemy, Level level)
{
    AABB hitbox = enemy.getAABB();

    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);

    glm::vec3 halfSize = (enemy.hitbox_size / 2.0f);

    if (hitbox.aabb_min.x < -halfWidth)
    {
        enemy.pos.x = -halfWidth + halfSize.x;
    }
    else if (hitbox.aabb_max.x > halfWidth)
    {
        enemy.pos.x =  halfWidth - halfSize.x;
    }

    if (hitbox.aabb_min.z < -halfLength)
    {
        enemy.pos.z = -halfLength + halfSize.z;
    }
    else if (hitbox.aabb_max.z > halfLength)
    {
        enemy.pos.z =  halfLength - halfSize.z;
    }

    if (hitbox.aabb_min.y < level.levelFloor)
    {
        enemy.pos.y = level.levelFloor + halfSize.y;
        enemy.grounded = true;
        enemy.y_velocity = 0.0f;
    }
}

// Calcula o time step atual
void Game::updateDeltaTime()
{
    float currentTime = (float)glfwGetTime();
    deltaTime = currentTime - prevTime;
    prevTime  = currentTime;
}

// Carrega os dados da fase na frente da fila de fases
void Game::loadTopLevel()
{
    obstacles.clear();
    enemies.clear();
    projectiles.clear();

    obstacles = level_queue.front().obstacles;

    for (unsigned int i = 0; i < level_queue.front().enemies.size(); i++)
    {
        enemies.push_back(level_queue.front().enemies[i].buildEnemy());
    }

    // carrega/reseta dados do jogador
    player.pos  = level_queue.front().player_pos;
    player.view = level_queue.front().player_view;
    g_CameraPhi   = 0.0f;
    g_CameraTheta = 0.0f;

    player.grounded   = false;
    player.y_velocity = 0.0f;

    player.dmgCooldown  = 0.0f;
    player.wpnCooldown  = 0.0f;
    player.wpnAnimation = 0.0f;

    player.resetHealth();

    noUpdate = false;
}
