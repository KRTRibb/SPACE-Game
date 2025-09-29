#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "raylib.h"
#include "UIElement.hpp"
#include <unordered_map>
#include <vector>

class UIManager {
    public:
        void AddElement(UIType& type, std::unique_ptr<UIElement> element);
        void Update(float dt);
        void Render();
        std::unordered_map<UIType, UIElement>;
};

#endif
