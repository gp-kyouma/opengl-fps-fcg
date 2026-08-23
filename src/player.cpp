#include "player.h"

#include <cstdio>
#include <cstdlib>

#include <algorithm>

#include "input.h"
#include "vec_aux.h"

#include "matrices.h"
#include "timer_aux.h"

void Player::setView(float theta, float phi)
{
    view.x = cos(phi)*sin(theta);
    view.y = sin(phi);
    view.z = cos(phi)*cos(theta);
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

    bool movementInput = g_WKeyPressed || g_AKeyPressed || g_SKeyPressed || g_DKeyPressed;

    if (movementInput)
    {
        if (g_WKeyPressed)
        {
            movedir -= w;
        }
        if (g_SKeyPressed)
        {
            movedir += w;
        }
        if (g_AKeyPressed)
        {
            movedir -= u;
        }
        if (g_DKeyPressed)
        {
            movedir += u;
        }

        normalize_vec4(movedir);
    }

    glm::vec3 offset = toVec3(movedir);

    // jump mechanic
    const float jump_force = 4.5f; // ~1.0 unit jump height

    if (g_SpaceBarKeyPressed && grounded)
    {
        grounded = false;
        velocity.y = jump_force;
    }
    else
    {
        velocity.y -= (Entity::gravity * deltaTime);
    }

    //maybe movement when not grounded is lesser?

    //cases for horizontal velocity
    //grounded: reduce by friction (high)
    //not grounded: reduce by air resistance (low)
    //applying this in an omnidirectional way instead of x/z...

    //MAYBE
    //have input velocity as its own thing
    //set to (input) when inputting, decay otherwise
    //add velocity and input velocity to get true velocity

    //the entirety of below might need to be rewritten?
    //regarding speed and such

    //also the gravity code doesn't stop at 0 but horizontal movement does need to stop at 0

    //movement code is hard...
    //TODO

    float speedMultiplier = 1.0f;
    if (getCurrentWeapon().effect == AIM_SLOWDOWN && wpnAnimation > 0.0f && grounded)
        speedMultiplier = 0.5f;

    float trueSpeed = speed * speedMultiplier;

    offset *= (trueSpeed * deltaTime);
    offset += (velocity * deltaTime);

    pos += offset;
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
    if (g_LastNumberPressed != currentWeapon && g_LastNumberPressed < (int)weapons.size())
    {
        currentWeapon = g_LastNumberPressed;
        wpnCooldown = getCurrentWeapon().drw_speed;
        wpnState = WPNSTATE_DRAW;
        wpnAnimation = 0.0f;
    }
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
    return weapons[currentWeapon];
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

        dmgCooldown = 0.5f;
    }
}

bool Player::isDead()
{
    return (health == 0);
}

void Player::apply_kb(float power, glm::vec3 direction)
{
    //TODO
}
