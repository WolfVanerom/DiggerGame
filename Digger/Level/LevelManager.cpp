#include "LevelManager.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "Scene.h"
#include "GameObject.h"
#include "TileMapComponent.h"
#include "TextureComponent.h"
#include "EmeraldComponent.h"
#include "GoldComponent.h"
#include <SDL3/SDL_log.h>
#include <ProjectileComponent.h>
#include <Enemy/EnemySpawnManager.h>
#include <HitBoxComponent.h>
#include <SceneManager.h>

void dae::LevelManager::CreateCurrentNonEntityDrawObject(Scene* scene)
{
	if (scene == nullptr or m_currentLevel.empty())
	{
		return;
	}

	if (m_currentNonEntetyDraw != nullptr)
	{
		m_currentNonEntetyDraw->MarkForDeletion();
		m_currentNonEntetyDraw = nullptr;
	}

	auto go = std::make_unique<GameObject>();
	auto tileMapComponent = std::make_unique<TileMapComponent>(go.get(), m_tileWidth, m_tileHeight);

	m_tileObjects.assign(m_maxHeight, std::vector<LevelObjectType>(m_maxWidth, LevelObjectType::empty));
	for (int y = 0; y < m_maxHeight; ++y)
	{
		for (int x = 0; x < m_maxWidth; ++x)
		{
         m_tileObjects[y][x] = CharToType(m_currentLevel[y][x]);
		}
	}

	tileMapComponent->SetTiles(&m_tileObjects);
	tileMapComponent->SetTileTexture(LevelObjectType::tunnelEnd, std::string(GetTextureForType(LevelObjectType::tunnelEnd)));
	tileMapComponent->SetTileTexture(LevelObjectType::horizontalTunnel, std::string(GetTextureForType(LevelObjectType::horizontalTunnel)));
	tileMapComponent->SetTileTexture(LevelObjectType::verticalTunnel, std::string(GetTextureForType(LevelObjectType::verticalTunnel)));

	go->addComponent(std::move(tileMapComponent));
	m_currentNonEntetyDraw = go.get();
	scene->InsertAt(1, std::move(go));
}

void dae::LevelManager::CreateCurrentBackgroundObject(Scene* scene)
{
	if (scene == nullptr)
	{
		return;
	}

	if (m_currentBackgroundObject != nullptr)
	{
		m_currentBackgroundObject->MarkForDeletion();
		m_currentBackgroundObject = nullptr;
	}

	auto go = std::make_unique<GameObject>();
	auto textureComponent = std::make_unique<TextureComponent>(go.get());
	textureComponent->SetTexture("media/cback1.png");
	textureComponent->SetTiling(true);
	textureComponent->SetTileArea(m_windowWidth, m_windowHeight);
	
	go->addComponent(std::move(textureComponent));
	m_currentBackgroundObject = go.get();
	scene->InsertAt(0, std::move(go));
}

void dae::LevelManager::SpawnLevelObject(LevelObjectType type, int x, int y, Scene* scene)
{
	if (type == LevelObjectType::emerald){
		if (scene == nullptr or m_currentLevel.empty() or !IsInBounds(x, y))
		{
			return;
		}

		auto go = std::make_unique<GameObject>();

		go->SetPosition(x * m_tileWidth, y * m_tileHeight);

		auto textureComponent = std::make_unique<TextureComponent>(go.get());
		textureComponent->SetTexture(std::string(GetTextureForType(type)));
		textureComponent->SetDrawSize(m_tileWidth, m_tileHeight);
		go->addComponent(std::move(textureComponent));

		auto emeraldComponent = std::make_unique<EmeraldComponent>(go.get());
		go->addComponent(std::move(emeraldComponent));

		m_EntityObjects[y][x] = go.get();
		scene->Add(std::move(go));
		m_amountOfEmeralds++;
	}
	else if (type == LevelObjectType::bag)
	{
		if (scene == nullptr or m_currentLevel.empty() or !IsInBounds(x, y))
		{
			return;
		}

		auto go = std::make_unique<GameObject>();

		go->SetPosition(x * m_tileWidth, y * m_tileHeight);

		auto textureComponent = std::make_unique<TextureComponent>(go.get());
		textureComponent->SetTexture(std::string(GetTextureForType(type)));
		textureComponent->SetDrawSize(m_tileWidth, m_tileHeight);
		go->addComponent(std::move(textureComponent));

		auto goldComponent = std::make_unique<GoldComponent>(go.get());
		go->addComponent(std::move(goldComponent));

		auto hitboxComponent = std::make_unique<HitBoxComponent>(go.get(), dae::HitboxLayer::Item, glm::vec2(m_tileWidth/4, m_tileHeight/4), glm::vec2(m_tileWidth/3, m_tileHeight/2));
		go->addComponent(std::move(hitboxComponent));

		m_EntityObjects[y][x] = go.get();
		scene->Add(std::move(go));
	}
}

