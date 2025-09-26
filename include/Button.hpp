#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "raylib.h" 

class Button {
    public:
        Button(const char* text, int centerX, int y, int fontSize, Color c, Color hc, Color pc);

        void Render() const;
        bool Update(); // returns true if clicked

    private:
        Rectangle bounds;
        const char* text;  
        Color color;
        Color hoverColor;
        Color pressedColor;
        Color currentColor;
        int fontSize;
};

#endif