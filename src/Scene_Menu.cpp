#include "../include/Scene_Menu.h"
#include "../include/GameEngine.h"
#include "../include/Scene_Play.h"
#include <SFML/Window/Keyboard.hpp>

Scene_Menu::Scene_Menu(GameEngine* g)
: Scene(g)
, m_menuText(g->assets().getFont("Tech"), "", 24) {}

void Scene_Menu::init() {
    m_title = "COMP4300 Demo";
    m_menuStrings = {"Play", "Quit"};
    m_levelPaths  = {"../levels/level1.txt", ""};

    registerAction(static_cast<int>(sf::Keyboard::Scancode::Up),           "UP");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Down),         "DOWN");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Enter),        "SELECT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::NumpadEnter),  "SELECT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Escape),       "QUIT");
}

void Scene_Menu::update() {}

void Scene_Menu::sRender() {
    auto& win = m_game->window();

    // title
    sf::Text title(m_game->assets().getFont("Tech"), m_title, 32);
    title.setPosition({40.f, 40.f});
    win.draw(title);

    // options
    float y = 120.f;
    for (std::size_t i = 0; i < m_menuStrings.size(); ++i) {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setCharacterSize(24);
        m_menuText.setPosition({60.f, y});
        if (i == m_selectMenuIndex) m_menuText.setStyle(sf::Text::Style::Bold);
        else                        m_menuText.setStyle(sf::Text::Style::Regular);
        win.draw(m_menuText);
        y += 36.f;
    }
}

void Scene_Menu::sDoAction(const Action& a) {
    if (a.isStart()) {
        if (a.name() == "UP"    && m_selectMenuIndex > 0)                    --m_selectMenuIndex;
        if (a.name() == "DOWN"  && m_selectMenuIndex + 1 < m_menuStrings.size()) ++m_selectMenuIndex;
        if (a.name() == "SELECT") {
            if (m_menuStrings[m_selectMenuIndex] == "Play") {
                m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[0]), true);
            } else if (m_menuStrings[m_selectMenuIndex] == "Quit") {
                m_game->quit();
            }
        }
        if (a.name() == "QUIT") m_game->quit();
    }
}

void Scene_Menu::onEnd() {}
