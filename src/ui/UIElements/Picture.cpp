#include "raylib.h"
#include "ui/UIElements/Picture.hpp"

Picture::Picture(Texture2D tex, int x, int y, int width, int height) {
    texture = tex;
    dest = {(float)x, (float)y, (float)width, (float)height};
    src = {0, 0, (float)texture.width, (float)texture.height};
    origin = {0, 0};
}

void Picture::Render() {
    DrawTexturePro(texture, src, dest, origin, rotation, color);
}