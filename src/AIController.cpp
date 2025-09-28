#include "AIController.hpp"
#include "raylib.h"
#include <cmath>

AIController::AIController(Spaceship* selfShip, Spaceship* enemyShip)
    : self(selfShip),
      enemy(enemyShip),
      shootCooldown(0.0f),
      energyCooldown(0.0f),
      dodgeCooldown(0.0f),
      isDodging(false),
      dodgeDir({0.0f, 0.0f}),
      currentThreat(ThreatType::None),
      mode(AIMode::Nuetral) {}


ControlState AIController::GetState() {
    ControlState state;

    DecideMode();
    UpdateCooldowns();
    UpdateEnemySeparation();
    
    HandleShooting(state);
    HandleMovement(state);
    Vector2 modeMove = {state.moveX, state.moveY};

     if (isDodging) {
        UpdateDodgeDir();
        Vector2 dodgeMove = dodgeDir;
        Vector2 blended = BlendMovement(modeMove, dodgeMove);
        state.moveX = blended.x;
        state.moveY = blended.y;
        UpdateThreatType();
    } else {
        TryDodge(state);
    }
    
    return state;
}

void AIController::DecideMode() {
    if (self->health < enemy->health * 0.8f) {
        mode = AIMode::Defensive;
    }
    else if (self->health > enemy->health) {
        mode = AIMode::Offensive;
    }
    else{
        mode = AIMode::Nuetral;
    }
}

void AIController::HandleShooting(ControlState& state) {
    switch (mode) {
        case AIMode::Offensive:
            if (shootCooldown <= 0.0f) {
                state.shootBullet = true;
                shootCooldown = 0.2f;
            }
            if (energyCooldown <= 0.0f && (int)self->energyWeapons.size() < self->maxEnergyShots) {
                state.shootEnergy = true;
                energyCooldown = 2.5;
            }
            break;
        case AIMode::Defensive:
            if (shootCooldown <= 0.0f) {
                state.shootBullet = true;
                shootCooldown = 0.5f;
            }

            if (energyCooldown <= 0.0f && (int)self->energyWeapons.size() < self->maxEnergyShots) {
                state.shootEnergy = true;
                energyCooldown = 6.0f;
            }
            break;
        case AIMode::Nuetral:
            if (shootCooldown < 0.0f) {
                state.shootBullet = true;
                shootCooldown = 0.4f;
            }
            if (energyCooldown < 0.0f && (int)self->energyWeapons.size() < self->maxEnergyShots) {
                state.shootEnergy = true;
                energyCooldown = 4.0f;
            }
            break;
    }
}

void AIController::HandleMovement(ControlState& state) {
    switch (mode) {
        case AIMode::Offensive:
            HandleVerticalMovement(state);
            HandleHorizontalMovement(state);
            break;
        case AIMode::Defensive:
            HandleVerticalMovement(state);
            HandleHorizontalMovement(state);
            break;
        case AIMode::Nuetral:
            HandleHorizontalMovement(state);
            HandleVerticalMovement(state);
            break;
    }
}

void AIController::HandleVerticalMovement(ControlState& state) {
    if (isDodging) return;

    float yDiff = GetYDistanceToPlayer();
    float deadZone = 20.0f;

    float dt = GetFrameTime();
    float newYCenter = self->shipRect.y + self->shipRect.height / 2
                      + (yDiff > 0 ? 1.0f : -1.0f) * self->shipVel * dt;

    for (auto& b : enemy->bullets) {
        if(IsBulletThreatAtY(b, newYCenter)) {
            state.moveY = 0.0f;
            return;
        }
    }

    if (std::fabs(yDiff) > deadZone) {
        state.moveY = (yDiff > 0 ? 1.0f : -1.0f);
    }
    else {
        state.moveY = 0.0f;
    }
}

void AIController::HandleHorizontalMovement(ControlState& state) {
    float buffer = GetRandomValue(50, 90);
    float minDistance = separationFromEnemy - buffer;
    float maxDistance = separationFromEnemy + buffer;

    float xDiff = GetXDistanceToPlayer();

    if (std::fabs(xDiff) < minDistance) {
        state.moveX = (self->shipSide == Side::RIGHT) ? 1.0f : -1.0f;
    }
    else if (std::fabs(xDiff) > maxDistance) {
        state.moveX = (self->shipSide == Side::RIGHT) ? -1.0f : 1.0f;
    }
    else {
        state.moveX = 0.0f;
    }
}


