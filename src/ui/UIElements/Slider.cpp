#include "raylib.h"
#include "ui/UIElements/Slider.hpp"

namespace ui {
    Slider::Slider(std::string l, int centerX, int y, int width, int height, float minVal, float maxVal, float initialVal, Color barColor, Color handleColor)
        : minValue(minVal), maxValue(maxVal), currentValue(initialVal), barColor(barColor), handleColor(handleColor)
    {
        barBounds = {(float)(centerX - width / 2), (float)y, (float)width, (float)height};
        handleBounds = {(float)(centerX - handleRadius), (float)(y + height / 2 - handleRadius), handleRadius * 2, handleRadius * 2};

        label = std::make_unique<StaticText>(l, centerX, barBounds.y - fontSize - 5, WHITE, fontSize);
        valueLabel = std::make_unique<StaticText>(std::to_string((int)(currentValue * 100.0f)), barBounds.x + barBounds.width + 40, y, WHITE, fontSize);
    }

    void Slider::Update(float dt) {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, handleBounds)) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                dragging = true;
            }
        }

        if (dragging) {
            if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
                dragging = false;
            }
            else {
                handleBounds.x = mousePoint.x - handleRadius;
                UpdateValueFromHandle();
            }
        }

        UpdateHandlePosition();

        valueLabel->UpdateText(std::to_string((int)(currentValue * 100.0f)));
    }

    void Slider::Render() {
        DrawRectangleRec(barBounds, barColor);
        DrawCircle((int)(handleBounds.x + handleRadius), (int)(handleBounds.y + handleRadius), handleRadius, handleColor);
        label->Render();
        valueLabel->Render();
    }

    void Slider::UpdateHandlePosition() {
        float t = (currentValue - minValue) / (maxValue - minValue);
        handleBounds.x = barBounds.x + t * barBounds.width - handleRadius;
        handleBounds.y = barBounds.y + barBounds.height / 2 - handleRadius;
    }

    void Slider::UpdateValueFromHandle() {
        float t = (handleBounds.x + handleRadius - barBounds.x) / barBounds.width;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        currentValue = minValue + t * (maxValue - minValue);
    }

    float Slider::GetValue() const {
        return currentValue;
    }
}
