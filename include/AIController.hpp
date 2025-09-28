#ifndef AICONTROLLER_HPP
#define AICONTROLLER_HPP

#include "IController.hpp"
#include "raylib.h"
#include <vector>
#include "spaceship.hpp"
#include "mathUtils.hpp"

enum class AIMode {
    Offensive,
    Defensive,
    Nuetral
};

enum class ThreatType {
    None,
    Bullet,
    Energy
};

class AIController : public IController {
private:
    Spaceship* self;
    Spaceship* enemy;
    float shootCooldown;
    float energyCooldown;
    float separationFromEnemy = 600;
    float dodgeCooldown;
    bool isDodging;
    Vector2 dodgeDir;
    ThreatType currentThreat;
    AIMode mode;

    void DecideMode();

    void HandleShooting(ControlState& state);
    void HandleMovement(ControlState& state);

    void HandleVerticalMovement(ControlState& state);
    void HandleHorizontalMovement(ControlState& state);
    Vector2 BlendMovement(const Vector2& modeMove, const Vector2& dodgeMove);

    void UpdateCooldowns();
    void UpdateEnemySeparation();

    float GetXDistanceToPlayer();
    float GetYDistanceToPlayer();

    bool IsBulletThreat(const Bullet& bullet);
    bool AnyBulletThreatAhead();
    bool IsBulletThreatAtY(const Bullet& bullet, float newY);

    bool AnyEnergyThreatAhead();

    void UpdateThreatType();

    bool ComputeDodgeForEnergy(Vector2& outDir);
    bool ComputeDodgeForBullet(Vector2& outDir);
    void UpdateDodgeDir();

    bool TryDodge(ControlState& state);


public:
    AIController(Spaceship* selfShip, Spaceship* enemyShip);

    ControlState GetState() override;
};

#endif
