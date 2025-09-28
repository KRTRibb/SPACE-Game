#include "raylib.h"
#include "ResourceManager.hpp"
#include "spaceship.hpp"
#include "Game.hpp"
#include "PlayerController.hpp"
#include "AIController.hpp"
#include "config.h"
#include <string>


Game::Game() {
    InitWindow(WIDTH, HEIGHT, "Space Game");
    InitAudioDevice();
    SetTargetFPS(FPS);

    state = GameState::Menu;
    winner = Winner::None;

    SetUpUI();
    SetMenuUIVisible();

    resources.loadTexture("redShip", "graphics/spaceship_red.png");
    resources.loadTexture("yellowShip", "graphics/spaceship_yellow.png");
    resources.loadTexture("background", "graphics/space.png");
    resources.loadTexture("energyLeftFacing", "graphics/energyLeftFacing.png");
    resources.loadTexture("energyRightFacing", "graphics/energyRightFacing.png");
    resources.loadSound("shoot", "graphics/Gun+Silencer.mp3");
    resources.loadSound("hit", "graphics/Grenade+1.mp3");
    resources.loadSound("energyShoot", "graphics/spaceLaser.wav");
};

Game::~Game() {
    resources.UnloadAll();
    CloseAudioDevice();
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        Update();
        Render();
    }
}

void Game::Update() {
    float dt = GetFrameTime();
    uiManager.Update(dt);

    switch (state) {
        case GameState::Menu:
            if (singlePlayerButton && singlePlayerButton->WasClicked()) {
                StartSinglePlayer();
                state = GameState::Playing;
            }

            if (twoPlayerButton && twoPlayerButton->WasClicked()) {
                StartTwoPlayer();
                state = GameState::Playing;
            }

            if (noPlayerButton && noPlayerButton->WasClicked()) {
                StartNoPlayer();
                state = GameState::Playing;
            }
            break;

        case GameState::Playing:
            redShip->Update(dt, *yellowShip);
            yellowShip->Update(dt, *redShip);

            if (redShip->IsDead()) {
                winner = Winner::Yellow;
                state = GameState::GameOver;
                SetGameOverUIVisible();
            }

            if (yellowShip->IsDead()) {
                winner = Winner::Red;
                state = GameState::GameOver;
                SetGameOverUIVisible();
            }
            break;

        case GameState::GameOver:
            if (restartButton && restartButton->WasClicked()) {
                Reset();
            }

            if (backToMenuButton && backToMenuButton->WasClicked()) {
                state = GameState::Menu;
                SetMenuUIVisible();
            }    

            break;
    }
}

void Game::Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (state) {
        case GameState::Menu:
            DrawMenu();
        uiManager.Render();
            break;

        case GameState::Playing:
            DrawWindow();
            redShip->Draw();
            yellowShip->Draw();
            uiManager.Render();
            break;

        case GameState::GameOver:
            DrawGameOver();
            uiManager.Render();
            break;
    }
    EndDrawing();
}

void Game::Reset() {
    redShip->Reset();
    yellowShip->Reset();
    state = GameState::Playing;
    winner = Winner::None;
}

void Game::SetUpUI() {
    restartButton = std::make_unique<Button> ("Restart Game", WIDTH / 2 - 100, HEIGHT / 2 + 25, 25, GRAY, DARKGRAY, BLACK);
    restartButtonPtr = restartButton.get();
    backToMenuButton = std::make_unique<Button> ("Back To Menu", WIDTH / 2 + 100, HEIGHT / 2 + 25, 25, GRAY, DARKGRAY, BLACK);
    backToMenuButtonPtr = backToMenuButton.get();
    singlePlayerButton = std::make_unique<Button> ("Single Player", WIDTH / 2 - 100, HEIGHT / 2, 25, GRAY, DARKGRAY, BLACK);\
    singlePlayerButtonPtr = singlePlayerButton.get();
    twoPlayerButton = std::make_unique<Button> ("Two Player", WIDTH / 2 + 100, HEIGHT / 2, 25, GRAY, DARKGRAY, BLACK);
    twoPlayerButtonPtr = twoPlayerButton.get();
    noPlayerButton = std::make_unique<Button> ("AI vs AI", WIDTH / 2, HEIGHT / 2 + 75, 25, GRAY, DARKGRAY, BLACK);
    noPlayerButtonPtr = noPlayerButton.get();

    uiManager.AddElement(std::move(restartButton));
    uiManager.AddElement(std::move(backToMenuButton));
    uiManager.AddElement(std::move(singlePlayerButton));
    uiManager.AddElement(std::move(twoPlayerButton));
    uiManager.AddElement(std::move(noPlayerButton));
}

