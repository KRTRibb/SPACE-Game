#include "UIManager.hpp"

void UIManager::AddElement(std::unique_ptr<UIElement> element) {
    elements.push_back(std::move(element));
}

void UIManager::Update(float dt) {
    for (auto& element : elements) {
        if (element->isVisible) {
            element->Update(dt);
        }
    }
}

void UIManager::Render() {
    for (auto& element : elements) {
        if (element->isVisible) {
            element->Render();
        }
    }
}