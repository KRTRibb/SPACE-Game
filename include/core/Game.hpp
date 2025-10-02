#ifndef GAME_HPP
#define GAME_HPP

#include "raylib.h"
#include "ResourceManager.hpp"
#include "spaceship.hpp"
#include "ui/UIManager.hpp"
#include "ui/UIElements/Button.hpp"
#include "ui/UIElements/StaticText.hpp"
#include "ui/UIElements/FloatingText.hpp"
#include "ui/UIElements/Border.hpp"
#include "ui/UIElements/Image.hpp"
#include "ui/UIElements/Slider.hpp"
#include <string>


enum class GameState {
    Menu,
    Playing,
    GameOver,
    Settings,
    StoryMode
};

enum class GameMode {
    NoPlayer, 
    SinglePlayer,
    TwoPlayer
};

enum class Winner {
    None,
    Red,
    Yellow
};

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    void Update();
    void Render();
    void Reset();

    GameState state;
    Winner winner;

    ui::UIManager uiManager;

    ResourceManager resources;
    std::unique_ptr<Spaceship> redShip;
    std::unique_ptr<Spaceship> yellowShip;

    std::vector<ui::UIElementID> menuUIElements;
    std::vector<ui::UIElementID> gameOverUIElements;
    std::vector<ui::UIElementID> playingUIElements;
    std::vector<ui::UIElementID> settingsUIElements;
    std::vector<ui::UIElementID> storyModeUIElements;

    GameState previousState = GameState::Menu;

    void SetUpUI();
    void SetUIVisibility(const std::vector<ui::UIElementID>& ids, bool visible);
    void SetMenuUIVisible();
    void SetGameOverUIVisible();
    void SetPlayingUIVisible();
    void SetSettingsUIVisible();
    void SetStoryModeUIVisible();
    void SetStateUIVisibility(GameState state);

    void HandleTransitionToSettings();

    void UpdatePlayingUI();
    void UpdateGameOverUI();
    void StartGame(GameMode mode);

    void UpdateVolume();
};


#endif