#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "raylib.h"
#include "UIElement.hpp"
#include <unordered_map>
#include <memory>

class UIManager {
    public:
        void AddElement(UIElementID uiType, std::unique_ptr<UIElement> element);
        void Update(float dt);
        void Render();
        UIElement* GetElement(UIElementID id);
    private:
        std::unordered_map<UIElementID, std::unique_ptr<UIElement>> elements;
};

#endif
