#include "raylib.h"
#include "core/ResourceManager.hpp"
#include "core/spaceship.hpp"
#include "core/Game.hpp"
#include "controllers/PlayerController.hpp"
#include "controllers/AIController.hpp"
#include "core/config.h"
#include <string>
#include <vector>


Game::Game() {
    InitWindow(WIDTH, HEIGHT, "Space Game");
    InitAudioDevice();
    SetTargetFPS(FPS);
    SetExitKey(KEY_NULL);

    state = GameState::Menu;
    winner = Winner::None;

    menuUIElements = {
        ui::UIElementID::SinglePlayerButton,
        ui::UIElementID::TwoPlayerButton,
        ui::UIElementID::NoPlayerButton,
        ui::UIElementID::TitleText,
        ui::UIElementID::BackgroundImage,
        ui::UIElementID::SettingsButton
    };

    gameOverUIElements = {
        ui::UIElementID::BackToMenuButton,
        ui::UIElementID::RestartButton,
        ui::UIElementID::WinnerText,
        ui::UIElementID::RedShipScoreText,
        ui::UIElementID::YellowShipScoreText,
        ui::UIElementID::BackgroundImage,
        ui::UIElementID::SettingsButton
    };

    playingUIElements = {
        ui::UIElementID::RedShipHealthText,
        ui::UIElementID::YellowShipHealthText,
        ui::UIElementID::MiddleDivider,
        ui::UIElementID::BackgroundImage,
    };

    settingsUIElements = {
        ui::UIElementID::BackFromSettingsButton,
        ui::UIElementID::BackgroundImage,
        ui::UIElementID::VolumeSlider
    };

    resources.loadTexture("redShip", "assets/images/spaceship_red.png");
    resources.loadTexture("yellowShip", "assets/images/spaceship_yellow.png");
    resources.loadTexture("background", "assets/images/space.png");
    resources.loadTexture("energyLeftFacing", "assets/images/energyLeftFacing.png");
    resources.loadTexture("energyRightFacing", "assets/images/energyRightFacing.png");
    resources.loadSound("shoot", "assets/sounds/Gun+Silencer.mp3");
    resources.loadSound("hit", "assets/sounds/Grenade+1.mp3");
    resources.loadSound("energyShoot", "assets/sounds/spaceLaser.wav");

    SetUpUI();

    SetMenuUIVisible();

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
            auto singleBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::SinglePlayerButton));
            if (singleBtn && singleBtn->WasClicked()) {
                StartGame(GameMode::SinglePlayer);
                state = GameState::Playing;
                previousState = GameState::Menu;
                SetStateUIVisibility(state);
            }

            auto twoBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::TwoPlayerButton));
            if (twoBtn && twoBtn->WasClicked()) {
                StartGame(GameMode::TwoPlayer);
                state = GameState::Playing;
                previousState = GameState::Menu;
                SetStateUIVisibility(state);
            }

            auto noBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::NoPlayerButton));
            if (noBtn && noBtn->WasClicked()) {
                StartGame(GameMode::NoPlayer);
                state = GameState::Playing;
                previousState = GameState::Menu;
                SetStateUIVisibility(state);
            }

            auto settingsBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::SettingsButton));
            if (settingsBtn && settingsBtn->WasClicked()) {
                state = GameState::Settings;
                previousState = GameState::Menu;
                SetStateUIVisibility(state);
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                state = GameState::Settings;
                previousState = GameState::Menu;
                SetStateUIVisibility(state);
            }
            break;
        }
        case GameState::Playing: {
            redShip->Update(dt, *yellowShip);
            yellowShip->Update(dt, *redShip);

            if (redShip->IsDead()) {
                winner = Winner::Yellow;
                state = GameState::GameOver;
                previousState = GameState::Playing;
                SetStateUIVisibility(state);
            }

            if (yellowShip->IsDead()) {
                winner = Winner::Red;
                state = GameState::GameOver;
                previousState = GameState::Playing;
                SetStateUIVisibility(state);
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                state = GameState::Settings;
                previousState = GameState::Playing;
                SetStateUIVisibility(state);
            }
            break;
        }

        case GameState::GameOver: {
            auto restartBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::RestartButton));
            if (restartBtn && restartBtn->WasClicked()) {
                Reset();
                state = GameState::Playing;
                previousState = GameState::GameOver;
                SetStateUIVisibility(state);
            }
            auto menuBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::BackToMenuButton));
            if (menuBtn && menuBtn->WasClicked()) {
                state = GameState::Menu;
                previousState = GameState::GameOver;
                SetStateUIVisibility(state);
            }    

            auto settingsBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::SettingsButton));
            if (settingsBtn && settingsBtn->WasClicked()) {
                state = GameState::Settings;
                previousState = GameState::GameOver;
                SetStateUIVisibility(state);
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                state = GameState::Settings;
                previousState = GameState::GameOver;
                SetStateUIVisibility(state);
            }
            break;
        }

        case GameState::Settings: {
            auto backBtn = dynamic_cast<ui::Button*>(uiManager.GetElement(ui::UIElementID::BackFromSettingsButton));
            if (backBtn && backBtn->WasClicked()) {
                state = previousState;
                previousState = GameState::Settings;
                SetStateUIVisibility(state);
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                state = previousState;
                previousState = GameState::Settings;
                SetStateUIVisibility(state);
            }

            UpdateVolume();
        }
    }
}

