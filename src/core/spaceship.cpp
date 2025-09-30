#include "raylib.h"
#include "core/config.h"
#include "core/spaceship.hpp"
#include <vector>
#include "core/ResourceManager.hpp"
#include "core/weapons.hpp"
#include <iostream>
#include "core/mathUtils.hpp"

const float initialHelth = 10.0f;
const int initialBulletLim = 5;
const int initialMaxEnergyShots = 1;
const int initialShipVel = 500; // pixels per second
const int initialBullVel = 530; // pixels per second
const Vector2 bulletSize = {15, 5}; // width, height.

Spaceship::Spaceship(const Texture2D& ship, Side side, Sound& shoot, Sound& hit, Texture2D& energyImage, Sound& energyShootingSound, std::unique_ptr<IController> ctrl)
    : shipImage(ship), shipSide(side), controller(std::move(ctrl)), shootingSound(shoot), hitSound(hit), energyShootSound(energyShootingSound) // These are initialized in the constructor initializer list, since they are references (&)
    {
    scale = 0.1f;
    Vector2 initalPos = side == Side::LEFT ? Vector2{10, 10} : Vector2{(float)WIDTH - 10 - ship.width * scale, (float)HEIGHT - 10 - ship.height * scale};
    shipRect = {initalPos.x, initalPos.y, (float)ship.width * scale, (float)ship.height * scale};
    shipVel = initialShipVel;
    bulletVel = initialBullVel;
    health = initialHelth;
    bulletLim = initialBulletLim;
    maxEnergyShots = initialMaxEnergyShots;
    score = 0;

    energyWeaponDamage = 2.5;
    bulletDamage = 1.0;

    energySprite = energyImage;
    rotation = side == Side::RIGHT ? 90.0f : 270.0f; 
}

bool Spaceship::InBounds(float newX, float newY) {
    if (shipSide == Side::LEFT) {
        return (newX >= 0 &&
                newY >= 0 &&
                newX + shipRect.width <= WIDTH / 2 &&
                newY + shipRect.height <= HEIGHT);
    }

    if (shipSide == Side::RIGHT) {
        return (newX >= WIDTH / 2 &&
                newY >= 0 &&
                newX + shipRect.width <= WIDTH &&
                newY + shipRect.height <= HEIGHT);
    }

    if (shipSide == Side::NONE) {
        return (newX >= 0 &&
                newY >= 0 &&
                newX + shipRect.width <= WIDTH && 
                newY + shipRect.height <= HEIGHT);
    }

    return false; 
}

float Spaceship::Accelerate(float current, float target, float& rate, float& dt) {
        if (current < target) {
            return std::min(current + rate * dt, target);
        } else if (current > target) {
            return std::max(current - rate * dt, target);
        }
        return current;
}

void Spaceship::ApplyMovement(const ControlState& state, float dt) {
    Vector2 dir = {state.moveX, state.moveY};

    // Add small deadzone since to ensure a zero movement state lerp only approaches zero in PlayerController 
    if (std::fabs(dir.x) < 0.01f) dir.x = 0;
    if (std::fabs(dir.y) < 0.01f) dir.y = 0;

    dir = math::NormalizeVec(dir);

    desiredVelocity = {dir.x * shipVel, dir.y * shipVel};

    if (dir.x !=0 || dir.y != 0) {
        velocity.x = Accelerate(velocity.x, desiredVelocity.x, accel, dt);
        velocity.y = Accelerate(velocity.y, desiredVelocity.y, accel, dt);
    } else {
        velocity.x = Accelerate(velocity.x, 0.0f, decel, dt);
        velocity.y = Accelerate(velocity.y, 0.0f, decel, dt);
    }

    float newX = shipRect.x + velocity.x * dt;
    float newY = shipRect.y + velocity.y * dt;


    if (InBounds(newX, shipRect.y)) {
        shipRect.x = newX;
    }
    else {
        velocity.x = 0;
    }
    
    if (InBounds(shipRect.x, newY)) {
        shipRect.y = newY;
    }
    else {
        velocity.y = 0;
    }
}

void Spaceship::Draw() {
    Rectangle source = {0, 0, (float)shipImage.width, (float)shipImage.height};
    Rectangle dest = {
        shipRect.x + shipRect.width / 2, 
        shipRect.y + shipRect.height / 2, 
        (float)shipImage.width * scale, 
        (float)shipImage.height * scale};
    
    // origin of rotation, about center of ship
    Vector2 origin = {dest.width/2, dest.height/2}; 
    DrawTexturePro(shipImage, source, dest, origin, rotation, WHITE);

    for (const auto& b : bullets) {
        DrawRectangleRec(b.rect, b.color);
    }

    for (auto& e : energyWeapons) {
        e.Render();
    }

    #if DEBUG
    Rectangle hitbox = GetHitBox();
    DrawRectangleLinesEx(hitbox, 2, GREEN);
    #endif
}

