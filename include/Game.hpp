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
    std::unique_ptr<Button> restartButton;
    Button* restartButtonPtr = nullptr;
    std::unique_ptr<Button> backToMenuButton;
    Button* backToMenuButtonPtr = nullptr;
    std::unique_ptr<Button> singlePlayerButton;
    Button* singlePlayerButtonPtr = nullptr;
    std::unique_ptr<Button> twoPlayerButton;
    Button* twoPlayerButtonPtr = nullptr;
    std::unique_ptr<Button> noPlayerButton;
    Button* noPlayerButtonPtr = nullptr;

    ResourceManager resources;
    std::unique_ptr<Spaceship> redShip;
    std::unique_ptr<Spaceship> yellowShip;

    void SetUpUI();
    void SetMenuUIVisible();
    void SetGameOverUIVisible();

    void DrawWindow();
    void DrawGameOver();
    void DrawMenu();
    void StartSinglePlayer();
    void StartTwoPlayer();
    void StartNoPlayer();
};


#endif