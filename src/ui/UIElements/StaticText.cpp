#include "ui/UIElements/StaticText.hpp"
#include "raylib.h"

StaticText::StaticText(std::string txt, int cX, int y, Color c, int fsize) 
: text{txt}, centerX{cX}, Y{y}, color{c}, fontsize{fsize} {
    isVisible = false;
}


void StaticText::Render() {
    float textWidth = MeasureText(text.c_str(), fontsize);
    DrawText(text.c_str(), centerX - textWidth / 2, Y, fontsize, color);
}

void StaticText::UpdateText(std::string newTxt) {
    text = newTxt;
}



