#ifndef WEAPONS_HPP
#define WEAPONS_HPP

#include "raylib.h"

class Spaceship;

struct Bullet {
    Vector2 pos;
    float speed;
    bool active;
    Rectangle rect;
    Color color;
    float damage;
};

struct EnergyWeapon {
    Vector2 pos; // Position of center of circle hitbox
    float radius; // Radius of circle hitbox
    bool active = true; // false when not homing AND off screen
    bool isHoming = true; // false when no longer hominh
    float spriteRotation = 0; // Clockwise rotation
    Vector2 dir;
    float vel;
    Spaceship* target;
    Color color;
    float turnSpeed;
    float timeEmmited;
    float homingDuration = 4;
    float damage;
    float distanceToTarget;
    Texture2D image;
    float homingStrength = 5;

    void Render();

    void UpdateDirection();

    void UpdateSpriteRotation();

    void Move();

    void Update();

    void UpdateStatus();
};

#endif