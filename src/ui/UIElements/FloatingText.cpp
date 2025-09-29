#include "ui/UIElements/FloatingText.hpp"
#include "raylib.h"
#include <cmath>

FloatingText::FloatingText(std::string txt, int cX, int y, Color c, int fsize, int range)
    : text(txt), centerX(cX), Y(y), color(c), fontSize(fsize), range(range) {
        vel = 5.0f;
        initialY = Y;
    };


void FloatingText::Render() {
    float textWidth = MeasureText(text.c_str(), fontSize);
    DrawText(text.c_str(), centerX - textWidth / 2, Y, fontSize, color);
}

void FloatingText::Update(float dt) {  
    time += dt;
    Y = initialY + range * std::sin(time * vel);
}

void FloatingText::UpdateText(std::string newTxt) {
    text = newTxt;
}