void Game::SetMenuUIVisible() {
    singlePlayerButtonPtr->isVisible = true;
    twoPlayerButtonPtr->isVisible = true;
    noPlayerButtonPtr->isVisible = true;

    restartButtonPtr->isVisible = false;
    backToMenuButtonPtr->isVisible = false;
}

void Game::SetGameOverUIVisible() {
    restartButtonPtr->isVisible = true;
    backToMenuButtonPtr->isVisible = true;

    singlePlayerButtonPtr->isVisible = false;
    twoPlayerButtonPtr->isVisible = false;
    noPlayerButtonPtr->isVisible = false;
}

void Game::DrawWindow() {
    DrawTexture(resources.GetTexture("background"), 0, 0, WHITE);

    Rectangle middleRect = {WIDTH / 2 - MIDDLERECTWIDTH / 2, 0, MIDDLERECTWIDTH, HEIGHT};
    DrawRectangleRec(middleRect, BLACK);

    DrawText(TextFormat("Health: %.1f", yellowShip->health), 10, 10, 25, WHITE);

    const char* rightHandText = TextFormat("Health: %.1f", redShip->health);
    int textWidth = MeasureText(rightHandText, 25);
    DrawText(rightHandText, WIDTH - 10 - textWidth, 10, 25, WHITE);
}

void Game::DrawGameOver() {
    DrawTexture(resources.GetTexture("background"), 0, 0, WHITE);

    const char* winnerMessage = winner == Winner::Red ? "RED WINS!" : "YELLOW WINS!";
    int textWidth = MeasureText(winnerMessage, 50);
    DrawText(winnerMessage, WIDTH / 2 - textWidth / 2, HEIGHT / 2 - 25, 50, WHITE);

    DrawText(TextFormat("%d", yellowShip->score), WIDTH / 2 - textWidth / 2 - 70, HEIGHT / 2 + yellowShip->shipRect.height, 30, WHITE);
    DrawText(TextFormat("%d", redShip->score), WIDTH / 2 + textWidth / 2 + 70, HEIGHT / 2 + yellowShip->shipRect.height, 30, WHITE);
}

void Game::DrawMenu() {
    DrawTexture(resources.GetTexture("background"), 0, 0, WHITE);
}

void Game::StartSinglePlayer() {
    std::unique_ptr playerController = std::make_unique<PlayerController>(
        std::vector<int>{KEY_W, KEY_S, KEY_A, KEY_D}, KEY_C, KEY_V
    );

    // Player Ship
    yellowShip = std::make_unique<Spaceship>(
        resources.GetTexture("yellowShip"), Side::LEFT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyLeftFacing"), resources.GetSound("energyShoot"),
        std::move(playerController)
    );


    // AI Ship
    redShip = std::make_unique<Spaceship>(
        resources.GetTexture("redShip"), Side::RIGHT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyRightFacing"), resources.GetSound("energyShoot"),
        nullptr
    );

    redShip->controller = std::make_unique<AIController>(redShip.get(), yellowShip.get());

    #if AITest
    yellowShip->health = 100;
    redShip->health = 100;
    #endif
}

void Game::StartTwoPlayer() {
    std::unique_ptr redController = std::make_unique<PlayerController>(
        std::vector<int>{KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, KEY_M, KEY_K
    );
    std::unique_ptr yellowController = std::make_unique<PlayerController>(
        std::vector<int>{KEY_W, KEY_S, KEY_A, KEY_D}, KEY_C, KEY_V
    );


    redShip = std::make_unique<Spaceship>(
        resources.GetTexture("redShip"), Side::RIGHT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyRightFacing"), resources.GetSound("energyShoot"),
        std::move(redController)
    );

    yellowShip = std::make_unique<Spaceship>(
        resources.GetTexture("yellowShip"), Side::LEFT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyLeftFacing"), resources.GetSound("energyShoot"),
        std::move(yellowController)
    );
}

void Game::StartNoPlayer() {
    // AI ship 1
    yellowShip = std::make_unique<Spaceship>(
        resources.GetTexture("yellowShip"), Side::LEFT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyLeftFacing"), resources.GetSound("energyShoot"),
        nullptr
    );

    // AI Ship 2
    redShip = std::make_unique<Spaceship>(
        resources.GetTexture("redShip"), Side::RIGHT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyRightFacing"), resources.GetSound("energyShoot"),
        nullptr
    );

    yellowShip->controller = std::make_unique<AIController>(yellowShip.get(), redShip.get());
    redShip->controller = std::make_unique<AIController>(redShip.get(), yellowShip.get());

    #if AITest
    yellowShip->health = 100;
    redShip->health = 100;
    #endif
}