Vector2 AIController::BlendMovement(const Vector2& modeMove, const Vector2& dodgeMove) {
    float dodgeWeight;

    switch (mode) {
        case AIMode::Defensive:
            dodgeWeight = 1.0f;
            break;
        case AIMode::Nuetral:
            dodgeWeight = 0.75f;
            break;
        case AIMode::Offensive:
            dodgeWeight = 0.5f;
            break;
    }

    float modeWeight = 1.0f - dodgeWeight;

    Vector2 blended = {
        modeMove.x * modeWeight + dodgeMove.x * dodgeWeight,
        modeMove.y * modeWeight + dodgeMove.y * dodgeWeight
    };

    return math::NormalizeVec(blended);
}

void AIController::UpdateCooldowns() {
    float dt = GetFrameTime();
    shootCooldown -= dt;
    energyCooldown -= dt;
    dodgeCooldown -= dt;
}

void AIController::UpdateEnemySeparation() {
    switch (mode) {
        case AIMode::Offensive:
            separationFromEnemy = 600.0f;
            break;
        case AIMode::Defensive:
            separationFromEnemy = 800.0f;
            break;
        case AIMode::Nuetral:
            separationFromEnemy = 400.0;
            break;
    }
}

float AIController::GetXDistanceToPlayer() {
    float enemyCenterX = enemy->shipRect.x + enemy->shipRect.width / 2;
    float selfCenterX = self->shipRect.x + self->shipRect.width / 2;

    float xDiff = enemyCenterX - selfCenterX;

    return xDiff;
}

float AIController::GetYDistanceToPlayer() {
    float enemyCenterY = enemy->shipRect.y + enemy->shipRect.height /2;
    float selfCenterY = self->shipRect.y + self->shipRect.height / 2;
    float yDiff = enemyCenterY - selfCenterY;

    return yDiff;
}

bool AIController::IsBulletThreat(const Bullet& bullet) {
    if (!bullet.active) return false;

    float bx = bullet.rect.x + bullet.rect.width / 2;
    float by = bullet.rect.y + bullet.rect.height / 2;

    float sx = self->shipRect.x + self->shipRect.width / 2;
    float sy = self->shipRect.y + self->shipRect.height / 2;

    float vx = bullet.speed;

    float tImpact = (sx - bx) / vx;
    if (tImpact <= 0.0f) return false; // behind or already passed

    const float reactionWindow = 2.0f; 
    if (tImpact > reactionWindow) return false; 

    float predictedY = by;
    float verticalTolerance = self->shipRect.height * 0.6f; // tune
    if (std::fabs(predictedY - sy) > verticalTolerance) return false;

    return true;
}

bool AIController::AnyBulletThreatAhead() {
    float currentY = self->shipRect.y + self->shipRect.height / 2;
    for (auto& bullet : enemy->bullets) {
        if(IsBulletThreatAtY(bullet, currentY)) {
            return true;
        }
    }
    return false;
}

bool AIController::IsBulletThreatAtY(const Bullet& bullet, float Y) {
    if (!bullet.active) return false;

    float bx = bullet.rect.x + bullet.rect.width * 0.5f;
    float by = bullet.rect.y + bullet.rect.height * 0.5f;

    float sx = self->shipRect.x + self->shipRect.width * 0.5f;
    float verticalTolerance = self->shipRect.height * 0.5f;

    if (std::fabs(by - Y) > verticalTolerance) return false;

    float vx = bullet.speed;
    if (vx == 0.0f) return false;

    float tImpact = (sx - bx) / vx;

    const float nearWindow = 1.5f;
    return (tImpact > 0.0f && tImpact <= nearWindow);
}

bool AIController::AnyEnergyThreatAhead() {
    for (auto ew : enemy->energyWeapons) {
        if (ew.active) return true;
    }
    return false;
}


void AIController::UpdateThreatType() {
    if (currentThreat == ThreatType::Bullet && !AnyBulletThreatAhead()) {
        isDodging = false;
        currentThreat = ThreatType::None;
    }
    else if (currentThreat == ThreatType::Energy && !AnyEnergyThreatAhead()) {
        isDodging = false;
        currentThreat = ThreatType::None;
    }

}