void dae::LevelManager::CheckIfLevelCompleted()
{
	SDL_Log("Checking if level completed. Remaining emeralds: %d", m_amountOfEmeralds);
	if (m_amountOfEmeralds <= 0)
	{
		m_currentLevelIndex++;
		QueueLevelLoad("Data/levelData/" + std::to_string(m_currentLevelIndex) + ".txt", m_currentScene);
		return;
	}
	else {
		m_amountOfEmeralds--;
	}
}

dae::LevelObjectType dae::LevelManager::CharToType(char c) const
{
	switch (c)
	{
	case ' ':
		return LevelObjectType::empty;
	case 'T':
	case 'S':
		return LevelObjectType::tunnelEnd;
	case 'H':
		return LevelObjectType::horizontalTunnel;
	case 'V':
		return LevelObjectType::verticalTunnel;
	case 'B':
		return LevelObjectType::bag;
	case 'C':
		return LevelObjectType::emerald;
	default:
		return LevelObjectType::none;
	}
}

char dae::LevelManager::TypeToChar(LevelObjectType type) const
{
	switch (type)
	{
	case LevelObjectType::empty:
		return ' ';
	case LevelObjectType::tunnelEnd:
		return 'S';
	case LevelObjectType::horizontalTunnel:
		return 'H';
	case LevelObjectType::verticalTunnel:
		return 'V';
	case LevelObjectType::bag:
		return 'B';
	case LevelObjectType::emerald:
		return 'C';
	default:
		return ' ';
	}
}

std::string_view dae::LevelManager::GetTextureForType(LevelObjectType type) const
{
	static constexpr std::string_view emptyTexture{ "empty" };
	static constexpr std::string_view tunnelEndTexture{ "media/ENDTUNNEL.png" };
	static constexpr std::string_view horizontalTunnelTexture{ "media/blob1.png" };
	static constexpr std::string_view verticalTunnelTexture{ "media/blob2.png" };
	static constexpr std::string_view bagTexture{ "media/cfbag.png" };
	static constexpr std::string_view emeraldTexture{ "media/emerald.png" };

	switch (type)
	{
	case LevelObjectType::tunnelEnd:
		return tunnelEndTexture;
	case LevelObjectType::horizontalTunnel:
		return horizontalTunnelTexture;
	case LevelObjectType::verticalTunnel:
		return verticalTunnelTexture;
	case LevelObjectType::bag:
		return bagTexture;
	case LevelObjectType::emerald:
		return emeraldTexture;
	default:
		return emptyTexture;
	}
}

bool dae::LevelManager::IsInBounds(int x, int y) const
{
	return x >= 0 && x < m_maxWidth && y >= 0 && y < m_maxHeight;
}

