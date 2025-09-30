#ifndef PICTURE_HPP
#define PICTURE_HPP

#include "raylib.h"
#include "ui/UIElement.hpp"
#include "core/ResourceManager.hpp"

// For image ui elements that just need to be drawn to the screen
class Picture : public UIElement {
    public:
        Picture(Texture2D tex, int x, int y, int width, int height);

        void Render() override;

    private:
        Texture2D texture;
        Rectangle dest;
        Rectangle src;
        Vector2 origin; // Origin of rotation and scaling
        float rotation = 0.0f;
        Color color = WHITE;
};

#endif