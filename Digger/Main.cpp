#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "TextureComponent.h"
#include <filesystem>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include "InputManager.h"
#include "LevelManager.h"
#include "PlayerComponent.h"
#include "Observer.h"
#include "ScoreComponent.h"
#include "LifeComponent.h"
#include "DeathComponent.h"
#include "SdlSoundSystem.h"
#include "ServiceLocator.h"
#include "Enemy/EnemySpawnManager.h"
#include "Enemy/EnemySpawnUpdateComponent.h"
#include "Player/PlayerAccessor.h"
#include "MenuComponent.h"
#include "ButtonComponent.h"
#include "LetterSwitchComponent.h"
#include <fstream>
#include <HitBoxComponent.h>
#include <SdlGamePadSystem.h>
#include "Player/PlayerDamageComponent.h"

namespace fs = std::filesystem;

auto* pHighScoreTextComponent = static_cast<dae::TextComponent*>(nullptr);

static void PopulateHighScores()
{
	auto& gameDataManager = dae::serviceLocator::GetGameDataManager();
	std::ifstream file(gameDataManager.GetHighScorePath());
	std::string highScores;
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			highScores += line + "\n";
		}
		file.close();
	}
	else
	{
		highScores = "No high scores yet!";
	}
	if (pHighScoreTextComponent)
	{
		pHighScoreTextComponent->SetText(highScores);
	}
}

auto& menuScene = dae::SceneManager::GetInstance().CreateScene();
auto& ScoreSaveScreen = dae::SceneManager::GetInstance().CreateScene();
auto& HighScoreScreen = dae::SceneManager::GetInstance().CreateScene([] {PopulateHighScores();}, [] {});
auto& scene = dae::SceneManager::GetInstance().CreateScene();

auto* letterSwitchComponentPtr1 = static_cast<dae::LetterSwitchComponent*>(nullptr);
auto* letterSwitchComponentPtr2 = static_cast<dae::LetterSwitchComponent*>(nullptr);
auto* letterSwitchComponentPtr3 = static_cast<dae::LetterSwitchComponent*>(nullptr);

static void ReturnToStartMenu()
{
	dae::SceneManager::GetInstance().SetActiveScene(&menuScene);
	dae::InputManager::GetInstance().ChangeMenuCommandContext(0);
}

static void StartGameFromMenu()
{
	dae::SceneManager::GetInstance().SetActiveScene(&scene);
}

static void StartCoOpGameFromMenu()
{
	dae::serviceLocator::GetGameDataManager().SetGameMode(dae::GameMode::Coop);
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto go = std::make_unique<dae::GameObject>();
	auto textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Lives = 5", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 100);
	auto livesTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Score = 0", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 150);
	auto scoreTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetPosition(220, 200);
	auto textureComponent = std::make_unique<dae::TextureComponent>(go.get());
	textureComponent->SetTexture("cldig1.png");
	textureComponent->SetDrawSize(32, 32);
	auto* textureComponentPtr2 = textureComponent.get();
	go->addComponent(std::move(textureComponent));
	auto playerComponent = std::make_unique<dae::PlayerComponent>(go.get(), textureComponentPtr2);
	auto* playerComponentPtr2 = playerComponent.get();
	go->addComponent(std::move(playerComponent));

	dae::InputManager::GetInstance().AddCommand(SDLK_UP, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 0.0f, -10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_DOWN, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 0.0f, 10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_LEFT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, -10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_RIGHT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_RSHIFT, std::make_unique<dae::shootCommand>(go.get(), playerComponentPtr2));

	auto scoreComponent2 = std::make_unique<dae::ScoreComponent>(go.get(), playerComponentPtr2, scoreTextComponentPtr);
	auto* scoreComponentPtr2 = scoreComponent2.get();
	go->addComponent(std::move(scoreComponent2));

	auto lifeComponent2 = std::make_unique<dae::LifeComponent>(go.get(), playerComponentPtr2, livesTextComponentPtr);
	auto* lifeComponentPtr2 = lifeComponent2.get();
	go->addComponent(std::move(lifeComponent2));

	auto deathComponent2 = std::make_unique<dae::DeathComponent>(go.get());
	auto* deathComponentPtr2 = deathComponent2.get();
	go->addComponent(std::move(deathComponent2));

	playerComponentPtr2->AddObserver(scoreComponentPtr2);
	playerComponentPtr2->AddObserver(lifeComponentPtr2);
	playerComponentPtr2->AddObserver(deathComponentPtr2);

	auto currentGameMode = dae::serviceLocator::GetGameDataManager().GetGameMode();

	auto hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Player, glm::vec2(32, 32), glm::vec2(0, 0));
	dae::HitBoxComponent* hitboxComponentPtr2{};
	if (currentGameMode == dae::GameMode::Versus) {
		hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Enemy, glm::vec2(32, 32), glm::vec2(0, 0));
		hitboxComponentPtr2 = hitboxComponent.get();
	}
	else {
		hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Player, glm::vec2(32, 32), glm::vec2(0, 0));
		hitboxComponentPtr2 = hitboxComponent.get();
	}
	go->addComponent(std::move(hitboxComponent));

	auto playerDamageComponent = std::make_unique<dae::PlayerDamageComponent>(go.get(), playerComponentPtr2, hitboxComponentPtr2);
	go->addComponent(std::move(playerDamageComponent));

	auto playerAccessor = dae::serviceLocator::GetPlayerAccessor();
	playerAccessor.AddPlayer(playerComponentPtr2);

	scene.Add(std::move(go));
	dae::SceneManager::GetInstance().SetActiveScene(&scene);
}

