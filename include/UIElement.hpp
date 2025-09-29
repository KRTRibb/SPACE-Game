#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

enum class UITypes {
    Button,
    StaticText,
    MovingText
};

class UIElement {
public:
    bool isVisible = true;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual ~UIElement() = default;
};

#endif