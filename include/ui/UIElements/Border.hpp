#ifndef BORDER_HPP
#define BORDER_HPP

#include "raylib.h"
#include "ui/UIElement.hpp"
#include <string>

class Border : public UIElement {
    public:
        Border(int x, int y, int width, int height, Color c);

        void Render() override;

    private:
        Rectangle bounds;
        Color color;
};

#endif