void Spaceship::ShootBullet() {
    if ((int)bullets.size() < bulletLim) {
        Bullet b;
        float bulletX = shipSide == Side::LEFT ? shipRect.x + shipRect.width : shipRect.x;
        b.pos = {bulletX, shipRect.y + shipRect.height / 2};
        b.speed = bulletVel;
        b.active = true;
        b.rect = {b.pos.x, b.pos.y, bulletSize.x, bulletSize.y};
        b.damage = bulletDamage;
        b.color = shipSide == Side::LEFT ? YELLOW : RED;
        bullets.push_back(b);
        PlaySound(shootingSound);
    }
}

void Spaceship::ShootEnergy(Spaceship& enemy) {

    if ((int)energyWeapons.size() < maxEnergyShots) {
        EnergyWeapon EW;
        EW.pos = {shipRect.x + shipRect.width/2, shipRect.y + shipRect.height/2};;
        EW.radius = 10;
        EW.active = true;
        EW.spriteRotation = 0;
        Vector2 toTarget = {
            enemy.shipRect.x + enemy.shipRect.width / 2 - EW.pos.x,
            enemy.shipRect.y + enemy.shipRect.height / 2 - EW.pos.y
        };
        float len = sqrt(toTarget.x*toTarget.x + toTarget.y*toTarget.y);
        EW.dir = {toTarget.x/len, toTarget.y/len};

        EW.vel = 300;
        EW.turnSpeed = 7.0f;
        EW.target = &enemy;
        EW.color = shipSide == Side::LEFT ? GREEN : RED;
        EW.timeEmmited = GetTime();
        EW.damage = energyWeaponDamage;
        EW.image = energySprite;
        energyWeapons.push_back(EW);
        PlaySound(energyShootSound);
    }
}

void Spaceship::HandleEnergyWeapon() {
    for (auto& e : energyWeapons) {
        if (e.active) {
            e.Update();
        }
    }

    energyWeapons.erase(
        std::remove_if(energyWeapons.begin(), energyWeapons.end(),
                       [](const EnergyWeapon& e){ return !e.active; }),
        energyWeapons.end()
    );
}

void Spaceship::HandleBullets() {
    float dt = GetFrameTime();

    for (auto &b : bullets) {
        if (b.active) {
            b.pos.x += (shipSide == Side::LEFT ? 1 : -1) * b.speed * dt;
            b.rect.x = b.pos.x;
            b.rect.y = b.pos.y;
            if (b.pos.x < -10 || b.pos.x > WIDTH) b.active = false;
        }
    }

    bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) {return !b.active; }),
            bullets.end()
        );
}

void Spaceship::HandleBeingShot(Spaceship& enemy) {
    // Handle Bullets
    for (auto& b : enemy.bullets) {
        if (CheckCollisionRecs(GetHitBox(), b.rect)) {
            health -= b.damage;
            b.active = false;
            if (IsDead()) enemy.score++;
            PlaySound(hitSound);
        }
    }

    // Handle Energy Weapons
    for (auto& e : enemy.energyWeapons) {
        if (CheckCollisionCircleRec(e.pos, e.radius, shipRect)){
            health -= e.damage;
            e.active = false;
            if (IsDead()) enemy.score++;
            PlaySound(hitSound);
        }
    }
}

void Spaceship::ApplyShooting(const ControlState& state, Spaceship& enemy) {
    if (state.shootBullet) {
        ShootBullet();
    }

    if (state.shootEnergy) {
        ShootEnergy(enemy);
    }

    HandleBullets();
    HandleEnergyWeapon();
}

void Spaceship::Update(float dt, Spaceship& enemy) {
    ControlState state = controller->GetState();

    ApplyMovement(state, dt);
    ApplyShooting(state, enemy);
    HandleBeingShot(enemy);
}

void Spaceship::Reset() {
    health = initialHelth;
    Vector2 initalPos = shipSide == Side::LEFT ? Vector2{10, 10} : Vector2{(float)WIDTH - 10 - shipImage.width * scale, (float)HEIGHT - 10 - shipImage.height * scale};
    shipRect = {initalPos.x, initalPos.y, (float)shipImage.width * scale, (float)shipImage.height * scale};
    // Clear bullets
    bullets.clear();
    std::vector<Bullet>().swap(bullets);
    energyWeapons.clear();
    std::vector<EnergyWeapon>().swap(energyWeapons);
}

bool Spaceship::IsDead() { 
    return health <= 0;
}

Rectangle Spaceship::GetHitBox() const{
    float shrinkFactor = 0.6f; // slightly smaller rectangle than shiprect
    float offsetX = (1.0f - shrinkFactor) / 2 * shipRect.width;
    float offsetY = (1.0f - shrinkFactor) / 2 * shipRect.height;

    return {
        shipRect.x + offsetX,
        shipRect.y + offsetY,
        shipRect.width * shrinkFactor,
        shipRect.height * shrinkFactor
    };
}
