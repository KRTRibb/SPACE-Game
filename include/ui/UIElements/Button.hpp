#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "raylib.h"     
#include "ui/UIElement.hpp"
#include <string>

namespace ui{
    class Button: public UIElement {
        public:
            Button(std::string text, int centerX, int y, int fontSize, Color c, Color hc, Color pc);

            void Update(float dt) override;
            void Render() override;
            bool WasClicked() const;

        private:
            Rectangle bounds;
            std::string text;  
            Color color;
            Color hoverColor;
            Color pressedColor;
            Color currentColor;
            int fontSize;
            bool clicked = false;
    };
}

#endif