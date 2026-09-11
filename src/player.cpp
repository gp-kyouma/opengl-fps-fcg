#include "player.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

#include "input.h"
#include "vec_aux.h"

#include "matrices.h"
#include "timer_aux.h"
#include "gamedata.h"

void Player::init()
{
    e_size = glm::vec3(1.0f,2.0f,1.0f);
    neck   = 0.5f;
    speed  = 3.0f;
    maxHealth = 100;
    health = maxHealth;

    kb_resist = 0.125;

    // (this assumes the existence of an initial weapon on key 1)
    currentWeapon = {1, 0};
    previousWeapon = {1, 0};

    wpnState = WPNSTATE_READY;

    weapons.clear();

    //this wpn list should not be hardcoded...
    std::vector<std::string> wpn_list = {"W_SWORD", "W_PISTOL", "W_SHOTGUN", "W_MINIGUN", "W_SNIPER"};
    for (auto key : wpn_list)
        addWeapon(key);
}

void Player::setView(float theta, float phi)
{
    view.x = cos(phi)*sin(theta);
    view.y = sin(phi);
    view.z = cos(phi)*cos(theta);
    view = normalize_vec3(view);
}

AABB Player::getHitbox()
{
    AABB result;
    glm::vec3 half = e_size / 2.0f;

    result.aabb_max = pos + half;
    result.aabb_min = pos - half;

    return result;
}

void Player::doPlayerMovement(float deltaTime)
{
    // jogador anda na direção onde está olhando
    glm::vec4 u,v,w;
    calculate_uvw(view,u,v,w,true);

    glm::vec4 movedir = glm::vec4(0.0f,0.0f,0.0f,0.0f);

    bool movementInput = g_KeyPressed[GLFW_KEY_W] ||
                         g_KeyPressed[GLFW_KEY_A] ||
                         g_KeyPressed[GLFW_KEY_S] ||
                         g_KeyPressed[GLFW_KEY_D];

    if (movementInput)
    {
        if (g_KeyPressed[GLFW_KEY_W])
        {
            movedir -= w;
        }
        if (g_KeyPressed[GLFW_KEY_S])
        {
            movedir += w;
        }
        if (g_KeyPressed[GLFW_KEY_A])
        {
            movedir -= u;
        }
        if (g_KeyPressed[GLFW_KEY_D])
        {
            movedir += u;
        }

        normalize_vec4_inplace(movedir);

        input_velocity = toVec3(movedir);
    }

    // jump mechanic
    const float jump_force = 4.5f; // ~1.0 unit jump height

    bool jumpFrame = false;
    if (g_KeyPressed[GLFW_KEY_SPACE] && grounded)
    {
        grounded = false;
        velocity.y = jump_force;
        jumpFrame = true;
    }

    // movement speed
    float speedMultiplier = 1.0f;
    if (getCurrentWeapon().has_effect(AIM_SLOWDOWN) && wpnAnimation > 0.0f && grounded)
        speedMultiplier = getCurrentWeapon().effects[AIM_SLOWDOWN].float_value;

    float trueSpeed = speed * speedMultiplier;
    if (movementInput)
        input_velocity *= trueSpeed;

    // variables for velocity falloff
    float v_decay = grounded ? Entity::friction : Entity::air_resist;
    float v_m = 0;
    glm::vec3 v_dir;

    // velocity falloff (input)
    if (!movementInput)
    {
        vec3_to_attributes(input_velocity, v_dir, v_m);

        v_m -= (v_decay * deltaTime);
        if (v_m < 0.0f)
            v_m = 0.0f;

        input_velocity = v_dir * v_m;
        v_m = 0;
    }

    // velocity falloff (horizontal, external)
    // (maybe this should also affect y? just buff jump height then... meh, not really worth it)
    glm::vec3 velocity_h = velocity;
    velocity_h.y = 0.0f;

    vec3_to_attributes(velocity_h, v_dir, v_m);

    v_m -= (v_decay * deltaTime);
    if (v_m < 0.0f)
        v_m = 0.0f;

    velocity_h = v_dir * v_m;

    velocity.x = velocity_h.x;
    velocity.z = velocity_h.z;

    // velocity falloff (gravity)
    if (!jumpFrame)
        velocity.y -= (Entity::gravity * deltaTime);

    pos += (input_velocity * deltaTime);
    pos += (velocity * deltaTime);

    //this is quite jank but it's ok. it's fine.
}

void Player::doWeaponAnimation(float deltaTime)
{
    float aim_speed = getCurrentWeapon().aim_speed;
    if (wpnState != WPNSTATE_DRAW && (g_RightMouseButtonPressed || (getCurrentWeapon().forced_aim && g_LeftMouseButtonPressed)))
        incrementTimer(wpnAnimation, deltaTime*aim_speed, 1.0f);
    else
        decrementTimer(wpnAnimation, deltaTime*aim_speed, 0.0f);
}

void Player::doDamageCooldown(float deltaTime)
{
    decrementTimer(dmgCooldown, deltaTime, 0.0f);
}

