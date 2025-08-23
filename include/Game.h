#pragma once

#include <memory>
#include <string>
#include <fstream>

#include "Entity.h"
#include "EntityManager.h"
#include "Vec2.h"

#include <SFML/Graphics.hpp>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
struct WindowConfig { int width, height, frameLimit, fullscreen;};
struct FontConfig {std::string file; int size; int R, G, B;};

struct GameConfig {
    WindowConfig window;
    FontConfig font;
    PlayerConfig player;
    EnemyConfig enemy;
    BulletConfig bullet;
};

class Game
{
private:
    // Core SFML members
    sf::RenderWindow              m_window;
    sf::Font                      m_font;
    sf::Clock                     m_specialClock;
    std::unique_ptr<sf::Text>     m_text;
    std::unique_ptr<sf::Text>     m_pausedText;

    // Game entities
    EntityManager                 m_entities;
    std::shared_ptr<Entity>       m_player;

    // Configuration structs
    PlayerConfig                  m_playerconfig;
    EnemyConfig                   m_enemyconfig;
    BulletConfig                  m_bulletconfig;
    GameConfig                    m_config;

    // Game state
    int                           m_score = 0;
    int                           m_currentFrame = 0;
    int                           m_lastEnemySpawnTime = 0;
    bool                          m_paused = false;
    bool                          m_running = true;
    float                         m_enemySpawnTimer = 0.0f;
    float                         m_specialCooldown = 0.0f;

    // Initialization
    void init(const std::string & config);

    // Systems
    void sMovement();
    void sUserInput();
    void sLifespan(float deltaTime);
    void sRender();
    void sEnemySpawner(float deltaTime);
    void sCollision();
    void sBounds();
    void sEnemyMovement();

    // Spawning
    void spawnPlayer();
    void spawnEnemy();
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target);
    void spawnSmallEnemies(std::shared_ptr<Entity> e);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2& target);

    // Utility
    void setPaused(bool paused);

public:
    explicit Game(const std::string & config);

    void run();
};
