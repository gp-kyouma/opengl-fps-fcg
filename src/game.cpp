#include "game.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "matrices.h"

#include "collisions.h"
#include "input.h"
#include "draw.h"
#include "vec_aux.h"
#include "timer_aux.h"
#include "bezier.h"

// protótipos de funções auxiliares
void entityWithinLevel(Entity &entity, Level level);

void Game::Init()
{
    // LEVELS
    // LOAD LEVEL LIST FROM FILE
    std::ifstream f_level_list("../../gamedata/level_list.json");

    if ( f_level_list.fail() )
    {
        std::cout << "level_list.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    json j_level_list = json::parse(f_level_list);
    f_level_list.close();

    auto level_list = j_level_list.get<std::vector<std::string>>();

    for (const auto& level_name : level_list) {
        // LOAD LEVEL DATA FROM FILE
        std::string filepath = "../../gamedata/levels/" + level_name + ".json";
        std::ifstream f_level(filepath);

        std::cout << "Loading level " << level_name << ".json..." << std::endl;

        if ( f_level.fail() )
        {
            std::cout << level_name << ".json failed to open" << std::endl;
            std::cout << "Error: " << strerror(errno) << std::endl;
            exit(1);
        }

        json j_level = json::parse(f_level);
        f_level.close();

        Level level = j_level.get<Level>();
        level.setLevelWalls();
        level_queue.push(level);
    }

    loadTopLevel();

    finalCutscene = false;

    // PLAYER
    player.init();

    // WEAPONS
    Weapon sword;
    Weapon pistol;
    Weapon shotgun;
    Weapon minigun;
    Weapon sniper;

    sword.type      = "SWORD";
    sword.dd_key    = "W_SWORD";
    sword.proj_type = PROJ_MELEE_INVISIBLE;
    sword.cooldown  = 0.5f;
    sword.drw_speed = 0.375f;
    sword.damage    = 25;
    sword.spread    = 0;
    sword.aim_speed = 4.0f;//0.25s
    sword.effect    = NO_EFFECT;

    pistol.type      = "PISTOL";
    pistol.dd_key    = "W_PISTOL";
    pistol.proj_type = PROJ_HITSCAN;
    pistol.cooldown  = 0.625f;
    pistol.drw_speed = 0.25f;
    pistol.damage    = 15;
    pistol.spread    = 5;
    pistol.aim_speed = 4.0f;//0.25s
    pistol.effect    = NO_EFFECT;

    shotgun.type      = "SHOTGUN";
    shotgun.dd_key    = "W_SHOTGUN";
    shotgun.proj_type = PROJ_HITSCAN;
    shotgun.cooldown  = 0.875f;
    shotgun.drw_speed = 0.5f;
    shotgun.damage    = 12;
    shotgun.spread    = 10;
    shotgun.aim_speed = 3.0f;//0.33s
    shotgun.effect    = SCATTER;

    minigun.type       = "MINIGUN";
    minigun.dd_key     = "W_MINIGUN";
    minigun.proj_type  = PROJ_HITSCAN;
    minigun.cooldown   = 0.10f;
    minigun.drw_speed  = 1.0f;
    minigun.damage     = 8;
    minigun.spread     = 35; // upgraded from 20
    minigun.aim_speed  = 1.0f;//1.0s
    minigun.effect     = AIM_SLOWDOWN;
    minigun.forced_aim = true;

    sniper.type      = "SNIPER";
    sniper.dd_key    = "W_SNIPER";
    sniper.proj_type = PROJ_BULLET;
    sniper.cooldown  = 1.125f;
    sniper.drw_speed = 1.0f;
    sniper.damage    = 50;
    sniper.spread    = 0;
    sniper.aim_speed = 2.0f;//0.5s
    sniper.effect    = AIM_SLOWDOWN;

    // these look a little off because of fov differences in first person
    sword.aim_displace   = glm::vec3(0.0f, 0.1f, 0.75f);    //lines up with blade...? also no horizontal displace
    pistol.aim_displace  = glm::vec3(0.3f, 0.125f, 0.45f);  //acceptable
    shotgun.aim_displace = glm::vec3(0.3f, 0.15f, 0.8f);    //actually good
    minigun.aim_displace = glm::vec3(0.3f, 0.125f, 0.4f);   //middling
    sniper.aim_displace  = glm::vec3(0.3f, 0.125f, 0.55f);  //decent

    player.weapons.push_back(sword);
    player.weapons.push_back(pistol);
    player.weapons.push_back(shotgun);
    player.weapons.push_back(minigun);
    player.weapons.push_back(sniper);

    // TEMPO
    prevTime = (float)glfwGetTime();
    totalTime = 0.0f;
}

void Game::Update()
{
    // TEMPO
    // calcula próximo delta time
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

    // faz o movimento da câmera pelo mouse
    player.setView(g_CameraTheta, g_CameraPhi);

    // faz a movimentação do jogador em função dos inputs do teclado
    // atualiza animação da arma
    // atualiza cooldown da arma
    // atualiza cooldown de dano
    // atualiza arma
    player.update(deltaTime);

    // CHECA SE PLAYER ESTÁ GROUNDED
    player.grounded = false;

    // testa colisão com fase (para grounded)
    entityWithinLevel(player, level_queue.front());

    // testa colisão com obstáculos 1 (para grounded)
    std::vector<Obstacle> obs_touching_player; // guarda os obstáculos que colidem com o jogador e precisam ser processados depois na ordem
    obs_touching_player.clear();

    for (unsigned int i = 0; i < obstacles.size(); i++)
    {
        glm::vec3 resolve;

        AABB player_hb = player.getHitbox();
        AABB obstcl_hb = obstacles[i].getHitbox();

        if (Collide(player_hb,obstcl_hb,resolve))
        {
            if (resolve.y != 0) // colisão vertical
            {
                player.pos += resolve;
                if (resolve.y > 0) // colisão em cima
                {
                    player.grounded = true;
                    if (player.velocity.y < 0)
                        player.velocity.y = 0.0f;
                }
                else // colisão em baixo
                    if (player.velocity.y > 0)
                        player.velocity.y = 0.0f;
            }
            else
                obs_touching_player.push_back(obstacles[i]);
        }
    }

    // testa colisão com obstáculos 2 (para step up)
    float step_up = 0.0f;
    bool ignore_step = false;

    auto obj = obs_touching_player.begin();
    while (obj != obs_touching_player.end())
    {
        glm::vec3 resolve;
        float step_limit = 0.25f;
        Obstacle ob = *obj;

        AABB player_hb = player.getHitbox();
        AABB obstcl_hb = ob.getHitbox();

        if (Collide(player_hb,obstcl_hb))
        {
            float height_diff = (obstcl_hb.aabb_max.y - player_hb.aabb_min.y);

            if (height_diff <= step_limit && player.grounded)
            {
                if (height_diff > step_up)
                    step_up = height_diff;
                obj = obs_touching_player.erase(obj);
            }
            else
            {
                obj++;
                ignore_step = true;//this fixes triple step, but sometimes it catches on the steps...? good enough
            }
        }
    }

    if (!ignore_step)
        player.pos.y += step_up;

    // testa colisão com obstáculos 3 (o resto dos resolves)
    for (unsigned int i = 0; i < obs_touching_player.size(); i++)
    {
        glm::vec3 resolve;

        AABB player_hb = player.getHitbox();
        AABB obstcl_hb = obs_touching_player[i].getHitbox();

        if (Collide(player_hb,obstcl_hb,resolve))
        {
            player.pos += resolve;
            //TODO KNOCKBACK RESOLVE
        }
    }

    // testa colisão com a fase (para manter in bounds)
    entityWithinLevel(player, level_queue.front());

    // testa se deve atirar
    std::vector<Projectile> new_projectiles;

    if (player.fire(new_projectiles))
    {
        projectiles.insert(projectiles.end(), new_projectiles.begin(), new_projectiles.end());
    }

    // PROJECTILES

    // vetor utilizado para contabilizar o dano levado por cada inimigo nesse frame
    std::vector<int> damageTaken(enemies.size(),0);

    unsigned int i_proj = 0;
    while (i_proj < projectiles.size())
    {
        projectiles[i_proj].update(deltaTime);

        // testa colisão com obstáculos
        for (unsigned int i = 0; i < obstacles.size(); i++)
        {
            float min_dist;
            bool result = projectiles[i_proj].collideAgainstEntity(obstacles[i],min_dist);

            // Pierce Projectile logic goes here
            if (result)
                switch (projectiles[i_proj].hit_type)
                {
                    case BOX:   // o único projétil que usa box é o melee então ignora (THIS IS UNSUSTAINABLE, TO FIX)
                        break;
                    case SPHERE:
                        projectiles[i_proj].lifespan = 0.0f;
                        break;
                    case RAY:
                        if (min_dist < projectiles[i_proj].e_size.z)
                        {
                            projectiles[i_proj].e_size.z = min_dist;
                        }
                        break;
                    default:
                        break;
                }
        }

        // testa colisão com inimigos
        float shortest_dist = 100.0f;
        int closest_enemy   = -1;

        for (unsigned int i = 0; i < enemies.size(); i++)
        {
            float min_dist;
            bool result = projectiles[i_proj].collideAgainstEntity(enemies[i],min_dist);

            // Pierce Enemy logic goes here
            if (result)
            {
                //TODO KNOCKBACK APPLY
                //USING PROJECTILE VIEW AS DIRECTION PROBABLY?
                //OR VECTOR FROM PROJ CENTER TO ENEMY?
                //DEPENDS, THIS MIGHT NEED TO BE A PROJ ATTRIBUTE
                //AKA: FOR EXPLOSIONS
                //LET IT STACK MAYBE?
                //THIS MIGHT FUCK UP THE BULLET BUT OK
                if (projectiles[i_proj].hit_type == RAY)
                {
                    if ((min_dist < projectiles[i_proj].e_size.z) && (min_dist < shortest_dist))
                    {
                        // isso é feito para que cada raio atinja somente o inimigo mais próximo
                        shortest_dist = min_dist;
                        closest_enemy = i;
                        projectiles[i_proj].e_size.z = min_dist;
                    }
                }
                else
                {
                    damageTaken[i] += (projectiles[i_proj].damage);
                }
            }
        }

        if (closest_enemy != -1)
        {
            damageTaken[closest_enemy] += (projectiles[i_proj].damage);
        }

        // testa colisão com fase
        // 1: projéteis atingem as paredes e chão/teto
        AABB* level_walls = level_queue.front().levelWalls;
        for (int i = 0; i < 6; i++)
        {
            float min_dist;
            bool result = projectiles[i_proj].collideAgainstAABB(level_walls[i],min_dist);
            if (result)
                switch (projectiles[i_proj].hit_type)
                {
                    case BOX:   // o único projétil que usa box é o melee então ignora (THIS IS UNSUSTAINABLE, TO FIX)
                        break;
                    case SPHERE:
                        projectiles[i_proj].lifespan = 0.0f;
                        break;
                    case RAY:
                        if (min_dist < projectiles[i_proj].e_size.z)
                        {
                            projectiles[i_proj].e_size.z = min_dist;
                        }
                        break;
                    default:
                        break;
                }
        }

        // 2: projéteis não contidos na fase são deletados
        // mais um failsafe do que qualquer outra coisa, o teste de verdade é o 1
        // (this is a little jank)
        AABB level_boundary = level_queue.front().getAABB();
        glm::vec3 tolerance = glm::vec3(1.0f,1.0f,1.0f);
        level_boundary.aabb_max += tolerance;
        level_boundary.aabb_min -= tolerance;

        if (!Collide(projectiles[i_proj].pos,level_boundary))//maybe use the generic check here??
        {
            projectiles[i_proj].lifespan = 0.0f;
        }

        // deleta projéteis "velhos"
        if (projectiles[i_proj].isDead())
            projectiles.erase(projectiles.begin()+i_proj);
        else
            i_proj++;
    }

    // ENEMIES

    // dá o dano contabilizado na fase dos projéteis
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
        // checa se inimigo vê jogador
        enemies[i].seesPlayer = enemies[i].isWithinRange(player.pos);

        if (enemies[i].seesPlayer)
            enemies[i].updateView(player.pos);

        // faz a movimentação do inimigo se o jogador estiver perto
        // inimigo é afetado por gravidade independente do jogador estar perto
        // atualiza cooldown de dano do inimigo
        enemies[i].update(deltaTime);

        // testa colisão com obstáculos
        enemies[i].grounded = false;

        bool col_result;
        glm::vec3 resolve;

        for (unsigned int j = 0; j < obstacles.size(); j++)
        {
            col_result = Collide(enemies[i].getHitbox(),obstacles[j].getHitbox(),resolve);
            if (col_result)
            {
                enemies[i].pos += resolve;
                if (resolve.y > 0) // colisão em cima
                {
                    enemies[i].grounded = true;
                    enemies[i].velocity.y = 0.0f;
                }
                //TODO KNOCKBACK RESOLVE
            }
        }

        // testa colisão com o jogador (dá dano)
        col_result = Collide(enemies[i].getHitbox(),player.getHitbox(),resolve);
        if (col_result)
        {
            enemies[i].pos +=  resolve/2.0f;
            player.pos     += -resolve/2.0f;

            //KNOCKBACK APPLY
            glm::vec3 kb_dir = player.pos - enemies[i].pos;
            kb_dir.y = 1.0f;
            kb_dir = glm::normalize(kb_dir);
            player.apply_kb(enemies[i].kb_power, kb_dir);

            player.takeDamage(enemies[i].damage);

            // checa se player está colidindo verticalmente com inimigo
            if (resolve.y < 0) // colisão em cima
            {
                player.grounded = true;
                if (player.velocity.y < 0)
                    player.velocity.y = 0.0f;
            }
        }

        // testa colisão com a fase
        entityWithinLevel(enemies[i], level_queue.front());
    }

    // update level timer
    levelTime += deltaTime;

    // liga noUpdate se estiver numa condição de fim de fase
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
    else if (enemies.empty())   // todos os inimigos derrotados, fim de nível
    {
        if (level_queue.size() == 1) // se for o último nível
        {
            totalTime += levelTime;
            initCutscene();
        }
        else if (g_EnterKeyPressed)
        {
            level_queue.pop();
            totalTime += levelTime;
            loadTopLevel();
        }
    }
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

    // Se estiver na cutscene, entra em DrawCutscene e aborta
    if (finalCutscene)
    {
        drawCutscene(window);
        return;
    }

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
    float nearplane = -0.1f;   // Posição do "near plane"
    float farplane  = -100.0f; // Posição do "far plane"

    // Projeção Perspectiva.
    // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float camera_fov = 3.141592 / 2.0f; //90graus
    glm::mat4 projection = Matrix_Perspective(camera_fov, g_ScreenRatio, nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    setViewMatrix(view);
    setProjectionMatrix(projection);

    // reseta repetição de texturas
    resetTextureRepeat(); // this FAILS the first frame it runs. god only knows why

    // reseta alpha stuff
    // these FAIL the first frame they run. god only knows why
    resetAlphaMask();
    resetAlphaValue();

    //reset misc flags
    setIgnoreLighting(false);
    setUseGouraud(false);
    setUseSphericalUV(false);

    // (/INICIALIZAÇÃO)

    // (DESENHA OBJETOS)

    drawFloor(level_queue.front());
    drawWall(level_queue.front(), NORTH);
    drawWall(level_queue.front(), SOUTH);
    drawWall(level_queue.front(), EAST);
    drawWall(level_queue.front(), WEST);

    //draws all entities (obstacles, projectiles, enemies)
    //transparent entities are drawn after opaque entities,
    //sorted by distance from camera (works well enough...)
    std::multimap<float,std::shared_ptr<Entity>> trans_entities;
    std::multimap<float,std::shared_ptr<Entity>>::reverse_iterator it;

    for (auto& item : obstacles)
    {
        //IF item.drawdata can have transparency, add to trans_entities[distance] to be drawn later, else draw now
        if (g_DrawDataMap[item.dd_key].may_use_alpha)
        {
            glm::vec3 closest_point = ClosestPoint(toVec3(camera_pos), item.getHitbox());
            float distance = glm::length(toVec3(camera_pos) - closest_point);
            trans_entities.insert({distance, std::make_shared<Obstacle>(item)});
        }
        else
            item.draw();
    }
    for (auto& item : projectiles)
    {
        //IF item.drawdata can have transparency, add to trans_entities[distance] to be drawn later, else draw now
        if (g_DrawDataMap[item.dd_key].may_use_alpha)
        {
            glm::vec3 closest_point = ClosestPoint(toVec3(camera_pos), item.getHitbox());
            float distance = glm::length(toVec3(camera_pos) - closest_point);
            trans_entities.insert({distance, std::make_shared<Projectile>(item)});
        }
        else
            item.draw();
    }
    for (auto& item : enemies)
    {
        //IF item.drawdata can have transparency, add to trans_entities[distance] to be drawn later, else draw now
        if (g_DrawDataMap[item.dd_key].may_use_alpha)
        {
            glm::vec3 closest_point = ClosestPoint(toVec3(camera_pos), item.getHitbox());
            float distance = glm::length(toVec3(camera_pos) - closest_point);
            trans_entities.insert({distance, std::make_shared<Enemy>(item)});
        }
        else
            item.draw();
    }
    for (it = trans_entities.rbegin(); it != trans_entities.rend(); ++it)
    {
        it->second->draw();
    }

    // se g_ShowInfo = true, mostra as AABBs na tela
    if (g_ShowInfo)
    {
        drawAABB(player.getHitbox());
        drawPosition(window, player);

        for (unsigned int i = 0; i < projectiles.size(); i++)
            if (projectiles[i].hit_type == BOX)
                drawAABB(projectiles[i].getHitbox());
            else if (projectiles[i].hit_type == RAY)
                drawPoint(projectiles[i].pos + projectiles[i].view * projectiles[i].e_size.z);//impact point

        for (unsigned int i = 0; i < enemies.size(); i++)
            drawAABB(enemies[i].getHitbox());

        //drawPoint(player.calculateWeaponPos());

        // weapons in 3rd person view
        Player fakeplayer[(int)player.weapons.size()];

        for (int i = 1; i < (int)player.weapons.size(); i++)
        {
            fakeplayer[i].pos  = glm::vec3(4.5f - (i * 1.5f),2.25f,-12.0f);
            fakeplayer[i].neck = 0.5f;
            fakeplayer[i].view = glm::vec3(0.0f,0.0f,1.0f);
            fakeplayer[i].wpnAnimation = 0.0f;
            fakeplayer[i].wpnCooldown  = 0.0f;
            fakeplayer[i].wpnState = WPNSTATE_READY;
            fakeplayer[i].weapons = {player.weapons[i]};
            fakeplayer[i].currentWeapon = 0;

            drawWeapon(fakeplayer[i],getTheta(fakeplayer[i].view),getPhi(fakeplayer[i].view));
            drawPoint(fakeplayer[i].calculateWeaponPos());
        }

        // bullet for checking spherical uv
        Projectile fakebullet;
        fakebullet.setProjectileData(PROJ_BULLET);
        fakebullet.pos = glm::vec3(0.0f,2.0f,-13.5f);
        fakebullet.view = glm::vec3(0.0f,0.0f,1.0f);
        drawAxes(fakebullet.pos,glm::vec4(1.0f,0.0f,0.0f,0.0f),glm::vec4(0.0f,1.0f,0.0f,0.0f),glm::vec4(0.0f,0.0f,1.0f,0.0f));
        drawProjectile(fakebullet);
    }

    // Resetamos todos os pixels do Z-buffer (depth buffer)
    // Assim a arma não atravessa as paredes
    glClear(GL_DEPTH_BUFFER_BIT);

    // Desenha arma
    if (!noUpdate)
    {
        // draw weapon with separate fov
        float weapon_fov = 3.141592 / 3.0f; //60graus
        projection = Matrix_Perspective(weapon_fov, g_ScreenRatio, nearplane, farplane);
        setProjectionMatrix(projection);

        drawWeapon(player, g_CameraTheta, g_CameraPhi);
    }

    // Os objetos a seguir sempre serão desenhados na frente; desativa o z-buffer
    glDisable(GL_DEPTH_TEST);

    // Últimas coisas são desenhadas diretamente em NDC
    // Desativa matrizes de view e projeção
    view       = Matrix_Identity();
    projection = Matrix_Identity();

    setViewMatrix(view);
    setProjectionMatrix(projection);

    // print level timer on top right
    drawTimer(window, levelTime, true);

    // Dá um flash vermelho na tela se tiver tomado dano
    if (player.dmgCooldown > 0)
        drawColorFade(COLOR_RED, player.dmgCooldown);

    // Desenha mensagem ao usuário se estiver morto/acabou fase
    if (player.isDead())
        drawBanner(g_ScreenRatio, "player_dead");
    else if (enemies.empty())
        drawBanner(g_ScreenRatio, "level_clear");
    else
    {
        // Desenha mira
        drawCrosshair(g_ScreenRatio);

        // Desenha barra de vida
        std::vector<glm::vec3> healthColors = {COLOR_GREEN, COLOR_GREEN, COLOR_YELLOW, COLOR_RED};
        drawBarNDC((float)player.health, (float)player.maxHealth, g_ScreenRatio, healthColors, 0);

        // Desenha barra de cooldown da arma se estiver em cooldown
        if (player.wpnCooldown > 0)
        {
            if (player.wpnState == WPNSTATE_COOLDOWN)
                drawBarNDC(player.wpnCooldown, player.getCurrentWeapon().cooldown, g_ScreenRatio, COLOR_WHITE, 1);
            else if (player.wpnState == WPNSTATE_DRAW)
                drawBarNDC(player.wpnCooldown, player.getCurrentWeapon().drw_speed, g_ScreenRatio, COLOR_WHITE, 1);
        }

        // print current player hp
        drawHealth(window, player);
    }

    /*
    if (g_ShowInfo)
    {
        drawColorCompare(g_ScreenRatio);
    }
    */

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

// Inicializa variáveis referentes à cutscene
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
    // Pontos de controle da(s) curva(s) bézier que a câmera irá percorrer
    const glm::vec3 bezier_pts[7] = {glm::vec3( 0.0f, 1.5f, 3.0f),
                                     glm::vec3( 5.5f, 1.5f, 3.0f),
                                     glm::vec3( 5.5f, 2.5f,-3.0f),
                                     glm::vec3( 0.0f, 2.5f,-3.0f),
                                     glm::vec3(-5.5f, 2.5f,-3.0f),
                                     glm::vec3(-5.5f, 3.5f, 5.0f),
                                     glm::vec3( 0.0f, 3.5f, 5.0f)};

    // Fade out no minotauro ao invés de shrink
    float minotaur_alpha = 1.0f;

    // Variáveis da câmera virtual
    // A posição será obtida por curva de bézier, e o view vector será obtido através de look-at
    glm::vec4 camera_pos;
    glm::vec4 view_vector;
    glm::vec4 up_vector = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eixo Y global)

    // Determina em que fase da cutscene estamos, dependendo do valor de cutsceneStep

    // Determina a posição da câmera
    if (cutsceneStep <= 2.0f)   // entre segundos 0 e 2, faz a primeira curva de bézier (primeira metade piecewise)
    {
        camera_pos = Ponto(CubicBezier(bezier_pts[0], bezier_pts[1], bezier_pts[2], bezier_pts[3], cutsceneStep/2.0f));
    }
    else if (cutsceneStep > 2.0f && cutsceneStep <= 4.0f) // entre segundos 2 e 4, faz a segunda curva de bézier (segunda metade piecewise)
    {
        camera_pos = Ponto(CubicBezier(bezier_pts[3], bezier_pts[4], bezier_pts[5], bezier_pts[6], cutsceneStep/2.0f - 1.0f));
    }
    else    // a partir do segundo 4, deixa a câmera parada no último ponto
    {
        camera_pos = Ponto(bezier_pts[6]);
    }

    // Encolhe o tamanho do minotauro a partir do segundo 5
    if (cutsceneStep > 5.0f && cutsceneStep <= 6.0f) // entre segundos 5 e 6
    {
        enemies[0].dmgCooldown = 1.0f;  // pra ele brilhar vermelho
        minotaur_alpha = 6.0f - cutsceneStep;
    }

    // Define o view vector
    // (câmera look-at)
    view_vector = Ponto(enemies[0].pos) - camera_pos;
    view_vector = view_vector / norm(view_vector);

    // Computamos a matriz "View" utilizando os parâmetros do jogador para
    // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::mat4 view = Matrix_Camera_View(camera_pos, view_vector, up_vector);

    // Agora computamos a matriz de Projeção.
    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;   // Posição do "near plane"
    float farplane  = -50.0f; // Posição do "far plane"

    // Projeção Perspectiva.
    // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = 3.141592 / 3.0f; //60graus
    glm::mat4 projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    setViewMatrix(view);
    setProjectionMatrix(projection);

    // reseta repetição de texturas
    resetTextureRepeat(); // this FAILS the first time it runs. god only knows why

    // reseta alpha stuff
    // these FAIL the first time they run. god only knows why
    resetAlphaMask();
    resetAlphaValue();

    //reset misc flags
    setIgnoreLighting(false);
    setUseGouraud(false);
    setUseSphericalUV(false);

    // Desenha o chão e as paredes da fase
    drawFloor(level_queue.front());
    drawWall(level_queue.front(), NORTH);
    drawWall(level_queue.front(), SOUTH);
    drawWall(level_queue.front(), EAST);
    drawWall(level_queue.front(), WEST);

    // Desenha o minotauro
    setAlphaValue(minotaur_alpha);
    drawEnemy(enemies[0]);
    resetAlphaValue();

    // Os objetos a seguir sempre serão desenhados na frente; desativa o z-buffer
    glDisable(GL_DEPTH_TEST);

    // Desenha tempo da fase final
    drawTimer(window, levelTime, true);

    // Se tiver acabado a cutscene
    if (cutsceneStep == 6.0f)
    {
        // Últimas coisas são desenhadas diretamente em NDC
        // Desativa matrizes de view e projeção
        view       = Matrix_Identity();
        projection = Matrix_Identity();

        setViewMatrix(view);
        setProjectionMatrix(projection);

        // Desenha mensagem de fim de jogo
        drawBanner(g_ScreenRatio, "game_clear");

        // Desenha tempo final de jogo
        DrawString(window, "FINAL TIME:", 0.0f, -0.625f, TEXTPOS_CENTER, TEXTPOS_CENTER, 4.0f, false, COLOR_WHITE);
        drawTimer(window, totalTime, false);
    }

    // Reativa o z-buffer
    glEnable(GL_DEPTH_TEST);

    // O framebuffer onde OpenGL executa as operações de renderização não
    // é o mesmo que está sendo mostrado para o usuário, caso contrário
    // seria possível ver artefatos conhecidos como "screen tearing". A
    // chamada abaixo faz a troca dos buffers, mostrando para o usuário
    // tudo que foi renderizado pelas funções acima.
    // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
    glfwSwapBuffers(window);

    // Incrementa cutsceneStep
    incrementTimer(cutsceneStep, deltaTime, 6.0f);
}

