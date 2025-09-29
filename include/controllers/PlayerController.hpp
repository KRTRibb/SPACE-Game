#ifndef PLAYERCOLTROLLER_HPP
#define PLAYERCOLTROLLER_HPP

#include "IController.hpp"
#include "raylib.h"
#include <vector>

class PlayerController : public IController {
    private:
        std::vector<int> moveKeys;
        int shootBulletKey, shootEnergyKey;

        float moveX = 0.0f;
        float moveY = 0.0f;
        float rampSpeed = 6.5f;

    public:
        PlayerController(const std::vector<int>& move, int shootBullet, int shootEnergy);

        ControlState GetState() override;
};

#endif