void dae::LevelManager::LoadLevel(const std::string& levelFile, Scene* scene)
{
	m_currentScene = scene;

	m_enemySpawnManager.SetMaxEnemyCountForLevel(m_currentLevelIndex);

	m_amountOfEmeralds = -1;

	std::ifstream file(levelFile);
	if (!file.is_open())
	{
		std::cerr << "Failed to open level file: " << levelFile << '\n';
		return;
	}

	std::vector<std::string> loadedLevel;
	std::string line;
	while (std::getline(file, line))
	{
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}
		loadedLevel.push_back(line);
	}

	if (static_cast<int>(loadedLevel.size()) != m_maxHeight)
	{
		std::cerr << "Invalid level height in " << levelFile << ". Expected " << m_maxHeight
			<< ", got " << loadedLevel.size() << '\n';
		return;
	}

	for (int y = 0; y < m_maxHeight; ++y)
	{
		if (static_cast<int>(loadedLevel[y].size()) != m_maxWidth)
		{
			std::cerr << "Invalid level width in " << levelFile << " at row " << y
				<< ". Expected " << m_maxWidth << ", got " << loadedLevel[y].size() << '\n';
			return;
		}
	}

	m_currentLevel = std::move(loadedLevel);
	m_EntityObjects.assign(m_maxHeight, std::vector<GameObject*>(m_maxWidth, nullptr));
	m_projectileObjects.assign(m_maxHeight, std::vector<GameObject*>(m_maxWidth, nullptr));
	CreateCurrentBackgroundObject(scene);
	CreateCurrentNonEntityDrawObject(scene);
	for (int y = 0; y < m_maxHeight; ++y)
	{
		for (int x = 0; x < m_maxWidth; ++x)
		{
			const char c = m_currentLevel[y][x];
			const LevelObjectType type = CharToType(c);
			if (type == LevelObjectType::emerald or type == LevelObjectType::bag)
			{
				SpawnLevelObject(type, x, y, scene);
			}
		}
	}
}

void dae::LevelManager::ClearLevel()
{
	m_currentLevel.clear();
    m_tileObjects.clear();
	m_projectileObjects.clear();
	m_enemySpawnManager.ClearEnemies();
    if (!m_EntityObjects.empty())
	{
		for (auto& row : m_EntityObjects)
		{
			for (auto& object : row)
			{
				if (object != nullptr)
				{
					object->MarkForDeletion();
					object = nullptr;
				}
			}
		}
	}
	m_EntityObjects.clear();
	m_tunnelPreviews.clear();
}

dae::LevelObjectType dae::LevelManager::GetCell(int x, int y) const
{
	if (!IsInBounds(x, y) or m_currentLevel.empty())
	{
		return LevelObjectType::none;
	}
	char c = m_currentLevel[y][x];
	return CharToType(c);
}

dae::GameObject* dae::LevelManager::GetEntityObjectFromCell(int x, int y) const
{
	if (!IsInBounds(x, y) or m_currentLevel.empty())
	{
		return nullptr;
	}
	if (!m_EntityObjects.empty())
	{
		return m_EntityObjects[y][x];
	}
	return nullptr;
}

void dae::LevelManager::SetCell(int x, int y, LevelObjectType type)
{
	if (!IsInBounds(x, y) or m_currentLevel.empty())
	{
		return;
	}

	auto currentType = GetCell(x, y);

	if (currentType == LevelObjectType::emerald && !m_EntityObjects.empty() && m_EntityObjects[y][x] != nullptr)
	{
		m_EntityObjects[y][x]->MarkForDeletion();
		m_EntityObjects[y][x] = nullptr;
	}

	if (currentType == LevelObjectType::bag && !m_EntityObjects.empty() && m_EntityObjects[y][x] != nullptr)
	{
		auto goldComponent = static_cast<GoldComponent*>(m_EntityObjects[y][x]->getComponent(typeid(GoldComponent)));
		if (goldComponent != nullptr && goldComponent->HasBroken())
		{
			m_EntityObjects[y][x]->MarkForDeletion();
			m_EntityObjects[y][x] = nullptr;
		}
		else
		{
			return;
		}
	}

	m_currentLevel[y][x] = TypeToChar(type);
	if (!m_tileObjects.empty())
	{
		m_tileObjects[y][x] = type;
	}
}

void dae::LevelManager::MoveEntityCell(int fromX, int fromY, int toX, int toY, LevelObjectType newType, bool moveTexture)
{
	if (!IsInBounds(fromX, fromY) or !IsInBounds(toX, toY) or m_currentLevel.empty())
	{
		return;
	}

	if (!m_EntityObjects.empty() && m_EntityObjects[fromY][fromX] != nullptr)
	{
		m_EntityObjects[toY][toX] = m_EntityObjects[fromY][fromX];
		m_EntityObjects[fromY][fromX] = nullptr;
	}

	m_currentLevel[fromY][fromX] = TypeToChar(LevelObjectType::empty);
	m_currentLevel[toY][toX] = TypeToChar(newType);

	if (!m_tileObjects.empty() and moveTexture)
	{
		m_tileObjects[fromY][fromX] = LevelObjectType::empty;
		m_tileObjects[toY][toX] = newType;
	}
}

