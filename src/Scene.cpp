#include "../include/Scene.h"
#include "../include/GameEngine.h"
#include <SFML/Graphics.hpp>

Scene::Scene(GameEngine* g) : m_game(g) {}

void Scene::registerAction(int inputKey, const std::string& actionName) {
    m_actionMap[inputKey] = actionName;
}

const ActionMap& Scene::getActionMap() const { return m_actionMap; }

void Scene::setPaused(bool p) { m_paused = p; }

size_t Scene::width()  const { return static_cast<int>(m_game->window().getSize().x); }
size_t  Scene::height() const { return static_cast<int>(m_game->window().getSize().y); }

void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
    sf::Vertex verts[2];
    verts[0].position = sf::Vector2f(p1.x, p1.y);
    verts[1].position = sf::Vector2f(p2.x, p2.y);
    m_game->window().draw(verts, 2, sf::PrimitiveType::Lines);
}
