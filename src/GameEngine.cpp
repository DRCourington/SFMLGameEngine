#include "../include/GameEngine.h"
#include "../include/Assets.h"
#include "../include/Scene_Play.h"
#include "../include/Scene_Menu.h"


GameEngine::GameEngine(const std::string & path)
{
    init(path);
}

void GameEngine::init(const std::string & path)
{
    m_assets.loadFromFile(path);

    m_window.create(sf::VideoMode({1280, 720}), "Definitely NOT Mario");
    m_window.setFramerateLimit(60);

    changeScene("Menu", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    if (auto it = m_sceneMap.find(m_currentScene); it != m_sceneMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool GameEngine::isRunning()
{
    return m_running & m_window.isOpen();
}

void GameEngine::run() {
    m_running = true;
    while (m_running && m_window.isOpen()) {
        sUserInput();
        update();

        m_window.clear();
        if (auto s = currentScene()) s->sRender();

        m_window.display();
    }
}

void GameEngine::sUserInput()
{
    while (auto ev = m_window.pollEvent()) {
    const auto& e = *ev;

    if (e.is<sf::Event::Closed>()) {
        quit();
        continue;
    }

    if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
        if (kp->scancode == sf::Keyboard::Scancode::X) {
            sf::Texture backbuffer(m_window.getSize());
            backbuffer.update(m_window);
            const bool ok = backbuffer.copyToImage().saveToFile("screenshot.png");
            (void)ok;
        }
    }

    if (e.is<sf::Event::KeyPressed>() || e.is<sf::Event::KeyReleased>()) {
        const bool pressed = e.is<sf::Event::KeyPressed>();
        const auto scancode = pressed
            ? e.getIf<sf::Event::KeyPressed>()->scancode
            : e.getIf<sf::Event::KeyReleased>()->scancode;
        const int key = static_cast<int>(scancode);

        const auto& map = currentScene()->getActionMap();
        const auto it = map.find(key);
        if (it != map.end()) {
            const auto at = pressed ? ActionType::Start : ActionType::End;
            currentScene()->doAction(Action(it->second, at));
        }
    }
}
}

void GameEngine::changeScene(const std::string& sceneName,
                             std::shared_ptr<Scene> scene,
                             bool endCurrentScene)
{
    if (endCurrentScene && !m_currentScene.empty()) {
        if (auto it = m_sceneMap.find(m_currentScene);
            it != m_sceneMap.end() && it->second) {
            it->second->onEnd();
        }
    }

    m_sceneMap[sceneName] = std::move(scene);
    m_currentScene = sceneName;

    if (auto it = m_sceneMap.find(m_currentScene);
        it != m_sceneMap.end() && it->second) {
        it->second->init();
    }
}

void GameEngine::quit()
{
    m_running = false;
    if (m_window.isOpen()) m_window.close();
}

void GameEngine::update() {
    if (auto s = currentScene()) s->update();
}
