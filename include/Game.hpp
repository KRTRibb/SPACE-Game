#ifndef GAME_HPP
#define GAME_HPP

#include "raylib.h"
#include "ResourceManager.hpp"
#include "spaceship.hpp"
#include "Button.hpp"
#include <string>


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

    ResourceManager resources;
    std::unique_ptr<Spaceship> redShip;
    std::unique_ptr<Spaceship> yellowShip;
    std::unique_ptr<Button> restartButton;
    std::unique_ptr<Button> backToMenuButton;
    std::unique_ptr<Button> singlePlayerButton;
    std::unique_ptr<Button> twoPlayerButton;
    std::unique_ptr<Button> noPlayerButton;

    void DrawWindow();
    void DrawGameOver();
    void DrawMenu();
    void StartSinglePlayer();
    void StartTwoPlayer();
    void StartNoPlayer();
};


#endif