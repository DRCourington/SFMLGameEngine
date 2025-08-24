#pragma once

#include "Assets.h"
#include "Scene.h"

#include <memory>

using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

class GameEngine
{

protected:

    sf::RenderWindow    m_window;
    Assets              m_assets;
    std::string         m_currentScene;
    SceneMap            m_sceneMap;
    size_t              m_simulationSpeed = 1;
    bool                m_running = true;

    void init(const std::string & path);
    void update();

    void sUserInput();

    std::shared_ptr<Scene> currentScene();

public:

    GameEngine(const std::string & path);

    void changeScene(const std::string & sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);

    void quit();
    void run();

    sf::RenderWindow& window() { return m_window; }
    const sf::RenderWindow& window() const { return m_window; }
    bool isRunning();
    Assets& assets() { return m_assets; }
    const Assets& assets() const { return m_assets; }
    Assets& getAssets() { return m_assets; }
    const Assets& getAssets() const { return m_assets; }
};