void Game::Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (state) {
        case GameState::Menu:
            uiManager.Render();
            break;

        case GameState::Playing:
            UpdatePlayingUI();
            uiManager.Render();
            redShip->Draw();
            yellowShip->Draw();
            break;

        case GameState::GameOver:
            UpdateGameOverUI();
            uiManager.Render();
            break;

        case GameState::Settings:
            uiManager.Render();
            break;
    }

    EndDrawing();
}

void Game::SetUIVisibility(const std::vector<ui::UIElementID>& ids, bool visible) {
    for (const auto& id : ids) {
        ui::UIElement* element = uiManager.GetElement(id);
        if (element) {
            element->isVisible = visible;
        }
    }
}

void Game::SetMenuUIVisible() {
    SetUIVisibility(gameOverUIElements, false);
    SetUIVisibility(playingUIElements, false);
    SetUIVisibility(settingsUIElements, false);
    SetUIVisibility(menuUIElements, true);
}

void Game::SetPlayingUIVisible() {
    SetUIVisibility(gameOverUIElements, false);
    SetUIVisibility(menuUIElements, false);
    SetUIVisibility(settingsUIElements, false);
    SetUIVisibility(playingUIElements, true);
}

void Game::SetGameOverUIVisible() {
    SetUIVisibility(playingUIElements, false);
    SetUIVisibility(menuUIElements, false);
    SetUIVisibility(settingsUIElements, false);
    SetUIVisibility(gameOverUIElements, true);
}

void Game::SetSettingsUIVisible() {
    SetUIVisibility(playingUIElements, false);
    SetUIVisibility(menuUIElements, false);
    SetUIVisibility(gameOverUIElements, false);
    SetUIVisibility(settingsUIElements, true);
}

void Game::SetStateUIVisibility(GameState state){
    switch (state) {
        case GameState::Menu:
            SetMenuUIVisible();
            break;
        case GameState::Playing:
            SetPlayingUIVisible();
            break;
        case GameState::GameOver:
            SetGameOverUIVisible();
            break;
        case GameState::Settings:
            SetSettingsUIVisible();
            break;
    }
}

void Game::Reset() {
    redShip->Reset();
    yellowShip->Reset();
    winner = Winner::None;
}

