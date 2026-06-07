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

namespace fs = std::filesystem;

auto& menuScene = dae::SceneManager::GetInstance().CreateScene();
auto& ScoreSaveScreen = dae::SceneManager::GetInstance().CreateScene();
auto& HighScoreScreen = dae::SceneManager::GetInstance().CreateScene();
auto& scene = dae::SceneManager::GetInstance().CreateScene();

static void StartGameFromMenu()
{
	dae::SceneManager::GetInstance().SetActiveScene(&scene);
}

static void ExitGameFromMenu()
{
	std::exit(0);
}

static void load() {
	dae::serviceLocator::RegisterSoundSystem(std::make_unique<dae::SdlSoundSystem>());
	dae::serviceLocator::RegisterPlayerAccessor(std::make_unique<dae::PlayerAccessor>());
	dae::serviceLocator::RegisterEnemySpawnManager(std::make_unique<dae::EnemySpawnManager>());
	dae::SceneManager::GetInstance().SetActiveScene(&ScoreSaveScreen);

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

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), coOpTextComponentPtr, StartGameFromMenu);
	auto* coOpButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "Start VS", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(650, 200);
	auto* vsTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), vsTextComponentPtr, StartGameFromMenu);
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

	dae::InputManager::GetInstance().AddMenuCommand(SDLK_UP, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, -1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_DOWN, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, 1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_LEFT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), -1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RIGHT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RETURN, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));

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

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), nextTextComponentPtr, StartGameFromMenu);
	auto* nextButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(350, 250);
	auto* Initial1TextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	auto letterSwitchComponent = std::make_unique<dae::LetterSwitchComponent>(menuGo.get(), 'a', Initial1TextComponentPtr);
	menuGo->addComponent(std::move(letterSwitchComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "<", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(300, 250);
	auto* previousTextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), previousTextComponentPtr, StartGameFromMenu);
	auto* previousButtonComponentPtr = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), ">", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(550, 250);
	auto* nextTextComponentPtr2 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), nextTextComponentPtr2, StartGameFromMenu);
	auto* nextButtonComponentPtr2 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(500, 250);
	auto* Initial2TextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	letterSwitchComponent = std::make_unique<dae::LetterSwitchComponent>(menuGo.get(), 'a', Initial2TextComponentPtr);
	menuGo->addComponent(std::move(letterSwitchComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "<", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(450, 250);
	auto* previousTextComponentPtr2 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), previousTextComponentPtr2, StartGameFromMenu);
	auto* previousButtonComponentPtr2 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), ">", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(700, 250);
	auto* nextTextComponentPtr3 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), nextTextComponentPtr3, StartGameFromMenu);
	auto* nextButtonComponentPtr3 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(650, 250);
	auto* Initial3TextComponentPtr = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	letterSwitchComponent = std::make_unique<dae::LetterSwitchComponent>(menuGo.get(), 'a', Initial3TextComponentPtr);
	menuGo->addComponent(std::move(letterSwitchComponent));

	menuTextComponent = std::make_unique<dae::TextComponent>(menuGo.get(), "<", font.get(), SDL_Color{ 255, 255, 255, 255 });
	menuTextComponent->SetPosition(600, 250);
	auto* previousTextComponentPtr3 = menuTextComponent.get();
	menuGo->addComponent(std::move(menuTextComponent));

	buttonComponent = std::make_unique<dae::ButtonComponent>(menuGo.get(), previousTextComponentPtr3, StartGameFromMenu);
	auto* previousButtonComponentPtr3 = buttonComponent.get();
	menuGo->addComponent(std::move(buttonComponent));

	// Create a menu component specifically for the score save screen (was missing, caused crash when reused)
	menuComponent = std::make_unique<dae::MenuComponent>(menuGo.get(), &ScoreSaveScreen);
	menuComponent->AddMenuItem(previousButtonComponentPtr, 0);
	menuComponent->AddMenuItem(nextButtonComponentPtr, 0);
	menuComponent->AddMenuItem(previousButtonComponentPtr2, 0);
	menuComponent->AddMenuItem(nextButtonComponentPtr2, 0);
	menuComponent->AddMenuItem(previousButtonComponentPtr3, 0);
	menuComponent->AddMenuItem(nextButtonComponentPtr3, 0);

	menuComponent->SetSelectedItem(previousButtonComponentPtr);

	// Register menu input commands for this menu
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_UP, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, -1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_DOWN, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 0.f, 1.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_LEFT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), -1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RIGHT, std::make_unique<dae::MenuMoveCommand>(menuGo.get(), menuComponent.get(), 1.f, 0.f));
	dae::InputManager::GetInstance().AddMenuCommand(SDLK_RETURN, std::make_unique<dae::MenuSelectCommand>(menuGo.get(), menuComponent.get()));

	menuGo->addComponent(std::move(menuComponent));

	ScoreSaveScreen.Add(std::move(menuGo));

    #if __EMSCRIPTEN__
	levelManager->LoadLevel("levelData/1.txt", &scene);
	#else
	levelManager->LoadLevel("Data/levelData/1.txt", &scene);
	#endif

	auto& soundSystem = dae::serviceLocator::GetSoundSystem();
	auto& playerAccessor = dae::serviceLocator::GetPlayerAccessor();
	soundSystem.registerSound(1, "Data/media/audio/0.mp3");

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
	go->addComponent(std::move(textureComponent));
	auto playerComponent = std::make_unique<dae::PlayerComponent>(go.get());
	auto* playerComponentPtr = playerComponent.get();
	go->addComponent(std::move(playerComponent));
	
	dae::InputManager::GetInstance().AddCommand(SDLK_W, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 0.0f, -10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_S, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 0.0f, 10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_A, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, -10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_D, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr, 10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_SPACE, std::make_unique<dae::damageCommand>(go.get(), playerComponentPtr));

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
	scene.Add(std::move(go));

	playerAccessor.AddPlayer(playerComponentPtr);

	//player 2

	go = std::make_unique<dae::GameObject>();
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Lives = 5", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 100);
	livesTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	textComponent = std::make_unique<dae::TextComponent>(go.get(), "#Score = 0", font.get(), SDL_Color{ 255, 255, 255, 255 });
	textComponent->SetPosition(0, 150);
	scoreTextComponentPtr = textComponent.get();
	go->addComponent(std::move(textComponent));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetPosition(220, 200);
	textureComponent = std::make_unique<dae::TextureComponent>(go.get());
	textureComponent->SetTexture("cldig1.png");
	go->addComponent(std::move(textureComponent));
	playerComponent = std::make_unique<dae::PlayerComponent>(go.get());
	auto* playerComponentPtr2 = playerComponent.get();
	go->addComponent(std::move(playerComponent));
	
	dae::InputManager::GetInstance().AddCommand(SDLK_UP, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 0.0f, -10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_DOWN, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 0.0f, 10.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_LEFT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, -10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_RIGHT, std::make_unique<dae::moveCommand>(go.get(), playerComponentPtr2, 10.0f, 0.0f));
	dae::InputManager::GetInstance().AddCommand(SDLK_RSHIFT, std::make_unique<dae::damageCommand>(go.get(), playerComponentPtr2));

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

	playerAccessor.AddPlayer(playerComponentPtr2);

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
}


