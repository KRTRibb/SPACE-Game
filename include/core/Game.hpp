#ifndef GAME_HPP
#define GAME_HPP

#include "raylib.h"
#include "ResourceManager.hpp"
#include "spaceship.hpp"
#include "ui/UIManager.hpp"
#include "ui/UIElements/Button.hpp"
#include "ui/UIElements/StaticText.hpp"
#include "ui/UIElements/FloatingText.hpp"
#include <string>


enum class GameState {
    Menu,
    Playing,
    GameOver
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

    UIManager uiManager;

    ResourceManager resources;
    std::unique_ptr<Spaceship> redShip;
    std::unique_ptr<Spaceship> yellowShip;

    std::vector<UIElementID> menuUIElements;
    std::vector<UIElementID> gameOverUIElements;
    std::vector<UIElementID> playingUIElements;

    void SetUpUI();
    void SetMenuUIVisible();
    void SetGameOverUIVisible();
    void SetPlayingUIVisible();

    void UpdatePlayingUI();
    void UpdateGameOverUI();
    void DrawMenu();
    void StartGame(GameMode mode);
};


#endif