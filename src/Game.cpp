#include "../include/Game.h"
#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <algorithm>

GameConfig loadConfig(const std::string& filename) {
    GameConfig cfg;
    std::ifstream file(filename);
    std::string type;

    while (file >> type) {
        auto& w = cfg.window;
        if (type == "Window") {
            file >> w.width >> w.height
                 >> w.frameLimit >> w.fullscreen;
        }
        else if (type == "Font") {
            file >> cfg.font.file >> cfg.font.size
                 >> cfg.font.R >> cfg.font.G >> cfg.font.B;
        }
        else if (type == "Player") {
            auto& p = cfg.player;
            file >> p.SR >> p.CR >> p.S
                 >> p.FR >> p.FG >> p.FB
                 >> p.OR >> p.OG >> p.OB
                 >> p.OT >> p.V;
        }
        else if (type == "Enemy") {
            auto& e = cfg.enemy;
            file >> e.SR >> e.CR >> e.SMIN >> e.SMAX
                 >> e.OR >> e.OG >> e.OB >> e.OT
                 >> e.VMIN >> e.VMAX >> e.L >> e.SI;
        }
        else if (type == "Bullet") {
            auto& b = cfg.bullet;
            file >> b.SR >> b.CR >> b.S
                 >> b.FR >> b.FG >> b.FB
                 >> b.OR >> b.OG >> b.OB
                 >> b.OT >> b.V  >> b.L;
        }
    }

    return cfg;
}

void Game::init(const std::string &path)
{
    m_config = loadConfig("../config/config.txt");

    m_window.create(sf::VideoMode(sf::Vector2u(m_config.window.width, m_config.window.height)), "Game");
    m_window.setFramerateLimit(m_config.window.frameLimit);

    spawnPlayer();
}

Game::Game(const std::string &config)
{
    m_config = loadConfig("../config/config.txt");

    if (!m_font.openFromFile(m_config.font.file))
        std::cerr << "[Error] Failed to load font!" << m_config.font.file << "\n";

    m_text = std::make_unique<sf::Text>(m_font, "Score: 0", m_config.font.size);
    m_text->setFillColor(sf::Color(m_config.font.R, m_config.font.G, m_config.font.B));

    init(config);
}

void Game::run()
{
    sf::Clock m_deltaClock;

    while (m_running && m_window.isOpen())
    {

        sf::Time dt = m_deltaClock.restart();
        float deltaTime = dt.asSeconds();

        sUserInput();

        if (!m_paused)
        {
            sMovement();
            sBounds();
            sEnemySpawner(deltaTime);
            sEnemyMovement();
            sCollision();
            sLifespan(deltaTime);

            m_entities.update();
        }

        sRender();

        if(m_paused)
        {
            m_window.display();
        }

        m_currentFrame++;
    }
}

void Game::setPaused(bool paused) {m_paused = paused;}


void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    float mx = m_window.getSize().x / 2.f;
    float my = m_window.getSize().y / 2.f;

    try
    {
        entity->cTransform = std::make_unique<CTransform>(Vec2(mx,my), Vec2(0.f,0.f), 0.f);

        entity->cShape = std::make_unique<CShape>(m_config.player.SR, m_config.player.V, sf::Color(m_config.player.FR, m_config.player.FG, m_config.player.FB), sf::Color(m_config.player.OR, m_config.player.OG, m_config.player.OB), static_cast<float>(m_config.player.OT));

        entity->cCollision = std::make_unique<CCollision>(m_config.player.CR);

        entity->cInput = std::make_unique<CInput>();

        entity->cBounds = std::make_unique<CBounds>(0.f, 0.f, static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y));
    }
    catch (const std::bad_alloc &e)
    {
        std::cerr << "[Malloc Error] " << e.what() << "\n";
        m_running = false;
        return;
    }

    m_player = entity;
}

