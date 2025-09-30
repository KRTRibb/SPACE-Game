#include "ui/UIElements/Border.hpp"
#include "raylib.h"

Border::Border(int x, int y, int width, int height, Color c)
    : bounds{(float)x, (float)y, (float)width, (float)height}, color(c) {};

void Border::Render() {
    DrawRectangleRec(bounds, color);
}