void Game::SetUpUI() {
    std::unique_ptr<ui::Button> restartButton = std::make_unique<ui::Button> ("Restart Game", WIDTH / 2 - 125, HEIGHT / 2 + 25, 30, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<ui::Button> backToMenuButton = std::make_unique<ui::Button> ("Back To Menu", WIDTH / 2 + 125, HEIGHT / 2 + 25, 30, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<ui::Button> singlePlayerButton = std::make_unique<ui::Button> ("Single Player", WIDTH / 2 - 100, HEIGHT / 2, 30, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<ui::Button> twoPlayerButton = std::make_unique<ui::Button> ("Two Player", WIDTH / 2 + 100, HEIGHT / 2, 30, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<ui::Button> noPlayerButton = std::make_unique<ui::Button> ("AI vs AI", WIDTH / 2, HEIGHT / 2 + 75, 30, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<ui::Button> settingsButton = std::make_unique<ui::Button> ("|||", WIDTH - 50, HEIGHT - 60, 30, GRAY, DARKGRAY, BLACK);
    std::unique_ptr<ui::Button> backFromSettingsButton = std::make_unique<ui::Button> ("Back", WIDTH - 50, 20, 30, GRAY, DARKGRAY, BLACK);

    std::unique_ptr<ui::FloatingText> titleText = std::make_unique<ui::FloatingText>("Space Battle", WIDTH / 2, HEIGHT / 2 - 150, WHITE, 100, 10);
    std::unique_ptr<ui::StaticText> yellowShipHealthText = std::make_unique<ui::StaticText>(TextFormat("Health: %.1f", 100.0f), 80, 25, WHITE, 25);
    std::unique_ptr<ui::StaticText> redShipHealthText = std::make_unique<ui::StaticText>(TextFormat("Health: %.1f", 100.0f), WIDTH - 80, 25, WHITE, 25);
    std::unique_ptr<ui::StaticText> yellowShipScoreText = std::make_unique<ui::StaticText>(TextFormat("%d", 0), WIDTH / 2 - 250, HEIGHT / 2 + 40, WHITE, 30);
    std::unique_ptr<ui::StaticText> redShipScoreText = std::make_unique<ui::StaticText>(TextFormat("%d", 0), WIDTH / 2 + 250, HEIGHT / 2 + 40, WHITE, 30);
    std::unique_ptr<ui::FloatingText> winnerText = std::make_unique<ui::FloatingText>("", WIDTH / 2, HEIGHT / 2 - 45, WHITE, 50, 20);

    std::unique_ptr<ui::Border> middleDivider = std::make_unique<ui::Border>(WIDTH / 2 - MIDDLERECTWIDTH, 0, MIDDLERECTWIDTH, HEIGHT, BLACK);

    std::unique_ptr<ui::Image> backgroundImage = std::make_unique<ui::Image>(resources.GetTexture("background"), 0, 0, WIDTH, HEIGHT);

    std::unique_ptr<ui::Slider> volumeSlider = std::make_unique<ui::Slider>("Volume", WIDTH / 2, HEIGHT / 2, 300, 20, 0.0f, 1.0f, 0.5f, DARKGRAY, LIGHTGRAY);

    uiManager.AddElement(ui::UIElementID::RestartButton, std::move(restartButton));
    uiManager.AddElement(ui::UIElementID::BackToMenuButton, std::move(backToMenuButton));
    uiManager.AddElement(ui::UIElementID::SinglePlayerButton, std::move(singlePlayerButton));
    uiManager.AddElement(ui::UIElementID::TwoPlayerButton, std::move(twoPlayerButton));
    uiManager.AddElement(ui::UIElementID::NoPlayerButton, std::move(noPlayerButton));
    uiManager.AddElement(ui::UIElementID::SettingsButton, std::move(settingsButton));
    uiManager.AddElement(ui::UIElementID::BackFromSettingsButton, std::move(backFromSettingsButton));

    uiManager.AddElement(ui::UIElementID::TitleText, std::move(titleText));
    uiManager.AddElement(ui::UIElementID::YellowShipHealthText, std::move(yellowShipHealthText));
    uiManager.AddElement(ui::UIElementID::RedShipHealthText, std::move(redShipHealthText));
    uiManager.AddElement(ui::UIElementID::YellowShipScoreText, std::move(yellowShipScoreText));
    uiManager.AddElement(ui::UIElementID::RedShipScoreText, std::move(redShipScoreText));
    uiManager.AddElement(ui::UIElementID::WinnerText, std::move(winnerText));  

    uiManager.AddElement(ui::UIElementID::MiddleDivider, std::move(middleDivider));

    uiManager.AddElement(ui::UIElementID::VolumeSlider, std::move(volumeSlider));

    uiManager.AddElement(ui::UIElementID::BackgroundImage, std::move(backgroundImage));
}

void Game::UpdatePlayingUI() {
    auto yellowShipHealthText = dynamic_cast<ui::StaticText*>(uiManager.GetElement(ui::UIElementID::YellowShipHealthText));
    yellowShipHealthText->UpdateText(TextFormat("Health: %.1f", yellowShip->health));

    auto redShipHealthText = dynamic_cast<ui::StaticText*>(uiManager.GetElement(ui::UIElementID::RedShipHealthText));
    redShipHealthText->UpdateText(TextFormat("Health: %.1f", redShip->health));
}

void Game::UpdateGameOverUI() {
    const char* winnerMessage = winner == Winner::Red ? "RED WINS!" : "YELLOW WINS!";
    
    auto winnerText = dynamic_cast<ui::FloatingText*>(uiManager.GetElement(ui::UIElementID::WinnerText));
    winnerText->UpdateText(winnerMessage);

    auto yellowShipScoreText = dynamic_cast<ui::StaticText*>(uiManager.GetElement(ui::UIElementID::YellowShipScoreText));
    yellowShipScoreText->UpdateText(TextFormat("%d", yellowShip->score));

    auto redShipScoreText = dynamic_cast<ui::StaticText*>(uiManager.GetElement(ui::UIElementID::RedShipScoreText));
    redShipScoreText->UpdateText(TextFormat("%d", redShip->score));
}

void Game::UpdateVolume() {
    auto volumeSlider = dynamic_cast<ui::Slider*>(uiManager.GetElement(ui::UIElementID::VolumeSlider));
    if (volumeSlider) {
        float volume = volumeSlider->GetValue();
        SetMasterVolume(volume);
    }
}

void Game::StartGame(GameMode mode) {
        yellowShip = std::make_unique<Spaceship>(
        resources.GetTexture("yellowShip"), Side::LEFT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyLeftFacing"), resources.GetSound("energyShoot"),
        nullptr
    );

    redShip = std::make_unique<Spaceship>(
        resources.GetTexture("redShip"), Side::RIGHT,
        resources.GetSound("shoot"), resources.GetSound("hit"), 
        resources.GetTexture("energyRightFacing"), resources.GetSound("energyShoot"),
        nullptr
    );

    switch (mode) {
        case GameMode::TwoPlayer: {
            std::unique_ptr yellowController = std::make_unique<PlayerController>(
                std::vector<int>{KEY_W, KEY_S, KEY_A, KEY_D}, KEY_C, KEY_V
            );

            std::unique_ptr redController = std::make_unique<PlayerController>(
            std::vector<int>{KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, KEY_M, KEY_K
            );

            yellowShip->controller = std::move(yellowController);
            redShip->controller = std::move(redController);
            break;
        }

        case GameMode::SinglePlayer: {
            std::unique_ptr yellowController = std::make_unique<PlayerController>(
                std::vector<int>{KEY_W, KEY_S, KEY_A, KEY_D}, KEY_C, KEY_V
            );

            std::unique_ptr redAIController = std::make_unique<AIController>(redShip.get(), yellowShip.get());

            yellowShip->controller = std::move(yellowController);
            redShip->controller = std::move(redAIController);

            #if AITest
            yellowShip->health = 100;
            redShip->health = 100;
            #endif

            break;
        }

        case GameMode::NoPlayer: {
            yellowShip->controller = std::make_unique<AIController>(yellowShip.get(), redShip.get());
            redShip->controller = std::make_unique<AIController>(redShip.get(), yellowShip.get());

            #if AITest
            yellowShip->health = 100;
            redShip->health = 100;
            #endif
            
            break;
        }
    }
}