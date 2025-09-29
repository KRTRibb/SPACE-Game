#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include <cstddef>
#include <functional>

enum class UIElementID {
    RestartButton,
    BackToMenuButton,
    SinglePlayerButton,
    TwoPlayerButton,
    NoPlayerButton,
    TitleText,
    RedShipHealthText,
    YellowShipHealthText,
    RedShipScoreText,
    YellowShipScoreText,
    WinnerText
};

namespace std{
    template <>
    struct hash<UIElementID>{
        std::size_t operator()(const UIElementID& id) const noexcept{
            return std::hash<std::underlying_type<UIElementID>::type>()(
                static_cast<std::underlying_type<UIElementID>::type>(id)
            );
        }
    };
}

class UIElement {
public:
    bool isVisible = false;
    virtual void Update(float dt) {};
    virtual void Render() = 0;
    virtual ~UIElement() = default;
};

#endif