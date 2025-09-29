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

    menuUIElements = {
        UIElementID::SinglePlayerButton,
        UIElementID::TwoPlayerButton,
        UIElementID::NoPlayerButton
    };

    gameOverUIElements = {
        UIElementID::BackToMenuButton,
        UIElementID::RestartButton
    };

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
        case GameState::Menu: {
            auto singleBtn = dynamic_cast<Button*>(uiManager.GetElement(UIElementID::SinglePlayerButton));
            if (singleBtn && singleBtn->WasClicked()) {
                StartSinglePlayer();
                state = GameState::Playing;
                SetPlayingUIVisible();
            }

            auto twoBtn = dynamic_cast<Button*>(uiManager.GetElement(UIElementID::TwoPlayerButton));
            if (twoBtn && twoBtn->WasClicked()) {
                StartTwoPlayer();
                state = GameState::Playing;
                SetPlayingUIVisible();
            }

            auto noBtn = dynamic_cast<Button*>(uiManager.GetElement(UIElementID::NoPlayerButton));
            if (noBtn && noBtn->WasClicked()) {
                StartNoPlayer();
                state = GameState::Playing;
                SetPlayingUIVisible();
            }
            break;
        }
        case GameState::Playing: {
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
        }

        case GameState::GameOver: {
            auto restartBtn = dynamic_cast<Button*>(uiManager.GetElement(UIElementID::RestartButton));
            if (restartBtn && restartBtn->WasClicked()) {
                Reset();
                SetPlayingUIVisible();
            }
            auto menuBtn = dynamic_cast<Button*>(uiManager.GetElement(UIElementID::BackToMenuButton));
            if (menuBtn && menuBtn->WasClicked()) {
                state = GameState::Menu;
                SetMenuUIVisible();
            }    
            break;
        }
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

void Game::SetMenuUIVisible() {
    for (const auto& id : menuUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = true;
    }

    for (const auto& id : gameOverUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = false;
    }
}

void Game::SetPlayingUIVisible() {
    for (const auto& id : menuUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = false;
    }

    for (const auto& id : gameOverUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = false;
    }
}

void Game::SetGameOverUIVisible() {
    for (const auto& id : gameOverUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = true;
    }
    
    for (const auto& id : menuUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = false;
    }
}


void Game::Reset() {
    redShip->Reset();
    yellowShip->Reset();
    state = GameState::Playing;
    winner = Winner::None;
}

void Game::SetUpUI() {
    std::unique_ptr<Button> restartButton = std::make_unique<Button> ("Restart Game", WIDTH / 2 - 100, HEIGHT / 2 + 25, 25, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<Button> backToMenuButton = std::make_unique<Button> ("Back To Menu", WIDTH / 2 + 100, HEIGHT / 2 + 25, 25, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<Button> singlePlayerButton = std::make_unique<Button> ("Single Player", WIDTH / 2 - 100, HEIGHT / 2, 25, GRAY, DARKGRAY, BLACK);\
    std::unique_ptr<Button> twoPlayerButton = std::make_unique<Button> ("Two Player", WIDTH / 2 + 100, HEIGHT / 2, 25, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<Button> noPlayerButton = std::make_unique<Button> ("AI vs AI", WIDTH / 2, HEIGHT / 2 + 75, 25, GRAY, DARKGRAY, BLACK);

    uiManager.AddElement(UIElementID::RestartButton, std::move(restartButton));
    uiManager.AddElement(UIElementID::BackToMenuButton, std::move(backToMenuButton));
    uiManager.AddElement(UIElementID::SinglePlayerButton, std::move(singlePlayerButton));
    uiManager.AddElement(UIElementID::TwoPlayerButton, std::move(twoPlayerButton));
    uiManager.AddElement(UIElementID::NoPlayerButton, std::move(noPlayerButton));
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