#ifndef SPACESHIP_HPP
#define SPACESHIP_HPP

#include <vector>
#include "raylib.h"
#include "config.h"
#include "ResourceManager.hpp"
#include "weapons.hpp"
#include "controllers/IController.hpp"

enum class Side {
    NONE,
    LEFT, 
    RIGHT
};

class Spaceship {
    public:
        Rectangle shipRect;
        Texture2D shipImage;
        float shipVel;
        float bulletVel;
        float health;
        int bulletLim;
        int maxEnergyShots;
        int score;
        Side shipSide;
        float energyWeaponTimer;
        float bulletDamage;
        float energyWeaponDamage;
        Texture2D energySprite;
        std::vector<Bullet> bullets;
        std::vector<EnergyWeapon> energyWeapons;
        std::unique_ptr<IController> controller;

        Vector2 velocity = {0, 0};
        Vector2 desiredVelocity = {0, 0};
        float accel = 600.0f;
        float decel = 12000.0f;
        
        Spaceship(const Texture2D& ship, Side side, Sound& shoot, Sound& hit, Texture2D& energyImage, Sound& energyShootingSound, std::unique_ptr<IController> ctrl);
        void ApplyMovement(const ControlState& state, float dt);
        float Accelerate(float current, float target, float& rate, float& dt);
        void Draw();
        void ShootBullet();
        void ShootEnergy(Spaceship& enemy);
        void HandleEnergyWeapon();
        void HandleBullets();
        void HandleBeingShot(Spaceship& enemy);
        void Update(float dt, Spaceship& enemy);
        bool IsDead();
        void Reset();
        void ApplyShooting(const ControlState& state, Spaceship& enemy);
        Rectangle GetHitBox() const;

    private:
        bool InBounds(float newX, float newY);
        Sound& shootingSound;
        Sound& hitSound;
        Sound& energyShootSound;
        float rotation;
        float scale;
};

#endif