static void StartVersusGameFromMenu()
{
	dae::serviceLocator::GetGameDataManager().SetGameMode(dae::GameMode::Versus);
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto go = std::make_unique<dae::GameObject>();
	auto textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Lives = 5", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 100);
	auto livesTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Score = 0", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 150);
	auto scoreTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetPosition(220, 200);
	auto textureComponent = std::make_unique<dae::TextureComponent>(go.get());
	textureComponent->SetTexture("cldig1.png");
	textureComponent->SetDrawSize(32, 32);
	auto* textureComponentPtr2 = textureComponent.get();
	go->addComponent(std::move(textureComponent));
	auto playerComponent = std::make_unique<dae::PlayerComponent>(go.get(), textureComponentPtr2);
	auto* playerComponentPtr2 = playerComponent.get();
	go->addComponent(std::move(playerComponent));

	dae::InputManager::GetInstance().AddCommand(SDLK_UP, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 0.0f, -10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_DOWN, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 0.0f, 10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_LEFT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, -10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_RIGHT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_RSHIFT, std::make_unique<dae::shootCommand>(go.get(), playerComponentPtr2));

	auto scoreComponent2 = std::make_unique<dae::ScoreComponent>(go.get(), playerComponentPtr2, scoreTextComponentPtr);
	auto* scoreComponentPtr2 = scoreComponent2.get();
	go->addComponent(std::move(scoreComponent2));

	auto lifeComponent2 = std::make_unique<dae::LifeComponent>(go.get(), playerComponentPtr2, livesTextComponentPtr);
	auto* lifeComponentPtr2 = lifeComponent2.get();
	go->addComponent(std::move(lifeComponent2));

	auto deathComponent2 = std::make_unique<dae::DeathComponent>(go.get());
	auto* deathComponentPtr2 = deathComponent2.get();
	go->addComponent(std::move(deathComponent2));

	playerComponentPtr2->AddObserver(scoreComponentPtr2);
	playerComponentPtr2->AddObserver(lifeComponentPtr2);
	playerComponentPtr2->AddObserver(deathComponentPtr2);

	auto currentGameMode = dae::serviceLocator::GetGameDataManager().GetGameMode();

	auto hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Player, glm::vec2(32, 32), glm::vec2(0, 0));
	dae::HitBoxComponent* hitboxComponentPtr2{};
	if (currentGameMode == dae::GameMode::Versus) {
		hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Enemy, glm::vec2(32, 32), glm::vec2(0, 0));
		hitboxComponentPtr2 = hitboxComponent.get();
	}
	else {
		hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Player, glm::vec2(32, 32), glm::vec2(0, 0));
		hitboxComponentPtr2 = hitboxComponent.get();
	}
	go->addComponent(std::move(hitboxComponent));

	auto playerDamageComponent = std::make_unique<dae::PlayerDamageComponent>(go.get(), playerComponentPtr2, hitboxComponentPtr2);
	go->addComponent(std::move(playerDamageComponent));

	auto playerAccessor = dae::serviceLocator::GetPlayerAccessor();
	playerAccessor.AddPlayer(playerComponentPtr2);

	scene.Add(std::move(go));
	dae::SceneManager::GetInstance().SetActiveScene(&scene);
}

static void ExitGameFromMenu()
{
	std::exit(0);
}

static void LowerLetter(int initialIndex)
{
	switch (initialIndex)
	{
	case 0:
		if (letterSwitchComponentPtr1)
			letterSwitchComponentPtr1->PreviousLetter();
		break;
	case 1:
		if (letterSwitchComponentPtr2)
			letterSwitchComponentPtr2->PreviousLetter();
		break;
	case 2:
		if (letterSwitchComponentPtr3)
			letterSwitchComponentPtr3->PreviousLetter();
		break;
	}
}

