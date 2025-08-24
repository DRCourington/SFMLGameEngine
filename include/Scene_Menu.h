#pragma once

#include "Scene.h"
#include "EntityManager.h"
#include <SFML/Graphics/Text.hpp>
#include <string>
#include <vector>
#include <memory>

class Scene_Menu : public Scene
{
protected:
    std::string              m_title;
    std::vector<std::string> m_menuStrings;
    std::vector<std::string> m_levelPaths;
    sf::Text                 m_menuText;
    std::size_t              m_selectMenuIndex = 0;

    void init() override;
    void update() override;
    void onEnd() override;
    void sDoAction(const Action& action) override;

public:
    explicit Scene_Menu(GameEngine* gameEngine);
    void sRender() override;
};
