#pragma once

#include "Action.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Play : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
    };

public:

    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
    void sRender() override;
    void onEnd() override;
    void sDoAction(const Action& action) override;
    void update() override;

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid = false;
    sf::Vector2f            m_gridSize = {64, 64};
    sf::Text                m_gridText;
    float                   m_moveSpeed = 4.0f;

    Vec2 gridToMidPixel(float gridX, float gridY,
                        std::shared_ptr<Entity> entity = nullptr);

    void init() override;

    void loadLevel(const std::string & filename);
    void spawnBullet(std::shared_ptr<Entity> entity);
    void spawnPlayer();
    void spawnBlock(float px, float py, int col, int row, float scale);
    void sMovement();
    void sLifespan();
    void sCollision();
    void sDebug();
    void sAnimation();
};