void Player::doWeaponSwitch()
{
    bool switched = g_NumberKeyPressed || g_KeyPressed[GLFW_KEY_Q] || g_LastScrollDirection != 0;
    if (!switched)
        return;

    int num_keys = weapons.size();

    //early exit if player has 0 weapons
    if (num_keys == 0)
        return;

    //early exit if player only has 1 weapon total
    if (num_keys == 1) {
        auto it = weapons.begin();
        if (it->second.size() == 1)
            return;
    }

    int map_index = currentWeapon.first;
    int vec_index = currentWeapon.second;
    std::vector<int> avaliable_keys;

    //get used keys
    avaliable_keys.reserve(num_keys);
    for (const auto& p : weapons) {
        //this should always be properly sorted
        avaliable_keys.push_back(p.first);
    }

    // number key press
    if (g_NumberKeyPressed)
    {
        g_NumberKeyPressed = false;//once per key press

        //if key exists
        if (std::binary_search(avaliable_keys.begin(), avaliable_keys.end(), g_LastNumberPressed))
        {
            if (g_LastNumberPressed != map_index)//pressed different key from current
            {
                previousWeapon = currentWeapon;
                //change map index, set vec index to 0
                currentWeapon.first = g_LastNumberPressed;
                currentWeapon.second = 0;
            }
            else//pressed same key as current
            {
                int wpns_on_key = weapons[map_index].size();

                //if more than one wpn on key
                if (wpns_on_key > 1)
                {
                    previousWeapon = currentWeapon;
                    //loop around current key vec
                    currentWeapon.second = (vec_index + 1) % wpns_on_key;
                }
                else
                    return;
            }
        }
        else
            return;
    }

    // weapon quickswap press
    else if (g_KeyPressed[GLFW_KEY_Q])
    {
        g_KeyPressed[GLFW_KEY_Q] = false;//once per key press

        if (currentWeapon != previousWeapon)
            std::swap(currentWeapon, previousWeapon);
        else
            return;
    }

    // mouse wheel scroll
    else if (g_LastScrollDirection != 0)
    {
        int wpns_on_key = weapons[map_index].size();

        previousWeapon = currentWeapon;

        //cases
        //1: scroll down, vec is at first
        //2: scroll up, vec is at max
        //3: else

        if (g_LastScrollDirection < 0 && vec_index == 0)
        {
            // get index of current key
            int index = std::distance(avaliable_keys.begin(), std::find(avaliable_keys.begin(), avaliable_keys.end(), map_index));
            // -1 to key (cycle), set vec to max
            index = (index + g_LastScrollDirection + num_keys) % num_keys;
            currentWeapon.first  = avaliable_keys[index];
            currentWeapon.second = weapons[currentWeapon.first].size() - 1;
        }
        else if (g_LastScrollDirection > 0 && vec_index == (wpns_on_key - 1))
        {
            // get index of current key
            int index = std::distance(avaliable_keys.begin(), std::find(avaliable_keys.begin(), avaliable_keys.end(), map_index));
            // +1 to key (cycle), set vec to 0
            index = (index + g_LastScrollDirection + num_keys) % num_keys;
            currentWeapon.first  = avaliable_keys[index];
            currentWeapon.second = 0;
        }
        else
        {
            // +scroll to vec
            currentWeapon.second = vec_index + g_LastScrollDirection;
        }

        g_LastScrollDirection = 0;//once per scroll
    }

    if (currentWeapon != previousWeapon)
    {
        //set wpn state
        wpnCooldown = getCurrentWeapon().drw_speed;
        wpnState = WPNSTATE_DRAW;
        wpnAnimation = 0.0f;
    }
}

void Player::addWeapon(std::string wpn_key)
{
    //todo:
    //if already has the weapon, add ammo (todo) for it instead
    //always add the ammo*

    Weapon wpn = g_GameData_Weapons[wpn_key];

    //clamp key slot
    if (wpn.key_slot < 0)
        wpn.key_slot = 0;
    else
    if (wpn.key_slot > 9)
        wpn.key_slot = 9;

    weapons[wpn.key_slot].push_back(wpn);
}

void Player::doWeaponCooldown(float deltaTime)
{
    decrementTimer(wpnCooldown, deltaTime, 0.0f);
    if (wpnCooldown == 0.0f)
        wpnState = WPNSTATE_READY;
}

void Player::update(float deltaTime)
{
    doPlayerMovement(deltaTime);
    doWeaponAnimation(deltaTime);
    doWeaponCooldown(deltaTime);
    doDamageCooldown(deltaTime);
    doWeaponSwitch();
}

Weapon Player::getCurrentWeapon()
{
    return (weapons[currentWeapon.first])[currentWeapon.second];
}

glm::vec3 Player::calculateWeaponPos()
{
    glm::vec3 result = pos;

    result.y += neck;

    glm::vec3 aim_displace = getCurrentWeapon().aim_displace;

    glm::vec4 u,v,w;
    calculate_uvw(view,u,v,w);

    glm::vec4 vertical_displace   = -v*aim_displace.y;
    glm::vec4 horizontal_displace = u*aim_displace.x *= (1.0f - wpnAnimation);
    glm::vec4 forward_displace    = -w*aim_displace.z;

    result += toVec3(vertical_displace) + toVec3(horizontal_displace) + toVec3(forward_displace);

    return result;
}

bool Player::fire(std::vector<Projectile> &new_projectiles)
{
    if (wpnState == WPNSTATE_READY &&
        g_LeftMouseButtonPressed &&
        (!getCurrentWeapon().forced_aim || wpnAnimation == 1.0f))
    {
        new_projectiles = getCurrentWeapon().fire(calculateWeaponPos(),view);

        wpnCooldown = getCurrentWeapon().cooldown;
        wpnState = WPNSTATE_COOLDOWN;
        return true;
    }
    else return false;
}

void Player::resetHealth()
{
    health = maxHealth;
}

void Player::takeDamage(int dmg)
{
    if (dmgCooldown == 0.0f)
    {
        health -= dmg;
        if (health < 0)
            health = 0;

        dmgCooldown = Player::baseDmgCooldown;
    }
}

bool Player::isDead()
{
    return (health == 0);
}

void Player::apply_kb(float power, glm::vec3 direction)
{
    if (dmgCooldown == 0.0f)
        velocity += direction * (power * (1.0f - kb_resist));
}