// Checa se a entidade está dentro dos limites da fase e ajusta sua posição se não estiver
// Como não é uma função entre dois corpos (AABB x AABB, etc) fica fora de collisions.cpp
void entityWithinLevel(Entity &entity, Level level)
{
    AABB hitbox = entity.getHitbox();

    float halfWidth  = (level.levelWidth /2.0f);
    float halfLength = (level.levelLength/2.0f);

    glm::vec3 halfSize = (entity.e_size / 2.0f);

    if (hitbox.aabb_min.x < -halfWidth)
    {
        entity.pos.x = -halfWidth + halfSize.x;
            //TODO KNOCKBACK RESOLVE
    }
    else if (hitbox.aabb_max.x > halfWidth)
    {
        entity.pos.x =  halfWidth - halfSize.x;
            //TODO KNOCKBACK RESOLVE
    }

    if (hitbox.aabb_min.z < -halfLength)
    {
        entity.pos.z = -halfLength + halfSize.z;
            //TODO KNOCKBACK RESOLVE
    }
    else if (hitbox.aabb_max.z > halfLength)
    {
        entity.pos.z =  halfLength - halfSize.z;
            //TODO KNOCKBACK RESOLVE
    }

    if (hitbox.aabb_min.y <= level.levelFloor) // constant grounded state when on floor, no matter what
    {
        entity.pos.y = level.levelFloor + halfSize.y;
        entity.grounded = true;
        entity.velocity.y = 0.0f;
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

    for (ObstacleData& ob : level_queue.front().obstacles)
    {
        obstacles.push_back(ob.buildObstacle());
    }

    for (EnemyData& enemy : level_queue.front().enemies)
    {
        enemies.push_back(enemy.buildEnemy());
    }

    // carrega/reseta dados do jogador
    player.pos  = level_queue.front().player_pos;
    player.view = level_queue.front().player_view;
    g_CameraPhi   = 0.0f;
    g_CameraTheta = 0.0f;

    player.grounded = false;
    player.velocity = glm::vec3(0.0f,0.0f,0.0f);
    player.input_velocity = glm::vec3(0.0f,0.0f,0.0f);

    player.dmgCooldown  = 0.0f;
    player.wpnCooldown  = 0.0f;
    player.wpnAnimation = 0.0f;

    player.wpnState = WPNSTATE_READY;

    player.resetHealth();

    levelTime = 0.0f;

    noUpdate = false;
}
