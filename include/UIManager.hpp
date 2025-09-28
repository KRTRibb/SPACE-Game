#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "raylib.h"
#include "UIElement.hpp"
#include <vector>

class UIManager {
    public:
        void AddElement(std::unique_ptr<UIElement> element);
        void Update(float dt);
        void Render();
    private:
        std::vector<std::unique_ptr<UIElement>> elements;
};

#endif
