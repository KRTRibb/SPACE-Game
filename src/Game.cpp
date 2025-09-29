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
        UIElementID::NoPlayerButton,
        UIElementID::TitleText
    };

    gameOverUIElements = {
        UIElementID::BackToMenuButton,
        UIElementID::RestartButton,
        UIElementID::WinnerText,
        UIElementID::RedShipScoreText,
        UIElementID::YellowShipScoreText
    };

    playingUIElements = {
        UIElementID::RedShipHealthText,
        UIElementID::YellowShipHealthText
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
            break;

        case GameState::Playing:
            UpdatePlayingUI();
            redShip->Draw();
            yellowShip->Draw();
            break;

        case GameState::GameOver:
            UpdateGameOverUI();
            break;
    }

    uiManager.Render();
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

    for (const auto& id : playingUIElements) {
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

    for (const auto& id : playingUIElements) {
        UIElement* element = uiManager.GetElement(id);
        element->isVisible = true;
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

    for (const auto& id : playingUIElements) {
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

    std::unique_ptr<StaticText> titleText = std::make_unique<StaticText>("Space Battle", WIDTH / 2, HEIGHT / 2 - 100, WHITE, 50);
    std::unique_ptr<StaticText> yellowShipHealthText = std::make_unique<StaticText>(TextFormat("Health: %.1f", 100.0f), 80, 25, WHITE, 25);
    std::unique_ptr<StaticText> redShipHealthText = std::make_unique<StaticText>(TextFormat("Health: %.1f", 100.0f), WIDTH - 80, 25, WHITE, 25);
    std::unique_ptr<StaticText> yellowShipScoreText = std::make_unique<StaticText>(TextFormat("%d", 0), WIDTH / 2 - 200, HEIGHT / 2 + 40, WHITE, 30);
    std::unique_ptr<StaticText> redShipScoreText = std::make_unique<StaticText>(TextFormat("%d", 0), WIDTH / 2 + 200, HEIGHT / 2 + 40, WHITE, 30);
    std::unique_ptr<StaticText> winnerText = std::make_unique<StaticText>("", WIDTH / 2, HEIGHT / 2 - 25, WHITE, 50);

    uiManager.AddElement(UIElementID::RestartButton, std::move(restartButton));
    uiManager.AddElement(UIElementID::BackToMenuButton, std::move(backToMenuButton));
    uiManager.AddElement(UIElementID::SinglePlayerButton, std::move(singlePlayerButton));
    uiManager.AddElement(UIElementID::TwoPlayerButton, std::move(twoPlayerButton));
    uiManager.AddElement(UIElementID::NoPlayerButton, std::move(noPlayerButton));

    uiManager.AddElement(UIElementID::TitleText, std::move(titleText));
    uiManager.AddElement(UIElementID::YellowShipHealthText, std::move(yellowShipHealthText));
    uiManager.AddElement(UIElementID::RedShipHealthText, std::move(redShipHealthText));
    uiManager.AddElement(UIElementID::YellowShipScoreText, std::move(yellowShipScoreText));
    uiManager.AddElement(UIElementID::RedShipScoreText, std::move(redShipScoreText));
    uiManager.AddElement(UIElementID::WinnerText, std::move(winnerText));  
}

void Game::UpdatePlayingUI() {
    DrawTexture(resources.GetTexture("background"), 0, 0, WHITE); 

    Rectangle middleRect = {WIDTH / 2 - MIDDLERECTWIDTH / 2, 0, MIDDLERECTWIDTH, HEIGHT};
    DrawRectangleRec(middleRect, BLACK);

    auto yellowShipHealthText = dynamic_cast<StaticText*>(uiManager.GetElement(UIElementID::YellowShipHealthText));
    yellowShipHealthText->UpdateText(TextFormat("Health: %.1f", yellowShip->health));

    auto redShipHealthText = dynamic_cast<StaticText*>(uiManager.GetElement(UIElementID::RedShipHealthText));
    redShipHealthText->UpdateText(TextFormat("Health: %.1f", redShip->health));
}

void Game::UpdateGameOverUI() {
    DrawTexture(resources.GetTexture("background"), 0, 0, WHITE);

    const char* winnerMessage = winner == Winner::Red ? "RED WINS!" : "YELLOW WINS!";
    
    auto winnerText = dynamic_cast<StaticText*>(uiManager.GetElement(UIElementID::WinnerText));
    winnerText->UpdateText(winnerMessage);

    auto yellowShipScoreText = dynamic_cast<StaticText*>(uiManager.GetElement(UIElementID::YellowShipScoreText));
    yellowShipScoreText->UpdateText(TextFormat("%d", yellowShip->score));

    auto redShipScoreText = dynamic_cast<StaticText*>(uiManager.GetElement(UIElementID::RedShipScoreText));
    redShipScoreText->UpdateText(TextFormat("%d", redShip->score));
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