#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

class UIElement {
public:
    bool isVisible = true;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual ~UIElement() = default;
};

#endif