bool AIController::ComputeDodgeForEnergy(Vector2& outDir) {
    Vector2 selfCenter = {
        self->shipRect.x + self->shipRect.width / 2,
        self->shipRect.y + self->shipRect.height / 2
    };

    float bestScore = -1e9;
    Vector2 bestDir = {0.0f, 0.0f};

    for (auto &ew : enemy->energyWeapons) {
        if(!ew.active || !ew.isHoming) continue;

        float timeAlive = GetTime() - ew.timeEmmited;
        // Reaction time delay
        if (timeAlive <= 0.2f) continue;

        // 30% chance not to dodge on a frame
        if (GetRandomValue(0, 100) > 70) continue;

        Vector2 toSelf = {selfCenter.x - ew.pos.x, selfCenter.y - ew.pos.y};

        float dist2 = toSelf.x*toSelf.x + toSelf.y*toSelf.y;
        if (dist2 < 1e-6) continue;
        Vector2 toSelfNorm = math::NormalizeVec(toSelf);

        float alignment = math::Dot(toSelfNorm, ew.dir);
        if (alignment < 0.3f) continue;

        float score = alignment / sqrtf(dist2);

        if (score > bestScore) {
            Vector2 left = {-ew.dir.y, ew.dir.x};
            Vector2 right = {ew.dir.y, -ew.dir.x};

            float cross = math::CrossZ(ew.dir, toSelfNorm);

            Vector2 chosen = (cross > 0.0f) ? left : right;

            Vector2 selfAfter = {selfCenter.x + chosen.x * 100.0f, selfCenter.y + chosen.y * 100.0f};

            bool insideY = (selfAfter.y + self->shipRect.height / 2  > 0.0f && selfAfter.y < (float)HEIGHT - self->shipRect.height / 2);
            bool insideX = (selfAfter.x + self->shipRect.width / 2 < (self->shipSide == Side::LEFT ? WIDTH / 2 : WIDTH) && selfAfter.x > (self->shipSide == Side::LEFT ? 0.0f : WIDTH / 2));

            if (!insideY || !insideX) {
                // If chosen side would go-offscreen, flip to other perp vector
                chosen = (chosen.x == left.x && chosen.y == left.y) ? right : left;
            }

            bestScore = score;
            bestDir = chosen;
        }
    }
    if (bestScore > -1e8f) {
        outDir = bestDir;
        return true;
    }
    return false;
}

bool AIController::ComputeDodgeForBullet(Vector2& outDir) {
    Vector2 selfCenter = {
        self->shipRect.x + self->shipRect.width / 2,
        self->shipRect.y + self->shipRect.height / 2
    };

    for (auto& b : enemy->bullets) {
        if (!b.active) continue;

        if (!IsBulletThreat(b)) continue;

        float by = b.rect.y + b.rect.height * 0.5f;
        float yDiff = selfCenter.y - by;

        outDir = {0.0f, (yDiff > 0.0f) ? 1.0f : -1.0f};

        float candidateY = selfCenter.y + outDir.y * 100;
        if (candidateY < self->shipRect.height / 2) outDir.y = 1.0f;
        if (candidateY > HEIGHT - self->shipRect.height / 2) outDir.y = -1.0f;

        return true;
    }

    return false;
}

void AIController::UpdateDodgeDir() {
    Vector2 newDir = dodgeDir;
    if (currentThreat == ThreatType::Energy) {
        Vector2 tmp;
        if (ComputeDodgeForEnergy(tmp)) newDir = tmp;
    }   else if (currentThreat == ThreatType::Bullet) {
        Vector2 tmp;
        if (ComputeDodgeForBullet(tmp)) newDir = tmp;
    }
    dodgeDir = math::NormalizeVec(newDir);
}

bool AIController::TryDodge(ControlState& state) {
    if (isDodging) {
        Vector2 newDir = dodgeDir;
        if (currentThreat == ThreatType::Energy) {
            Vector2 tmp;
            if (ComputeDodgeForEnergy(tmp)) newDir = tmp;
        } else if (currentThreat == ThreatType::Bullet) {
            Vector2 tmp;
            if (ComputeDodgeForBullet(tmp)) newDir = tmp;
        }

        dodgeDir = math::NormalizeVec(newDir);
        state.moveX = dodgeDir.x;
        state.moveY = dodgeDir.y;

        UpdateThreatType();
        return true;
    }

    if (dodgeCooldown > 0.0f) return false;

    Vector2 newDir;
    if (ComputeDodgeForEnergy(newDir)) {
        dodgeDir = newDir;
        isDodging = true;
        currentThreat = ThreatType::Energy;
        dodgeCooldown = 1.2f + GetRandomValue(-20, 20) / 100.0f;
        state.moveX = dodgeDir.x;
        state.moveY = dodgeDir.y;
        return true;
    }

    if (ComputeDodgeForBullet(newDir)) {
        dodgeDir = newDir;
        isDodging = true;
        currentThreat = ThreatType::Bullet;
        dodgeCooldown = 1.0f + GetRandomValue(-20, 20) / 100.0f;
        state.moveX = dodgeDir.x;
        state.moveY = dodgeDir.y;
        return true;
    }

    return false;
}