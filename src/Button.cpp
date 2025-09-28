#include "Button.hpp"
#include "raylib.h"

Button::Button(std::string txt, int centerX, int y, int fSize,
               Color c, Color hc, Color pc)
    : text(txt), color(c), hoverColor(hc), pressedColor(pc), currentColor(c), fontSize(fSize)
{
    int textWidth = MeasureText(text.c_str(), fontSize);
    int textHeight = fontSize;
    float padding = 10.0f;

    bounds = {
        (float)(centerX - textWidth / 2),
        (float)y,
        (float)textWidth,
        (float)(textHeight + padding)
    };
}

void Button::Update(float dt) {
    Vector2 mousePoint = GetMousePosition();
    clicked = false;

    if (CheckCollisionPointRec(mousePoint, bounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) currentColor = pressedColor;
        else currentColor = hoverColor;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clicked = true;
    } else {
        currentColor = color;
    }
}

void Button::Render() {
    int textWidth = MeasureText(text.c_str(), fontSize);
    int textHeight = fontSize;
    float textX = bounds.x + (bounds.width - textWidth) / 2;
    float textY = bounds.y + (bounds.height - textHeight) / 2;

    DrawRectangleRec(bounds, currentColor);
    DrawText(text.c_str(), (int)textX, (int)textY, fontSize, WHITE);
}

bool Button::WasClicked() const {
    return clicked;
}
