#ifndef FLOATINGTEXT_HPP
#define FLOATINGTEXT_HPP

#include "raylib.h"
#include "ui/UIElement.hpp"
#include <string>

class FloatingText: public UIElement {
    public:
        FloatingText(std::string txt, int cX, int y, Color c, int fsize, int range);

        void Render() override;
        void Update(float dt) override;

        void UpdateText(std::string newTxt);

    private:
        std::string text;
        int centerX;
        int Y;
        Color color;
        int fontSize;
        int range;
        float vel;
        float initialY;
        float direction = -1.0f; // -1 for up, 1 for down
        float time = 0.0f;
};

#endif