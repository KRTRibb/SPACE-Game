#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "raylib.h"
#include "ui/UIElement.hpp"
#include "StaticText.hpp"
#include <string>

namespace ui {
    class Slider : public UIElement {
        public:
            Slider(std::string l, int centerX, int y, int width, int height, float minVal, float maxVal, float initialVal, Color barColor, Color handleColor);

            void Update(float dt) override;
            void Render() override;
            float GetValue() const;
        private:
            Rectangle barBounds;
            Rectangle handleBounds;
            std::unique_ptr<StaticText> label;
            std::unique_ptr<StaticText> valueLabel;
            Color barColor;
            Color handleColor;
            float minValue;
            float maxValue;
            float currentValue;
            bool dragging = false;
            int fontSize = 20;
            float handleRadius = 10.0f;

            void UpdateHandlePosition();
            void UpdateValueFromHandle();
    };
}

#endif