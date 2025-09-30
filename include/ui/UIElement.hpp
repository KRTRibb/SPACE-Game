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
    WinnerText,
    MiddleDivider,
    BackgroundImage
};

// Makes the enum class hashable so it can be used as a key in unordered_map
// Does this by converting it to its underlying type (usually int but could be usigned in, etc)
// It converts that int to a size_t using the standard hash function 
// This allows UIElementID to be used as a key.
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