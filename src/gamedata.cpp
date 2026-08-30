#include "gamedata.h"

// mapas dos dados base de cada inimigo, onde cada objeto acessa usando sua key
std::map<std::string, Enemy> g_GameData_Enemies;

// mapas dos dados base de cada obstáculo, onde cada objeto acessa usando sua key
std::map<std::string, Obstacle> g_GameData_Obstacles;

// mapas dos dados base de cada projétil, onde cada objeto acessa usando sua key
std::map<std::string, Projectile> g_GameData_Projectiles;

// mapas dos dados base de cada arma, onde cada objeto acessa usando sua key
std::map<std::string, Weapon> g_GameData_Weapons;
