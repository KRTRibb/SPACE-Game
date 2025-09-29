#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

enum class UIElementID {
    RestartButton,
    BackToMenuButton,
    SinglePlayerButton,
    TwoPlayerButton,
    NoPlayerButton
};

class UIElement {
public:
    bool isVisible = false;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual ~UIElement() = default;
};

#endif