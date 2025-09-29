#ifndef GAME_HPP
#define GAME_HPP

#include "raylib.h"
#include "ResourceManager.hpp"
#include "spaceship.hpp"
#include "Button.hpp"
#include <string>
#include "UIManager.hpp"


enum class GameState {
    Menu,
    Playing,
    GameOver
};

enum class GameMode {
    None, 
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

    void DrawWindow();
    void DrawGameOver();
    void DrawMenu();
    void StartSinglePlayer();
    void StartTwoPlayer();
    void StartNoPlayer();
};


#endif