#include "controllers/PlayerController.hpp"
#include "raylib.h"
#include <vector>

PlayerController::PlayerController(const std::vector<int>& move, int shootBullet, int shootEnergy)
    : moveKeys(move), shootBulletKey(shootBullet), shootEnergyKey(shootEnergy) {}

ControlState PlayerController::GetState() {
    ControlState state;
    float targetX = 0.0f;
    float targetY = 0.0f;

    if (IsKeyDown(moveKeys[0])) targetY -= 1.0f;
    if (IsKeyDown(moveKeys[1])) targetY += 1.0f;
    if (IsKeyDown(moveKeys[2])) targetX -= 1.0f;
    if (IsKeyDown(moveKeys[3])) targetX += 1.0f;

    float dt = GetFrameTime();

    auto lerp = [&](float current, float target) {
        return current + (target - current) * rampSpeed * dt;
    };
    moveX = lerp(moveX, targetX);
    moveY = lerp(moveY, targetY);

    state.moveX = moveX;
    state.moveY = moveY;
    state.shootBullet = IsKeyPressed(shootBulletKey);
    state.shootEnergy = IsKeyPressed(shootEnergyKey);

    return state;
}