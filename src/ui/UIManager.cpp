#include "ui/UIManager.hpp"

void UIManager::AddElement(UIElementID id, std::unique_ptr<UIElement> element) {
    elements[id] = std::move(element);
}

void UIManager::RemoveElement(UIElementID id) {
    elements.erase(id);
}

void UIManager::Update(float dt) {
    for (auto& pair : elements) {
        if (pair.second->isVisible) {
            pair.second->Update(dt);
        }
    }
}

void UIManager::SetVisibility(const std::vector<UIElementID>& ids, bool visible) {
    for (auto id : ids) {
        auto elem = GetElement(id);
        if (elem) elem->isVisible = visible;
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