void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dx(0.f, m_window.getSize().x);
    std::uniform_real_distribution<float> dy(0.f, m_window.getSize().y);

    Vec2 pos(dx(rng), dy(rng));
    entity->cTransform = std::make_unique<CTransform>(pos, Vec2(0.f, 0.f), 0.f);

    std::uniform_real_distribution<float> speedDist(m_config.enemy.SMIN, m_config.enemy.SMAX);
    float speed = speedDist(rng);

    std::uniform_real_distribution<float> dirDist(-1.f, 1.f);
    Vec2 velocity = { dirDist(rng) * speed, dirDist(rng) * speed };
    entity->cTransform->velocity = velocity;

    std::uniform_int_distribution<int> shapePoints(m_config.enemy.VMIN, m_config.enemy.VMAX);
    int numPoints = shapePoints(rng);

    std::uniform_int_distribution<int> color(0, 255);
    sf::Color fillColor(color(rng), color(rng), color(rng));
    sf::Color outlineColor(m_config.enemy.OR, m_config.enemy.OG, m_config.enemy.OB);

    entity->cShape = std::make_unique<CShape>(32.f, numPoints, fillColor, outlineColor, 4.f);
    entity->cCollision = std::make_unique<CCollision>(m_config.enemy.CR);
    entity->cBounds = std::make_unique<CBounds>(0.f, 0.f, m_window.getSize().x, m_window.getSize().y);
    entity->cLifespan = std::make_unique<CLifespan>(m_config.enemy.L);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    if (!e || !e->cTransform || !e->cShape || !e->cCollision) return;

    Vec2 origin = e->cTransform->pos;
    int vertices = static_cast<int>(e->cShape->circle.getPointCount());
    sf::Color color = e->cShape->circle.getFillColor();

    // Fixed speed for small enemies
    float speed = 2.f; // adjust as needed

    for (int i = 0; i < vertices; ++i)
    {
        float angleDeg = i * (360.f / vertices);
        float angleRad = angleDeg * 3.14159265f / 180.f;

        Vec2 velocity = { std::cos(angleRad) * speed, std::sin(angleRad) * speed };

        auto small = m_entities.addEntity("small_enemy");

        small->cTransform = std::make_unique<CTransform>(origin, velocity, 0.f);

        // Small enemies have same vertices and color as original
        small->cShape = std::make_unique<CShape>(e->cShape->circle.getRadius() / 2.f, vertices, color,
                                                e->cShape->circle.getOutlineColor(),
                                                e->cShape->circle.getOutlineThickness());

        // Optional: smaller collision radius
        small->cCollision = std::make_unique<CCollision>(e->cCollision->radius / 2.f);

        // Bounds same as parent
        small->cBounds = std::make_unique<CBounds>(0.f, 0.f,
                                                   static_cast<float>(m_window.getSize().x),
                                                   static_cast<float>(m_window.getSize().y));

        // Optional: small lifespan
        small->cLifespan = std::make_unique<CLifespan>(m_config.enemy.L * .2f); // e.g., 1 second at 60 FPS
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> player, const Vec2 &target)
{
    if (!player || !player->cTransform) return;

    auto bullet = m_entities.addEntity("bullet");

    Vec2 startPos = player->cTransform->pos;
    bullet->cTransform = std::make_unique<CTransform>(startPos, Vec2(0.f, 0.f), 0.f);

    Vec2 dir = { target.x - startPos.x, target.y - startPos.y };
    float length = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (length != 0.f)
        dir = { dir.x / length, dir.y / length };

    bullet->cTransform->velocity = { dir.x * m_config.bullet.S, dir.y * m_config.bullet.S };
    bullet->cShape = std::make_unique<CShape>(m_config.bullet.SR, m_config.bullet.V, sf::Color(m_config.bullet.FR, m_config.bullet.FG, m_config.bullet.FB), sf::Color(m_config.bullet.OR, m_config.bullet.OG , m_config.bullet.OB), m_config.bullet.OT);
    bullet->cCollision = std::make_unique<CCollision>(m_config.bullet.CR);
    bullet->cLifespan = std::make_unique<CLifespan>(m_config.bullet.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> player, const Vec2 &target)
{
    if (!player || !player->cTransform) return;

    auto bullet = m_entities.addEntity("special_bullet");

    Vec2 pos = player->cTransform->pos;
    bullet->cTransform = std::make_unique<CTransform>(pos, Vec2(0, 0), 0.0f);

    Vec2 dir = { target.x - pos.x, target.y - pos.y };
    float length = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (length != 0.f)
        dir = { dir.x / length, dir.y / length };

    bullet->cTransform->velocity = {dir.x * m_config.bullet.S, dir.y * m_config.bullet.S };
    bullet->cCollision = std::make_unique<CCollision>(8.0f);
    bullet->cShape = std::make_unique<CShape>(m_config.bullet.SR, 5, sf::Color::Yellow, sf::Color::Red, 1.0f);
    bullet->cLifespan = std::make_unique<CLifespan>(m_config.bullet.L);
}

void Game::sMovement()
{
    if (m_player && m_player->cTransform && m_player->cInput)
    {
        auto &t = *m_player->cTransform;
        auto &i = *m_player->cInput;

        t.velocity = {0.f, 0.f};
        if (i.up)    t.velocity.y = -5.f;
        if (i.down)  t.velocity.y = 5.f;
        if (i.left)  t.velocity.x = -5.f;
        if (i.right) t.velocity.x = 5.f;

        t.pos.x += t.velocity.x;
        t.pos.y += t.velocity.y;
    }

    for (auto &e : m_entities.getEntities())
    {
        if (!e || !e->cTransform) continue;
        if (e == m_player) continue;

        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }
}

void Game::sEnemyMovement()
{
    for (auto &e : m_entities.getEntities("enemy"))
    {
        if (!e || !e->cTransform || !e->cBounds || !e->cShape) continue;

        auto &t = *e->cTransform;
        auto &b = *e->cBounds;

        t.pos.x += t.velocity.x;
        t.pos.y += t.velocity.y;
        float radius = e->cCollision->radius;

        if (t.pos.x - radius < b.minX)
        {
            t.pos.x = b.minX + radius;
            t.velocity.x *= -1.f;
        }
        else if (t.pos.x + radius > b.maxX)
        {
            t.pos.x = b.maxX - radius;
            t.velocity.x *= -1.f;
        }

        if (t.pos.y - radius < b.minY)
        {
            t.pos.y = b.minY + radius;
            t.velocity.y *= -1.f;
        }
        else if (t.pos.y + radius > b.maxY)
        {
            t.pos.y = b.maxY - radius;
            t.velocity.y *= -1.f;
        }

    }
}


void Game::sLifespan(float deltaTime)
{
    for (auto &e : m_entities.getEntities())
    {
        if (!e || !e->cLifespan) continue;

        e->cLifespan->remaining -= deltaTime;
        if (e->cLifespan->remaining <= 0)
            e->destroy();
    }

    m_entities.removeDead();
}


void Game::sEnemySpawner(float deltaTime)
{
    m_enemySpawnTimer += deltaTime;

    if (m_enemySpawnTimer >= m_config.enemy.SI)
    {
        spawnEnemy();
        m_enemySpawnTimer = 0.f;
    }
}

void Game::sCollision()
{
    auto &bullets        = m_entities.getEntities("bullet");
    auto &specialBullets = m_entities.getEntities("special_bullet");
    auto &enemies        = m_entities.getEntities("enemy");
    auto &small_enemy    = m_entities.getEntities("small_enemy");
    auto &player         = m_entities.getEntities("player");

    for (auto &b : bullets)
    {
        if (!b || !b->cTransform || !b->cCollision) continue;

        Vec2 bPos = b->cTransform->pos;
        float bRadius = b->cCollision->radius;

        for (auto &e : enemies)
        {
            if (!e || !e->cTransform || !e->cCollision) continue;

            Vec2 ePos = e->cTransform->pos;
            float eRadius = e->cCollision->radius;

            float dx = bPos.x - ePos.x;
            float dy = bPos.y - ePos.y;
            float distanceSq = dx * dx + dy * dy;
            float radiusSum  = bRadius + eRadius;

            if (distanceSq <= radiusSum * radiusSum)
            {
                b->destroy();
                e->destroy();
                m_score += 100;

                if (e->cShape->circle.getRadius() > 8.f)
                {
                    spawnSmallEnemies(e);
                }
            }
        }

        for (auto &e : small_enemy)
        {
            if (!e || !e->cTransform || !e->cCollision) continue;

            Vec2 ePos = e->cTransform->pos;
            float eRadius = e->cCollision->radius;

            float dx = bPos.x - ePos.x;
            float dy = bPos.y - ePos.y;
            float distanceSq = dx * dx + dy * dy;
            float radiusSum  = bRadius + eRadius;

            if (distanceSq <= radiusSum * radiusSum)
            {
                b->destroy();
                e->destroy();
                m_score += 300;
            }
        }
    }

    for (auto &sb : specialBullets)
    {
        if (!sb || !sb->cTransform || !sb->cCollision) continue;

        Vec2 sbPos = sb->cTransform->pos;
        float sbRadius = sb->cCollision->radius;

        for (auto &e : enemies)
        {
            if (!e || !e->cTransform || !e->cCollision) continue;

            Vec2 ePos = e->cTransform->pos;
            float eRadius = e->cCollision->radius;

            float dx = sbPos.x - ePos.x;
            float dy = sbPos.y - ePos.y;
            float distanceSq = dx * dx + dy * dy;
            float radiusSum  = sbRadius + eRadius;

            if (distanceSq <= radiusSum * radiusSum)
            {
                e->destroy();
                m_score += 100;

                const int numBullets = 36;
                for (int i = 0; i < numBullets; i++)
                {
                    float angle = i * (2 * M_PI / numBullets);
                    Vec2 dir(std::cos(angle), std::sin(angle));
                    auto newBullet = m_entities.addEntity("bullet");
                    newBullet->cTransform = std::make_unique<CTransform>(
                        sbPos, dir * 6.f, 0.f
                    );
                    newBullet->cCollision = std::make_unique<CCollision>(4.f);
                    newBullet->cShape = std::make_unique<CShape>(
                        4.f, 12,
                        sf::Color::Yellow,
                        sf::Color::Yellow,
                        1.f
                    );
                    newBullet->cLifespan = std::make_unique<CLifespan>(m_config.bullet.L);
                }
                if (e->cShape->circle.getRadius() > 8.f)
                {
                    spawnSmallEnemies(e);
                }

                sb->destroy();
            }
        }
        for (auto &e : small_enemy)
        {
            if (!e || !e->cTransform || !e->cCollision) continue;

            Vec2 ePos = e->cTransform->pos;
            float eRadius = e->cCollision->radius;

            float dx = sbPos.x - ePos.x;
            float dy = sbPos.y - ePos.y;
            float distanceSq = dx * dx + dy * dy;
            float radiusSum  = sbRadius + eRadius;

            if (distanceSq <= radiusSum * radiusSum)
            {
                e->destroy();
                m_score += 300;
                sb->destroy();
            }
        }
    }
    for (auto &p : player)
    {
        if (!p || !p->cTransform || !p->cCollision) continue;

        Vec2 pPos = p->cTransform->pos;
        float pRadius = p->cCollision->radius;

        for (auto& e : enemies)
        {
            Vec2 ePos = e->cTransform->pos;
            float eRadius = e->cCollision->radius;

            float dx = pPos.x - ePos.x;
            float dy = pPos.y - ePos.y;
            float distanceSq = dx * dx + dy * dy;
            float radiusSum = pRadius + eRadius;

            if (distanceSq <= radiusSum * radiusSum)
            {
                e->destroy();
                p->destroy();
                spawnPlayer();

                m_score = std::max(0, (int)m_score - 1000);
            }
        }

        for (auto& e : small_enemy)
        {
            Vec2 ePos = e->cTransform->pos;
            float eRadius = e->cCollision->radius;

            float dx = pPos.x - ePos.x;
            float dy = pPos.y - ePos.y;
            float distanceSq = dx * dx + dy * dy;
            float radiusSum = pRadius + eRadius;

            if (distanceSq <= radiusSum * radiusSum)
            {
                e->destroy();
                p->destroy();
                spawnPlayer();

                m_score = std::max(0, (int)m_score - 1000);
            }
        }
    }
}

void Game::sBounds()
{
    for (auto& e : m_entities.getEntities())
    {
        if (!e || !e->cTransform || !e->cBounds) continue;

        float radius = 0.f;
        if (e->cCollision)
        {
            radius = e->cCollision->radius;
        }
        else if (e->cShape)
        {
            radius = e->cShape->circle.getRadius();
        }

        float left   = e->cBounds->minX + radius;
        float right  = e->cBounds->maxX - radius;
        float top    = e->cBounds->minY + radius;
        float bottom = e->cBounds->maxY - radius;

        auto& pos = e->cTransform->pos;

        if (pos.x < left)   pos.x = left;
        if (pos.x > right)  pos.x = right;
        if (pos.y < top)    pos.y = top;
        if (pos.y > bottom) pos.y = bottom;
    }
}


void Game::sRender()
{
    m_window.clear();

    for (auto &e : m_entities.getEntities())
    {
        if (!e || !e->cShape || !e->cTransform) continue;

        e->cShape->circle.setPosition(sf::Vector2f(e->cTransform->pos.x,e->cTransform->pos.y));
        e->cTransform->angle += 1.f;
        e->cShape->circle.setRotation(sf::degrees(e->cTransform->angle));
        m_window.draw(e->cShape->circle);
    }

    m_text->setString("Score: " + std::to_string(m_score));
    m_text->setPosition(sf::Vector2f(10.f, 10.f));
    m_window.draw(*m_text);

    m_window.display();
}

void Game::sUserInput ()
{
    while (auto eventOpt = m_window.pollEvent())
    {
        if (!eventOpt.has_value()) continue;
        const sf::Event &event = *eventOpt;

        if (event.is<sf::Event::Closed>())
        {
            m_running = false;
            m_window.close();
        }

        static bool pWasPressed = false;

        if (event.is<sf::Event::KeyPressed>())
        {
            auto keyEvent = event.getIf<sf::Event::KeyPressed>();
            if (!keyEvent || !m_player || !m_player->cInput) continue;

            if (keyEvent->scancode == sf::Keyboard::Scan::W) m_player->cInput->up = true;
            if (keyEvent->scancode == sf::Keyboard::Scan::S) m_player->cInput->down = true;
            if (keyEvent->scancode == sf::Keyboard::Scan::A) m_player->cInput->left = true;
            if (keyEvent->scancode == sf::Keyboard::Scan::D) m_player->cInput->right = true;

            if (keyEvent->scancode == sf::Keyboard::Scan::P)
            {
                if (!pWasPressed)
                {
                    setPaused(!m_paused);
                    pWasPressed = true;
                }
            }
        }

        if (event.is<sf::Event::KeyReleased>())
        {
            auto keyEvent = event.getIf<sf::Event::KeyReleased>();
            if (!keyEvent || !m_player || !m_player->cInput) continue;

            if (keyEvent->scancode == sf::Keyboard::Scan::W) m_player->cInput->up = false;
            if (keyEvent->scancode == sf::Keyboard::Scan::S) m_player->cInput->down = false;
            if (keyEvent->scancode == sf::Keyboard::Scan::A) m_player->cInput->left = false;
            if (keyEvent->scancode == sf::Keyboard::Scan::D) m_player->cInput->right = false;

            if (keyEvent->scancode == sf::Keyboard::Scan::P)
            {
                pWasPressed = false;
            }
        }

        if (event.is<sf::Event::MouseButtonPressed>())
        {
            auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
            if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left && m_player)
            {
                spawnBullet(m_player, Vec2(mouseEvent->position.x, mouseEvent->position.y));
            }
            if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Right && m_player)
            {
                m_specialCooldown = 3;
                if (m_specialClock.getElapsedTime().asSeconds() >= m_specialCooldown)
                {
                    spawnSpecialWeapon(m_player, Vec2(mouseEvent->position.x, mouseEvent->position.y));
                    m_specialClock.restart();
                }
            }

        }
    }
}
