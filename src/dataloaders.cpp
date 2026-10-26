#include "dataloaders.h"

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

// Headers locais, definidos na pasta "include/"
#include "scene.h"
#include "renderer.h"
#include "input.h"
#include "game.h"

#include "draw_data.h"
#include "gamedata.h"

#define ASSET_PATH "../../assets"
#define GAMEDATA_PATH "../../gamedata"

void LoadAllTextures()
{
    std::string path = ASSET_PATH;
    path += "/textures";
    std::string listpath = path + "/texture_list.json";

    // LOAD TEXTURE LIST FROM FILE
    std::ifstream f_tex_list(listpath);

    if ( f_tex_list.fail() )
    {
        std::cout << "texture_list.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::map<std::string,std::string> tex_list;

    try
    {
        json j_tex_list = json::parse(f_tex_list);
        tex_list = j_tex_list.get<std::map<std::string,std::string>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid texture list file: " << std::endl << e.what() << std::endl;
        f_tex_list.close();
        exit(1);
    }

    f_tex_list.close();

    for (const auto& tex : tex_list) {
        // LOAD TEXTURE FROM FILE
        std::string key = tex.first;
        std::string filepath = tex.second;
        LoadTextureImage(path + filepath, key);
    }
}

void LoadAllModels()
{
    std::string path = ASSET_PATH;
    path += "/models";
    std::string listpath = path + "/model_list.json";

    // LOAD MODEL LIST FROM FILE
    std::ifstream f_model_list(listpath);

    if ( f_model_list.fail() )
    {
        std::cout << "model_list.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::vector<std::string> model_list;

    try
    {
        json j_model_list = json::parse(f_model_list);
        model_list = j_model_list.get<std::vector<std::string>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid model list file: " << std::endl << e.what() << std::endl;
        f_model_list.close();
        exit(1);
    }

    f_model_list.close();

    for (const auto& modelname : model_list) {
        // LOAD MODEL FROM FILE
        std::string fullpath = path + modelname;
        ObjModel model(fullpath.c_str());
        ComputeNormals(&model);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    //hardcoded
    BuildCubeEdgesAndAddToVirtualScene();
    BuildCrosshairAndAddToVirtualScene();
    BuildLineAndAddToVirtualScene();
    BuildSquareAndAddToVirtualScene();
}

void LoadAllDrawData()
{
    std::string path = GAMEDATA_PATH;
    std::string listpath = path + "/drawdata_list.json";

    // LOAD DRAWDATA LIST FROM FILE
    std::ifstream f_dd_list(listpath);

    if ( f_dd_list.fail() )
    {
        std::cout << "drawdata_list.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::vector<std::string> dd_list;

    try
    {
        json j_dd_list = json::parse(f_dd_list);
        dd_list = j_dd_list.get<std::vector<std::string>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid drawdata list file: " << std::endl << e.what() << std::endl;
        f_dd_list.close();
        exit(1);
    }

    f_dd_list.close();

    for (const auto& dd_name : dd_list) {
        // LOAD DRAWDATA FROM FILE
        std::string filepath = path + "/drawdata/" + dd_name + ".json";
        std::ifstream f_dd(filepath);

        std::cout << "Loading drawdata " << dd_name << ".json..." << std::endl;

        if ( f_dd.fail() )
        {
            std::cout << dd_name << ".json failed to open" << std::endl;
            std::cout << "Error: " << strerror(errno) << std::endl;
            exit(1);
        }

        std::map<std::string,DrawData> dd;

        try
        {
            json j_dd = json::parse(f_dd);
            dd = j_dd.get<std::map<std::string,DrawData>>();
        }
        catch (const json::exception& e)
        {
            std::cout << "Invalid drawdata file: " << std::endl << e.what() << std::endl;
            f_dd.close();
            exit(1);
        }

        f_dd.close();

        // MERGE INTO GLOBAL DRAWDATA MAP
        // (for some reason merge() doesn't exist even though it should
        // so use insert() instead)
        g_GameData_DrawData.insert(dd.begin(), dd.end());
    }
}

void LoadAllGameData()
{
    std::string path = GAMEDATA_PATH;
    path += "/gameplay";

    // LOAD each FROM FILE
    //-------------------------------------------------------------------------------------
    // enemies.json
    std::ifstream f_enemy_map(path + "/enemies.json");

    std::cout << "Loading enemies.json..." << std::endl;

    if ( f_enemy_map.fail() )
    {
        std::cout << "enemies.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::map<std::string,Enemy> enemy_map;

    try
    {
        json j_enemy_map = json::parse(f_enemy_map);
        enemy_map = j_enemy_map.get<std::map<std::string,Enemy>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid enemies file: " << std::endl << e.what() << std::endl;
        f_enemy_map.close();
        exit(1);
    }

    f_enemy_map.close();

    g_GameData_Enemies.insert(enemy_map.begin(), enemy_map.end());

    //-------------------------------------------------------------------------------------
    // obstacles.json
    std::ifstream f_obstacle_map(path + "/obstacles.json");

    std::cout << "Loading obstacles.json..." << std::endl;

    if ( f_obstacle_map.fail() )
    {
        std::cout << "obstacles.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::map<std::string,Obstacle> obstacle_map;

    try
    {
        json j_obstacle_map = json::parse(f_obstacle_map);
        obstacle_map = j_obstacle_map.get<std::map<std::string,Obstacle>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid obstacles file: " << std::endl << e.what() << std::endl;
        f_obstacle_map.close();
        exit(1);
    }

    f_obstacle_map.close();

    g_GameData_Obstacles.insert(obstacle_map.begin(), obstacle_map.end());

    //-------------------------------------------------------------------------------------
    // projectiles.json
    std::ifstream f_projectile_map(path + "/projectiles.json");

    std::cout << "Loading projectiles.json..." << std::endl;

    if ( f_projectile_map.fail() )
    {
        std::cout << "projectiles.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::map<std::string,Projectile> projectile_map;

    try
    {
        json j_projectile_map = json::parse(f_projectile_map);
        projectile_map = j_projectile_map.get<std::map<std::string,Projectile>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid projectiles file: " << std::endl << e.what() << std::endl;
        f_projectile_map.close();
        exit(1);
    }

    f_projectile_map.close();

    g_GameData_Projectiles.insert(projectile_map.begin(), projectile_map.end());

    //-------------------------------------------------------------------------------------
    // weapons.json
    std::ifstream f_weapon_map(path + "/weapons.json");

    std::cout << "Loading weapons.json..." << std::endl;

    if ( f_weapon_map.fail() )
    {
        std::cout << "weapons.json failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::map<std::string,Weapon> weapon_map;

    try
    {
        json j_weapon_map = json::parse(f_weapon_map);
        weapon_map = j_weapon_map.get<std::map<std::string,Weapon>>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid weapons file: " << std::endl << e.what() << std::endl;
        f_weapon_map.close();
        exit(1);
    }

    f_weapon_map.close();

    g_GameData_Weapons.insert(weapon_map.begin(), weapon_map.end());
}