static void RaiseLetter(int initialIndex)
{
	switch (initialIndex)
	{
	case 0:
		if (letterSwitchComponentPtr1)
			letterSwitchComponentPtr1->NextLetter();
		break;
	case 1:
		if (letterSwitchComponentPtr2)
			letterSwitchComponentPtr2->NextLetter();
		break;
	case 2:
		if (letterSwitchComponentPtr3)
			letterSwitchComponentPtr3->NextLetter();
		break;
	}
}

static void SaveScore()
{
	auto& gameDataManager = dae::serviceLocator::GetGameDataManager();
	std::string name;
	if (letterSwitchComponentPtr1)
		name += letterSwitchComponentPtr1->GetLetter();
	if (letterSwitchComponentPtr2)
		name += letterSwitchComponentPtr2->GetLetter();
	if (letterSwitchComponentPtr3)
		name += letterSwitchComponentPtr3->GetLetter();

	bool fileExists = fs::exists(gameDataManager.GetHighScorePath());

	std::ofstream file(gameDataManager.GetHighScorePath(), std::ios::app);
	if (file.is_open())
	{
		file << name << " - " << gameDataManager.GetCurrentScore() << std::endl;
		file.close();
	}

	dae::SceneManager::GetInstance().SetActiveScene(&HighScoreScreen);
	dae::InputManager::GetInstance().ChangeMenuCommandContext(2);
}