//static void load()
//{
//	auto& scene = dae::SceneManager::GetInstance().CreateScene();
//
//
//	auto go = std::make_unique<dae::GameObject>();
//	auto textureComponent = std::make_unique<dae::TextureComponent>(go.get());
//	textureComponent->SetTexture("background.png");
//	go->addComponent(std::move(textureComponent));
//	scene.Add(std::move(go));
//
//	go = std::make_unique<dae::GameObject>();
//	textureComponent = std::make_unique<dae::TextureComponent>(go.get());
//	textureComponent->SetTexture("logo.png");
//	go->SetPosition(358, 180);
//	go->addComponent(std::move(textureComponent));
//	scene.Add(std::move(go));
//
//	go = std::make_unique<dae::GameObject>();
//	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
//	auto tc = std::make_unique<dae::TextComponent>(go.get(), "Programming 4 Assignment", font.get(), SDL_Color{ 255, 255, 0, 255 });
//	tc->SetPosition(292, 20);
//	go->addComponent(std::move(tc));
//	scene.Add(std::move(go));
//
//	go = std::make_unique<dae::GameObject>();
//	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
//	auto tcFpscounter = std::make_unique<dae::TextComponent>(go.get(), "FPS: 0", font.get(), SDL_Color{ 255, 255, 255, 255 });
//	auto fpsComponent = std::make_unique<dae::FPSComponent>(go.get());
//	tcFpscounter->SetPosition(10, 10);
//	go->addComponent(std::move(tcFpscounter));
//	go->addComponent(std::move(fpsComponent));
//	scene.Add(std::move(go));
//
//	 Test for Command
//	go = std::make_unique<dae::GameObject>();
//	go->SetPosition(180, 200);
//	textureComponent = std::make_unique<dae::TextureComponent>(go.get());
//	textureComponent->SetTexture("cldig1.png");
//	dae::InputManager::GetInstance().AddCommand(SDLK_W, std::make_unique<dae::moveCommand>(go.get(), 0.0f, -10.0f));
//	dae::InputManager::GetInstance().AddCommand(SDLK_S, std::make_unique<dae::moveCommand>(go.get(), 0.0f, 10.0f));
//	dae::InputManager::GetInstance().AddCommand(SDLK_A, std::make_unique<dae::moveCommand>(go.get(), -10.0f, 0.0f));
//	dae::InputManager::GetInstance().AddCommand(SDLK_D, std::make_unique<dae::moveCommand>(go.get(), 10.0f, 0.0f));
//	go->addComponent(std::move(textureComponent));
//	scene.Add(std::move(go));
//
//	 Test for Command with controller input (D-Pad)
//	go = std::make_unique<dae::GameObject>();
//	go->SetPosition(120, 200);
//	textureComponent = std::make_unique<dae::TextureComponent>(go.get());
//	textureComponent->SetTexture("cldig1.png");
//	dae::InputManager::GetInstance().AddCommand(0x0001, std::make_unique<dae::moveCommand>(go.get(), 0.0f, -20.0f));
//	dae::InputManager::GetInstance().AddCommand(0x0002, std::make_unique<dae::moveCommand>(go.get(), 0.0f, 20.0f));
//	dae::InputManager::GetInstance().AddCommand(0x0004, std::make_unique<dae::moveCommand>(go.get(), -20.0f, 0.0f));
//	dae::InputManager::GetInstance().AddCommand(0x0008, std::make_unique<dae::moveCommand>(go.get(), 20.0f, 0.0f));
//	go->addComponent(std::move(textureComponent));
//	scene.Add(std::move(go));*/
//
//	Test for RotateAroundPivotComponent
//	go = std::make_unique<dae::GameObject>();
//	go->SetPosition(180, 200);
//	dae::GameObject* parentGo = go.get();
//	scene.Add(std::move(go));
//
//	go = std::make_unique<dae::GameObject>();
//	textureComponent = std::make_unique<dae::TextureComponent>(go.get());
//	textureComponent->SetTexture("cldig1.png");
//	go->SetPosition(10, 10);
//	go->addComponent(std::move(textureComponent));
//	auto rapComponent = std::make_unique<dae::RotateAroundPivotComponent>(go.get(), glm::radians(360.f));
//	go->addComponent(std::move(rapComponent));
//	go->SetParent(parentGo, false);
//	parentGo = go.get();
//	scene.Add(std::move(go));
//
//	go = std::make_unique<dae::GameObject>();
//	textureComponent = std::make_unique<dae::TextureComponent>(go.get());
//	textureComponent->SetTexture("cldig1.png");
//	go->SetPosition(50, 0);
//	go->SetParent(parentGo, false);
//	go->addComponent(std::move(textureComponent));
//	rapComponent = std::make_unique<dae::RotateAroundPivotComponent>(go.get(), glm::radians(-360.f));
//	go->addComponent(std::move(rapComponent));
//	scene.Add(std::move(go));
//}

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
