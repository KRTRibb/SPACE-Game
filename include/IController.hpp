#ifndef ICONTROLLER_HPP
#define ICONTROLLER_HPP

struct ControlState {
    float moveX = 0.0f; // -1 = left, 1 = right
    float moveY = 0.0f; // -1 = up, 1 = down
    bool shootBullet = false;
    bool shootEnergy = false;
};

class IController {
    public:
        virtual ~IController() = default;
        virtual ControlState GetState() = 0;
};

#endif