void dae::LevelManager::SetTunnelPreview(GameObject* player, int cellX, int cellY, LevelObjectType type, TunnelDirection direction, float progress)
{
	if (player == nullptr or !IsInBounds(cellX, cellY) or m_currentLevel.empty())
	{
		m_tunnelPreviews.erase(player);
		return;
	}

	auto& preview = m_tunnelPreviews[player];
	preview.active = true;
	preview.cellX = cellX;
	preview.cellY = cellY;
	preview.type = type;
	preview.direction = direction;
	preview.progress = std::clamp(progress, 0.f, 1.f);
}

void dae::LevelManager::ClearTunnelPreview(GameObject* player)
{
	if (player != nullptr)
	{
		m_tunnelPreviews.erase(player);
	}
}

std::vector<dae::TunnelPreview> dae::LevelManager::GetAllTunnelPreviews() const
{
	std::vector<TunnelPreview> previews;
	for (const auto& [player, preview] : m_tunnelPreviews)
	{
		if (preview.active && preview.type != LevelObjectType::none && preview.progress > 0.f)
		{
			previews.push_back(preview);
		}
	}
	return previews;
}

void dae::LevelManager::QueueLevelLoad(const std::string& levelFile, Scene* scene)
{
	if (m_currentLevelIndex >= 5) {
		m_soundSystem.pauseAllSounds(false);
		SceneManager::GetInstance().SetActiveScene(serviceLocator::GetGameDataManager().GetScoreSaveScene());
		return;
	}
	m_soundSystem.playSound(5, 0.25f, false);
	m_pendingLevelLoad = std::make_pair(levelFile, scene);
}

void dae::LevelManager::LowerEmeraldCount()
{
	m_amountOfEmeralds--;
}

int dae::LevelManager::GetCurrentLevelIndex() const
{
	return m_currentLevelIndex;
}

void dae::LevelManager::IncreaseCurrentLevelIndex()
{
	m_currentLevelIndex++;
}

void dae::LevelManager::ResetLevelForGameStart()
{
	m_currentLevelIndex = 1;
	QueueLevelLoad("Data/levelData/1.txt", m_currentScene);
}

dae::Scene* dae::LevelManager::GetCurrentScene() const
{
	return m_currentScene;
}

void dae::LevelManager::SpawnProjectileAt(int x, int y, TunnelDirection direction)
{
	if (!IsInBounds(x, y))
	{
		return;
	}
	auto go = std::make_unique<GameObject>();
	go->SetPosition(x * m_tileWidth, y * m_tileHeight);

	auto textureComponent = std::make_unique<TextureComponent>(go.get());
	textureComponent->SetTexture("media/cfire1.png");
	textureComponent->SetDrawSize(m_tileWidth, m_tileHeight);

	auto projectileComponent = std::make_unique<ProjectileComponent>(go.get(), direction);
	go->addComponent(std::move(textureComponent));
	go->addComponent(std::move(projectileComponent));

	auto hitboxComponent = std::make_unique<HitBoxComponent>(go.get(), dae::HitboxLayer::Item, glm::vec2(m_tileWidth / 4, m_tileHeight / 4), glm::vec2(m_tileWidth / 3, m_tileHeight / 2));
	go->addComponent(std::move(hitboxComponent));

	m_projectileObjects[y][x] = go.get();
	m_currentScene->Add(std::move(go));
}

bool dae::LevelManager::HasProjectileAt(int x, int y) const
{
	if (!IsInBounds(x, y) or m_currentLevel.empty())
	{
		return false;
	}
	return m_projectileObjects[y][x] != nullptr;
}

void dae::LevelManager::ProcessPendingLevelLoad()
{
	if (m_pendingLevelLoad.has_value())
	{
		ClearLevel();
		LoadLevel(m_pendingLevelLoad->first, m_pendingLevelLoad->second);
		m_pendingLevelLoad.reset();
	}
}
