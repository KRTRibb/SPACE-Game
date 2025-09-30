#include "raylib.h"
#include "ui/UIElements/Image.hpp"

namespace ui{
    Image::Image(Texture2D tex, int x, int y, int width, int height) {
        texture = tex;
        dest = {(float)x, (float)y, (float)width, (float)height};
        src = {0, 0, (float)texture.width, (float)texture.height};
        origin = {0, 0};
    }

    void Image::Render() {
        DrawTexturePro(texture, src, dest, origin, rotation, color);
    }
}