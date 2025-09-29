#ifndef STATICTEXT_HPP
#define STATICTEXT_HPP

#include "raylib.h"
#include "ui/UIElement.hpp"
#include <string>

class StaticText: public UIElement {
    public:
        StaticText(std::string txt, int cX, int cY, Color c, int fsize);

        void Render();
        void UpdateText(std::string newTxt);
        
        private:
        std::string text;
        int centerX;
        int Y;
        Color color;
        int fontsize;
};

#endif