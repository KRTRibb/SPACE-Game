#include "raylib.h"
#include "spaceship.hpp"
#include "weapons.hpp"
#include <iostream>
#include <cmath>
#include "mathUtils.hpp"

void EnergyWeapon::Render() {
    Rectangle src = {0, 0, (float)image.width, (float)image.height};
    Rectangle dest = {pos.x, pos.y, (float)image.width, (float)image.height};

    Vector2 origin = {(float)image.width / 2, (float)image.height / 2};

    DrawTexturePro(image, src, dest, origin, spriteRotation, color);

    #if DEBUG
    DrawCircleV(pos, radius, Fade(color, 0.5f));
    #endif
}

void EnergyWeapon::UpdateDirection() {
    Vector2 toTarget = {
        target->shipRect.x + target->shipRect.width / 2 - pos.x,
        target->shipRect.y + target->shipRect.height / 2 - pos.y
    };

    toTarget = math::NormalizeVec(toTarget);

    // Blend current direction toward target direction
    float dt = GetFrameTime();
    float homingStrength = 5.0f; // tweak this higher/lower to adjust responsiveness

    dir.x += (toTarget.x - dir.x) * homingStrength * dt;
    dir.y += (toTarget.y - dir.y) * homingStrength * dt;

    dir = math::NormalizeVec(dir);
}

void EnergyWeapon::UpdateSpriteRotation() {
    spriteRotation = std::atan2(dir.y, dir.x) * 180 / M_PI;

    // extra 180 degrees rotation when is the sprite is being fired from the right hand ship
    if (target->shipSide == Side::LEFT) {
        spriteRotation += 180;
    }
}

void EnergyWeapon::Move() {
    float dt = GetFrameTime();

    pos.x += dir.x * vel * dt;
    pos.y += dir.y * vel * dt;
}

// Helper Function to update active and is homing status
void EnergyWeapon::UpdateStatus() {
    if (GetTime() - timeEmmited > homingDuration) {
        isHoming = false;
    }
    bool inBounds = pos.x < 0 || pos.x > WIDTH || pos.y < 0 || pos.y > HEIGHT;
    if (inBounds && !isHoming) {
        active = false;
    }

}

void EnergyWeapon::Update() {
    if (!active) return;

    UpdateStatus();

    if (isHoming){ // Homing only lasts for 5 seconds
        UpdateDirection();
        UpdateSpriteRotation();
    }
    Move();
}