static void load() {
	dae::serviceLocator::RegisterGameDataManager(std::make_unique<dae::GameDataManager>());
	dae::serviceLocator::RegisterSoundSystem(std::make_unique<dae::SdlSoundSystem>());
	dae::serviceLocator::RegisterPlayerAccessor(std::make_unique<dae::PlayerAccessor>());
	dae::serviceLocator::RegisterEnemySpawnManager(std::make_unique<dae::EnemySpawnManager>());
	dae::serviceLocator::RegisterGamePadSystem(std::make_unique<dae::SdlGamePadSystem>());

	auto& gamePadSystem = dae::serviceLocator::GetGamePadSystem();
	gamePadSystem.Initialize();

	dae::SceneManager::GetInstance().SetActiveScene(&menuScene);

	auto levelManager = &dae::LevelManager::GetInstance();

	auto menuGo = std::make_unique<dae::GameObject>();

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Start Game", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(150, 200);
	auto* startTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	auto buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), startTextComponentPtr, StartGameFromMenu);
	auto* startButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Start Co-op", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(400, 200);
	auto* coOpTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), coOpTextComponentPtr, StartCoOpGameFromMenu);
	auto* coOpButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Start VS", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(650, 200);
	auto* vsTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), vsTextComponentPtr, StartVersusGameFromMenu);
	auto* vsButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Exit", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(450, 350);
	auto* exitTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), exitTextComponentPtr, ExitGameFromMenu);
	auto* exitButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	auto menuComponent = std::make_unique<dae::MenuComponent>(menuGo.get(), &menuScene);
	menuComponent->AddMenuItem(startButtonComponentPtr, 0);
	menuComponent->AddMenuItem(coOpButtonComponentPtr, 0);
	menuComponent->AddMenuItem(vsButtonComponentPtr, 0);
	menuComponent->AddMenuItem(exitButtonComponentPtr, 1);

	menuComponent->SetSelectedItem(startButtonComponentPtr);

	dae::InputManager::GetInstance().ChangeMenuCommandContext(0);
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_UP, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, -1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_DOWN, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, 1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_LEFT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), -1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RIGHT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RETURN, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_UP, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, -1.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_DOWN, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, 1.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_LEFT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), -1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_SOUTH, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));

	menuGo->addComponent(std::move(menuComponent));

	menuScene.Add(std::move(menuGo));

	menuGo = std::make_unique<dae::GameObject>();

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Game Over", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(350, 200);
	menuGo->addComponent(std::move(menuTextComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), ">", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(400, 250);
	auto* nextTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), nextTextComponentPtr, []() { RaiseLetter(0); });
	auto* nextButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(350, 250);
	auto* Initial1TextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	auto letterSwitchComponent = std::make_unique<dae::LetterSwitchComponent>(menuGo.get(), 'A', Initial1TextComponentPtr);
	letterSwitchComponentPtr1 = letterSwitchComponent.get();
	menuGo->addComponent(std::move(letterSwitchComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "<", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(300, 250);
	auto* previousTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), previousTextComponentPtr, []() { LowerLetter(0); });
	auto* previousButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), ">", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(550, 250);
	auto* nextTextComponentPtr2 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), nextTextComponentPtr2, []() { RaiseLetter(1); });
	auto* nextButtonComponentPtr2 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(500, 250);
	auto* Initial2TextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	letterSwitchComponent = std::make_unique<dae::LetterSwitchComponent>(menuGo.get(), 'A', Initial2TextComponentPtr);
	letterSwitchComponentPtr2 = letterSwitchComponent.get();
	menuGo->addComponent(std::move(letterSwitchComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "<", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(450, 250);
	auto* previousTextComponentPtr2 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), previousTextComponentPtr2, []() { LowerLetter(1); });
	auto* previousButtonComponentPtr2 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), ">", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(700, 250);
	auto* nextTextComponentPtr3 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), nextTextComponentPtr3, []() { RaiseLetter(2); });
	auto* nextButtonComponentPtr3 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(650, 250);
	auto* Initial3TextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	letterSwitchComponent = std::make_unique<dae::LetterSwitchComponent>(menuGo.get(), 'A', Initial3TextComponentPtr);
	letterSwitchComponentPtr3 = letterSwitchComponent.get();
	menuGo->addComponent(std::move(letterSwitchComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "<", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(600, 250);
	auto* previousTextComponentPtr3 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), previousTextComponentPtr3, []() { LowerLetter(2); });
	auto* previousButtonComponentPtr3 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Submit Score", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(500, 300);
	auto* SubmitScoreTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), SubmitScoreTextComponentPtr, SaveScore);
	auto* SubmitScoreButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuComponent = std::make_unique<dae::MenuComponent>(menuGo.get(), &ScoreSaveScreen);
	menuComponent->AddMenuItem(previousButtonComponentPtr, 0);
	menuComponent->AddMenuItem(nextButtonComponentPtr, 0);
	menuComponent->AddMenuItem(previousButtonComponentPtr2, 0);
	menuComponent->AddMenuItem(nextButtonComponentPtr2, 0);
	menuComponent->AddMenuItem(previousButtonComponentPtr3, 0);
	menuComponent->AddMenuItem(nextButtonComponentPtr3, 0);
	menuComponent->AddMenuItem(SubmitScoreButtonComponentPtr, 1);

	menuComponent->SetSelectedItem(previousButtonComponentPtr);

	dae::InputManager::GetInstance().ChangeMenuCommandContext(1);
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_UP, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, -1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_DOWN, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, 1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_LEFT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), -1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RIGHT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RETURN, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_UP, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, -1.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_DOWN, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, 1.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_LEFT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), -1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_SOUTH, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));

	menuGo->addComponent(std::move(menuComponent));

	ScoreSaveScreen.Add(std::move(menuGo));

	menuGo = std::make_unique<dae::GameObject>();
	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "High Scores", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(350, 50);
	menuGo->addComponent(std::move(menuTextComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(350, 100);
	pHighScoreTextComponent = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Restart Game", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(350, 500);
	auto* restartTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), restartTextComponentPtr, ReturnToStartMenu);
	auto* restartButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuComponent = std::make_unique<dae::MenuComponent>(menuGo.get(), &HighScoreScreen);
	menuComponent->AddMenuItem(restartButtonComponentPtr, 0);

	menuComponent->SetSelectedItem(restartButtonComponentPtr);

	dae::InputManager::GetInstance().ChangeMenuCommandContext(2);
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RETURN, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));
	dae::InputManager::GetInstance().AddMenuCommandController(SDL_GAMEPAD_BUTTON_SOUTH, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));

	menuGo->addComponent(std::move(menuComponent));

	HighScoreScreen.Add(std::move(menuGo));

    #if __EMSCRIPTEN__
	levelManager->LoadLevel("levelData/1.txt", &scene);
	#else
	levelManager->LoadLevel("Data/levelData/1.txt", &scene);
	#endif

	auto& soundSystem = dae::serviceLocator::GetSoundSystem();
	auto& playerAccessor = dae::serviceLocator::GetPlayerAccessor();
	soundSystem.registerSound(1, "Data/media/audio/0.mp3");
	soundSystem.registerSound(2, "Data/media/audio/MainTheme.mp3");
	soundSystem.registerSound(3, "Data/media/audio/BonusTheme.mp3");
	soundSystem.registerSound(4, "Data/media/audio/DeathSound.mp3");
	soundSystem.registerSound(5, "Data/media/audio/LevelComplete.mp3");

	//player 1

	auto go = std::make_unique<dae::GameObject>();
	auto textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Lives = 5", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 0);
	auto* livesTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Score = 0", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 50);
	auto* scoreTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetPosition(180, 200);
	auto textureComponent = std::make_unique<dae::TextureComponent>(go.get());
	textureComponent->SetTexture("cldig1.png");
	textureComponent->SetDrawSize(32, 32);
	auto* textureComponentPtr = textureComponent.get();
	go->addComponent(std::move(textureComponent));
	auto playerComponent = std::make_unique<dae::PlayerComponent>(go.get(), textureComponentPtr);
	auto* playerComponentPtr = playerComponent.get();
	go->addComponent(std::move(playerComponent));
	
	dae::InputManager::GetInstance().AddCommand(SDLK_W, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 0.0f, -10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_S, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 0.0f, 10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_A, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, -10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_D, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_SPACE, std::make_unique<dae::shootCommand>(go.get(), playerComponentPtr));
	dae::InputManager::GetInstance().AddCommandController(SDL_GAMEPAD_BUTTON_DPAD_UP, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 0.0f, -10.0f));
	dae::InputManager::GetInstance().AddCommandController(SDL_GAMEPAD_BUTTON_DPAD_DOWN, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 0.0f, 10.0f));
	dae::InputManager::GetInstance().AddCommandController(SDL_GAMEPAD_BUTTON_DPAD_LEFT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, -10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommandController(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 10.0f, 0.0f));

	auto scoreComponent = std::make_unique<dae::ScoreComponent>(go.get(), playerComponentPtr, scoreTextComponentPtr);
	auto* scoreComponentPtr = scoreComponent.get();
	go->addComponent(std::move(scoreComponent));

	auto lifeComponent = std::make_unique<dae::LifeComponent>(go.get(), playerComponentPtr, livesTextComponentPtr);
	auto* lifeComponentPtr = lifeComponent.get();
	go->addComponent(std::move(lifeComponent));

	auto deathComponent = std::make_unique<dae::DeathComponent>(go.get());
	auto* deathComponentPtr = deathComponent.get();
	go->addComponent(std::move(deathComponent));

	playerComponentPtr->AddObserver(scoreComponentPtr);
	playerComponentPtr->AddObserver(lifeComponentPtr);
	playerComponentPtr->AddObserver(deathComponentPtr);

	auto hitboxComponent = std::make_unique<dae::HitBoxComponent>(go.get(), dae::HitboxLayer::Player, glm::vec2(32, 32), glm::vec2(0, 0));
	auto* hitboxComponentPtr = hitboxComponent.get();
	go->addComponent(std::move(hitboxComponent));

	auto playerDamageComponent = std::make_unique<dae::PlayerDamageComponent>(go.get(), playerComponentPtr, hitboxComponentPtr);
	go->addComponent(std::move(playerDamageComponent));

	playerAccessor.AddPlayer(playerComponentPtr);

	scene.Add(std::move(go));

	//enemy spawn manager
	go = std::make_unique<dae::GameObject>();
	auto enemySpawnManager = &dae::serviceLocator::GetEnemySpawnManager();
	enemySpawnManager->Init(&scene, playerComponentPtr);
	auto enemySpawnUpdateComponent = std::make_unique<dae::EnemySpawnUpdateComponent>(go.get());
	go->addComponent(std::move(enemySpawnUpdateComponent));
	scene.Add(std::move(go));

	//fps
	go = std::make_unique<dae::GameObject>();
	auto tcFpscounter = std::make_unique<dae::TextComponent>(go.get(), "FPS: 0", font.get(), SDL_Color{ 255, 255, 255, 255 });
	auto fpsComponent = std::make_unique<dae::FPSComponent>(go.get());
	tcFpscounter->SetPosition(900, 10);
	go->addComponent(std::move(tcFpscounter));
	go->addComponent(std::move(fpsComponent));
	scene.Add(std::move(go));

	//explanation
	go = std::make_unique<dae::GameObject>();
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "Use wasd and arrows to move.", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 200);
	textComponent->SetScale(0.5f);
	go->addComponent(std::move(textComponent));
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "Use space and Rshift to damage", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 230);
	textComponent->SetScale(0.5f);
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	dae::InputManager::GetInstance().ChangeMenuCommandContext(0);
	soundSystem.playSound(2, 0.25f, true);
	dae::InputManager::GetInstance().AddCommand(SDLK_F7, std::make_unique<dae::SwitchMuteSoundCommand>());
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif

	dae::Minigin engine(data_location);
	engine.Run(load);

	return 0;
}
