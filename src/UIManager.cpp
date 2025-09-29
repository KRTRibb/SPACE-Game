#include "UIManager.hpp"

void UIManager::AddElement(UIElementID id, std::unique_ptr<UIElement> element) {
    elements[id] = std::move(element);
}

void UIManager::Update(float dt) {
    for (auto& pair : elements) {
        if (pair.second->isVisible) {
            pair.second->Update(dt);
        }
    }
}

UIElement* UIManager::GetElement(UIElementID id) {
    auto it = elements.find(id);
    return (it != elements.end()) ? it->second.get() : nullptr;
}

void UIManager::Render() {
    for (auto& pair : elements) {
        if (pair.second->isVisible) {
            pair.second->Render();
        }
    }
}